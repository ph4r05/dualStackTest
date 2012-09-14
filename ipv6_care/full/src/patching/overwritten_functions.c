/*
Copyright (c) Centre National de la Recherche Scientifique (CNRS,
France). 2010. 
Copyright (c) Members of the EGEE Collaboration. 2008-2010. See 
http://www.eu-egee.org/partners/ for details on the copyright
holders.  

Licensed under the Apache License, Version 2.0 (the "License"); 
you may not use this file except in compliance with the License. 
You may obtain a copy of the License at 

    http://www.apache.org/licenses/LICENSE-2.0 

Unless required by applicable law or agreed to in writing, software 
distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
See the License for the specific language governing permissions and 
limitations under the License.

---------------------------------------------------------------------
Developed by Etienne DUBLE - CNRS LIG (http://www.liglab.fr)
etienne __dot__ duble __at__ imag __dot__ fr
---------------------------------------------------------------------
*/
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>

#include "fd_tools.h"
#include "common_networking_tools.h"
#include "common_macros.h"
#include "address_name_matches.h"
#include "common_original_functions.h"
#include "ipv6_to_ipv4_mappings.h"
#include "connection_handling.h"
#include "family.h"
#include "utils.h"
#include "addresses_and_names.h"
#include "created_sockets.h"
#include "listening_sockets.h"
#include "select_and_poll.h"
#include "getxxxxname.h"
#include "ipv6_aware_or_agnostic.h"
#include "manage_hooks.h"
#include "overwritten_functions.h"

extern int h_errno;

#define debug_print(...) // future use

// The functions which are following are redefining the ones of libc.
// They are in alphabetical order.
// Some of these functions call some others. So we first need to declare 
// a few prototypes here for the functions which are called before they are defined.
int overwritten_inet_aton(const char *cp, struct in_addr *inp);
const char *overwritten_inet_ntop(int af, const void *src, char *dst, socklen_t size);

int overwritten_accept(int socket, struct sockaddr *address,
              socklen_t *address_len)
{
	int new_socket_created, resulting_socket, communication_socket, 
		got_connection_on_a_created_socket, this_is_a_network_socket;

	got_connection_on_a_created_socket = 0;
	resulting_socket = socket; // default
	this_is_a_network_socket = 0;

	if (test_if_fd_is_a_network_socket(socket) == 1)
	{
		new_socket_created = get_additional_listening_socket_if_needed(socket);
		this_is_a_network_socket = 1;

		if (new_socket_created != -1)
		{
			debug_print(1, "Created a new listening socket in order to listen to both IPv4 and IPv6 clients.\n");
			// wait on the two file descriptors
			resulting_socket = wait_on_two_sockets(socket, new_socket_created);

			if (resulting_socket == new_socket_created)
			{
				got_connection_on_a_created_socket = 1;
			}
		}
	}

	communication_socket = original_accept(resulting_socket, address, address_len);

	if ((this_is_a_network_socket == 1)&&(communication_socket != -1))
	{
		if (got_connection_on_a_created_socket == 1)
		{
			register_created_socket(COMM_SOCKET_OF_A_CREATED_SOCKET, communication_socket);
		}

		register_socket_state(communication_socket, socket_state_communicating);
		register_socket_type(communication_socket, get_socket_type(resulting_socket));
		register_socket_protocol(communication_socket, get_socket_protocol(resulting_socket));
	}

	return communication_socket;
}

int overwritten_bind(int socket, const struct sockaddr *address,
              socklen_t address_len)
{
	int result;
	struct polymorphic_sockaddr psa;

	result = original_bind(socket, address, address_len);
	if ((test_if_fd_is_a_network_socket(socket) == 1) && (result == 0))
	{
		copy_sockaddr_to_psa((struct sockaddr *)address, address_len, &psa);
		register_local_socket_address(socket, &psa);
	}

	return result;
}

int overwritten_close(int fd)
{
	int result;

	// we must try to get a very low intrusiveness with the close() call
	// because it is called very often and many times on descriptors which
	// are not sockets...
	if (test_if_fd_is_a_network_socket(fd) == 1)
	{
		close_sockets_related_to_fd(fd);
		result = original_close(fd);
		if (result == 0)
		{
			free_created_socket_data(fd); // if fd was created by IPv6 CARE
			free_socket_info(fd);
		}
	}
	else
	{
		result = original_close(fd);
	}

	return result;
}

int overwritten_connect(int s, const struct sockaddr *address,
              socklen_t address_len)
{
	int result;
	struct polymorphic_sockaddr original_psa, new_psa, other_psa, *succeeding_psa;
	struct polymorphic_addr original_pa, *new_pa;
	int connect_call_result, connect_call_errno;
	int connect_call_other_family_result, connect_call_other_family_errno;

	if (IS_AF_INET_OR_INET6(address->sa_family))
	{
		copy_sockaddr_to_psa((struct sockaddr *)address, address_len, &original_psa);
		copy_psa_to_pa(&original_psa, &original_pa);

		// convert to ipv6 aware data
		new_pa = return_converted_pa(&original_pa, from_ipv6_agnostic_to_ipv6_aware);
		copy_pa_and_port_to_psa(new_pa, get_port_from_psa(&original_psa), &new_psa);

		// try to connect to the host specified
		result = try_connect_and_register_connection_and_manage_wrong_family(s, 
			original_pa.family, &new_psa, &connect_call_result, &connect_call_errno);

		succeeding_psa = &new_psa; // default

		if (		(result == -1) &&
				IS_AF_INET_OR_INET6(address->sa_family) &&
				(get_equivalent_address(&new_psa, &other_psa) == 0))
		{	// try to connect to the host specified using an address of the other family
			debug_print(1, "Connection failed. But this host also as an address of the other family, trying it...\n");
			result = try_connect_and_register_connection_and_manage_wrong_family(s, 
					original_pa.family, &other_psa,
					&connect_call_other_family_result, &connect_call_other_family_errno);

			if (result == 0) // result is better
			{
				connect_call_result = connect_call_other_family_result;
				connect_call_errno = connect_call_other_family_errno;
				succeeding_psa = &other_psa;
			}
		}

		errno = connect_call_errno;

		if (connect_call_result == 0)
		{
			register_socket_state(s, socket_state_communicating);
			register_remote_socket_address(s, succeeding_psa);
		}
	}
	else
	{
		connect_call_result = original_connect(s, address, address_len);
	}


	return connect_call_result;
}

int overwritten_getaddrinfo(const char *nodename,
		const char *servname,
		const struct addrinfo *hints,
		struct addrinfo **res)
{
	int result;
//	struct addrinfo *paddress;

	result = original_getaddrinfo(nodename, servname, hints, res);
/*	
	if (result == 0)
	{
		for(paddress = *res; paddress != NULL; paddress = paddress->ai_next)
		{
			record_sa_address_name_match(paddress->ai_addr, (char *)nodename);
		}
	}
*/	return result;
}

#if GETHOSTBYXXXX_HOSTENT_RESULT_IS_RETURNED
#define TEST_RETURN_VALUE(value) 	(value != NULL)
#define ERROR_VALUE(func_result,errno) 	errno
#define RETURN_VALUE(gethostbyaddr_r_result, function_result) gethostbyaddr_r_result
#else
#define TEST_RETURN_VALUE(value)	(value == 0)
#define ERROR_VALUE(func_result,errno)	func_result
#define RETURN_VALUE(gethostbyaddr_r_result, function_result) function_result
#endif

struct hostent *overwritten_gethostbyaddr(const void *addr, socklen_t len, int type)
{
	static __thread struct hostent ret_storage;
	static __thread int buflen = 0;
	static __thread char *buf = NULL;
	int done = 0, *h_errnop = &h_errno;
	struct hostent *function_result, *ret = &ret_storage, **result = &function_result;
	GETHOSTBYXXXX_R_RETURN_TYPE gethostbyaddr_r_result;

	debug_print(1, "in gethostbyaddr\n");
	while(done == 0)
	{
		buflen += 10;
		buflen *= 2;
		buf = realloc(buf, buflen);
		// call the modified gethostbyaddr_r function below
		// GETHOSTBYADDR_R_ARGS = addr, len, type, ret, buf, buflen, [result,] h_errnop
		gethostbyaddr_r_result = overwritten_gethostbyaddr_r( GETHOSTBYADDR_R_ARGS );
		if TEST_RETURN_VALUE(gethostbyaddr_r_result)
		{
			done = 1;
		}
		else
		{
			if (ERROR_VALUE(gethostbyaddr_r_result, h_errno) != ERANGE)
			{
				function_result = NULL;
				done = 1;
			}
		}
	}

	return RETURN_VALUE(gethostbyaddr_r_result, function_result);
}

GETHOSTBYXXXX_R_RETURN_TYPE overwritten_gethostbyaddr_r( GETHOSTBYADDR_R_ARGS_WITH_TYPES )
{
	struct polymorphic_addr pa, *new_pa;

	debug_print(1, "in gethostbyaddr_r\n");

	if (type == AF_INET)
	{
		copy_ipv4_addr_to_pa((struct in_addr *)addr, &pa);
		new_pa = return_converted_pa(&pa, from_ipv6_agnostic_to_ipv6_aware);
		addr = (typeof(addr)) &new_pa->addr;
		len = new_pa->addr_len;
		type = new_pa->family;
	}

	return original_gethostbyaddr_r( GETHOSTBYADDR_R_ARGS );
}

/* Some systems return EINVAL instead of ERANGE when the following 
   is set really too low - don't touch it! */
#define INITIAL_BUFLEN	128

struct hostent *overwritten_gethostbyname(const char *name)
{
	static __thread struct hostent ret_storage;
	static __thread int buflen = 0;
	static __thread char *buf = NULL;
	int done = 0, *h_errnop = &h_errno;
	struct hostent *function_result, *ret = &ret_storage, **result = &function_result;
	GETHOSTBYXXXX_R_RETURN_TYPE gethostbyname_r_result;

	if (buflen == 0)
	{
		buflen = INITIAL_BUFLEN;
		buf = malloc(buflen);
	}

	while(done == 0)
	{
		buflen *= 2;
		debug_print(1, "buflen = %d\n", buflen);
		buf = realloc(buf, buflen);
		// call the modified gethostbyname_r function below
		// GETHOSTBYNAME_R_ARGS = name, ret, buf, buflen, [result,] h_errnop
		gethostbyname_r_result = overwritten_gethostbyname_r( GETHOSTBYNAME_R_ARGS );
		if TEST_RETURN_VALUE(gethostbyname_r_result)
		{
			done = 1;
		}
		else
		{
			if (ERROR_VALUE(gethostbyname_r_result, h_errno) != ERANGE)
			{
				function_result = NULL;
				done = 1;
			}
		}
	}

	return RETURN_VALUE(gethostbyname_r_result, function_result);
}

GETHOSTBYXXXX_R_RETURN_TYPE overwritten_gethostbyname_r( GETHOSTBYNAME_R_ARGS_WITH_TYPES )
{
#if GETHOSTBYXXXX_HOSTENT_RESULT_IS_RETURNED
	struct hostent *result_storage;
	ipv6_capable_gethostbyname_r(name, ret, buf, buflen, &result_storage, h_errnop);
	return result_storage;
#else
	return ipv6_capable_gethostbyname_r(name, ret, buf, buflen, result, h_errnop);
#endif
}

int overwritten_getnameinfo(const struct sockaddr *sa, socklen_t salen,
              char *node, socklen_t nodelen, char *service,
              socklen_t servicelen, unsigned int flags)
{
	int result;

	result = original_getnameinfo(sa, salen, node, nodelen, service, servicelen, flags);

/*	if ((node != NULL) && (nodelen != 0))
	{
		record_sa_address_name_match((struct sockaddr *)sa, node);
	}
*/	return result;
}

int overwritten_getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
	return getxxxxname(sockfd, addr, addrlen, original_getpeername);
}

int overwritten_getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
	return getxxxxname(sockfd, addr, addrlen, original_getsockname);
}

in_addr_t overwritten_inet_addr(const char *cp) 
{
	in_addr_t result;
	struct in_addr ipv4_addr;
	int conversion_result;

	// call the modified version of inet_aton below
	conversion_result = overwritten_inet_aton(cp, &ipv4_addr);

	if (conversion_result == 0)
	{
		result = INADDR_NONE;
	}
	else
	{
		result = ipv4_addr.s_addr;
	}
	
	return result;
}

int overwritten_inet_aton(const char *cp, struct in_addr *inp)
{
	int result;
	char *text_ip;
	struct polymorphic_addr pa, *new_pa;

	text_ip = return_converted_text_ip((char *)cp, from_ipv6_agnostic_to_ipv6_aware);
	if (fill_pa_given_an_ipv6_aware_text_ip(text_ip, &pa) == 0)
	{
		new_pa = return_converted_pa(&pa, from_ipv6_aware_to_ipv6_agnostic);
		inp->s_addr = new_pa->addr.ipv4_addr.s_addr;
		result = 1; // ok
	}
	else
	{
		result = 0; // not ok
	}
	return result;
}

char *overwritten_inet_ntoa(struct in_addr in)
{
	static __thread char buffer[INET6_ADDRSTRLEN];
	// call the modified inet_ntop below
	return (char *)overwritten_inet_ntop(AF_INET, (const void *)&in, buffer, INET6_ADDRSTRLEN);
}

const char *overwritten_inet_ntop(int af, const void *src, char *dst, socklen_t size)
{
	struct polymorphic_addr pa;
	struct mapping_data *mapping_data;
	const char *result = NULL;

	if (af == AF_INET)
	{
		copy_ipv4_addr_to_pa((struct in_addr *)src, &pa);
		mapping_data = get_mapping_for_address(mapped_ipv4_addr, &pa);
		if (mapping_data != NULL)
		{	// a mapping was registered, let's return the corresponding text form
			if (strlen(mapping_data->ip_text_forms[full_ipv6])+1 > size)
			{
				if (strlen(mapping_data->ip_text_forms[abbreviated_ipv6])+1 > size)
				{
					errno = ENOSPC;
					return NULL;
				}
				else
				{
					debug_print(1, "Retrieving abbreviated text-form of an IPv6 address...\n");
					result = mapping_data->ip_text_forms[abbreviated_ipv6];
				}
			}
			else
			{
				result = mapping_data->ip_text_forms[full_ipv6];
			}
		}
	}

	if (result == NULL)
	{
		result = original_inet_ntop(af, src, dst, size);
	}

	if ((result != NULL) && (dst != result))
	{
		strcpy(dst, result);
	}

	return result;
}

int overwritten_inet_pton(int af, const char *src, void *dst)
{
	if (af == AF_INET)
	{	// call the modified inet_aton (see above)
		return overwritten_inet_aton(src, (struct in_addr *)dst);
	}
	else
	{
		return original_inet_pton(af, src, dst);
	}
}

int overwritten_listen(int sockfd, int backlog)
{
	int result;

	result = original_listen(sockfd, backlog);
	if ((test_if_fd_is_a_network_socket(sockfd) == 1) && (result == 0))
	{
		register_listening_socket_backlog(sockfd, backlog);
		register_socket_state(sockfd, socket_state_listening);
	}

	return result;
}

int overwritten_poll(struct pollfd *fds, nfds_t nfds, int timeout)
{
	struct pollfd *final_fds;
	int final_nfds, result;
	int saved_errno;

	// indicate to manage_socket_accesses_on_pollfd_table that no allocation has been done yet
	final_fds = NULL;

	manage_socket_accesses_on_pollfd_table(nfds, &final_nfds, fds, &final_fds);

	result = original_poll(final_fds, final_nfds, timeout);
	saved_errno = errno;

	remap_changes_to_initial_pollfd_table(nfds, final_nfds, fds, final_fds);

	// free memory
	final_fds = realloc(final_fds, 0);

	errno = saved_errno; // in case it would have changed
	return result;
}

int overwritten_ppoll(struct pollfd *fds, nfds_t nfds,
               const struct timespec *timeout, const sigset_t *sigmask)
{
	struct pollfd *final_fds;
	int final_nfds, result;
	int saved_errno;

	// indicate to manage_socket_accesses_on_pollfd_table that no allocation has been done yet
	final_fds = NULL;

	manage_socket_accesses_on_pollfd_table(nfds, &final_nfds, fds, &final_fds);

	result = original_ppoll(final_fds, final_nfds, timeout, sigmask);
	saved_errno = errno;

	remap_changes_to_initial_pollfd_table(nfds, final_nfds, fds, final_fds);

	// free memory
	final_fds = realloc(final_fds, 0);

	errno = saved_errno; // in case it would have changed
	return result;
}

int overwritten_pselect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds,
      const struct timespec *timeout, const sigset_t *sigmask)
{
	int result;
	fd_set final_readfds, final_writefds, final_errorfds;
	int modifiable_nfds;
	int saved_errno;

	modifiable_nfds = nfds;

	manage_socket_accesses_on_fdset(&modifiable_nfds, readfds, &final_readfds);
	manage_socket_accesses_on_fdset(&modifiable_nfds, writefds, &final_writefds);
	manage_socket_accesses_on_fdset(&modifiable_nfds, errorfds, &final_errorfds);

	result = original_pselect(modifiable_nfds, &final_readfds, &final_writefds, &final_errorfds, timeout, sigmask);
	saved_errno = errno;

	remap_changes_to_initial_fdset(modifiable_nfds, readfds, &final_readfds);
	remap_changes_to_initial_fdset(modifiable_nfds, writefds, &final_writefds);
	remap_changes_to_initial_fdset(modifiable_nfds, errorfds, &final_errorfds);

	errno = saved_errno; // in case it would have changed
	return result;
}

int overwritten_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds,
      struct timeval *timeout)
{
	int result;
	fd_set final_readfds, final_writefds, final_errorfds;
	int modifiable_nfds;
	int saved_errno;
	
	modifiable_nfds = nfds;

	manage_socket_accesses_on_fdset(&modifiable_nfds, readfds, &final_readfds);
	manage_socket_accesses_on_fdset(&modifiable_nfds, writefds, &final_writefds);
	manage_socket_accesses_on_fdset(&modifiable_nfds, errorfds, &final_errorfds);

	result = original_select(modifiable_nfds, &final_readfds, &final_writefds, &final_errorfds, timeout);
	saved_errno = errno;

	remap_changes_to_initial_fdset(modifiable_nfds, readfds, &final_readfds);
	remap_changes_to_initial_fdset(modifiable_nfds, writefds, &final_writefds);
	remap_changes_to_initial_fdset(modifiable_nfds, errorfds, &final_errorfds);

	errno = saved_errno; // in case it would have changed
	return result;
}

int overwritten_setsockopt(int sockfd, int level, int optname,
                      const void *optval, socklen_t optlen)
{
	int created_socket, result;

	if (test_if_fd_is_a_network_socket(sockfd) == 1)
	{
		// also apply the option on the related created socket if any
		created_socket = get_created_socket_for_initial_socket(sockfd);
		if (created_socket != -1)
		{
			original_setsockopt(created_socket, level, optname, optval, optlen);
		}

		// call the function
		result = original_setsockopt(sockfd, level, optname, optval, optlen);

#if HAVE_SO_BINDTODEVICE
		// getsockopt with SO_BINDTODEVICE does not work so we record this here
		// for later retrieval
		if ((result == 0) && (level == SOL_SOCKET) && (optname == SO_BINDTODEVICE))
		{
			register_bound_interface(sockfd, (struct ifreq *)optval);

			if (created_socket != -1)
			{
				register_bound_interface(created_socket, (struct ifreq *)optval);
			}
		}
#endif
	}
	else
	{
		result = original_setsockopt(sockfd, level, optname, optval, optlen);
	}

	return result;
}

int overwritten_socket(int domain, int type, int protocol)
{
	int fd;

	fd = original_socket(domain, type, protocol);
	if ((IS_AF_INET_OR_INET6(domain)) && (fd != -1))
	{
		register_socket_state(fd, socket_state_created);
		register_socket_type(fd, type);
		register_socket_protocol(fd, protocol);
	}

	return fd;
}

