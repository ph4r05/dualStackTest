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
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <poll.h>
#include <netinet/in.h>

#include "macros.h"
#include "problems.h"
#include "register_problem.h"
#include "networking_tools.h"
#include "fd_set_tools.h"
#include "pollfd_table_tools.h"
#include "stack_tools.h"
#include "common_networking_tools.h"
#include "common_original_functions.h"
#include "common_macros.h"


#define INTERPRETER_MAX_SIZE	32
#define IP_MAX_SIZE             64

extern char *interpreter_name; 

// Start of the network-related functions overriden
// ------------------------------------------------
PUBLIC_FUNCTION int accept(int socket, struct sockaddr *address,
              socklen_t *address_len)
{
	if (test_if_fd_is_a_network_socket(socket) == 1)
	{
		if (__START_FUNCTION_CALL_ANALYSIS_OK)
		{
			__REGISTER_INFO_INT("socket", socket);

			if (test_if_accepting_only_IPv4(socket) == 1)
			{
				write_problem(ERROR, ACCEPT_ONLY_IPV4_PROBLEM, ACCEPT_ONLY_IPV4_PROBLEM_DESCRIPTION);
			}
	
			__END_FUNCTION_CALL_ANALYSIS
		}
	}

	return original_accept(socket, address, address_len);
}

PUBLIC_FUNCTION int bind(int socket, const struct sockaddr *address,
              socklen_t address_len)
{
	char ip[IP_MAX_SIZE];
	int port;
	
	if (test_if_fd_is_a_network_socket(socket) == 1)
	{
		if (__START_FUNCTION_CALL_ANALYSIS_OK)
		{
			get_ip_string_and_port((struct sockaddr *)address, ip, IP_MAX_SIZE, &port);
			__REGISTER_INFO_INT("socket", socket);
			__REGISTER_INFO_CHARS("address.ip", ip);
			__REGISTER_INFO_INT("address.port", port);
			__END_FUNCTION_CALL_ANALYSIS
		}
	}
	
	return original_bind(socket, address, address_len);
}

PUBLIC_FUNCTION int close(int fd)
{
	if (test_if_fd_is_a_network_socket(fd) == 1)
	{
		if (__START_FUNCTION_CALL_ANALYSIS_OK)
		{
			__REGISTER_INFO_INT("fd", fd);
			__END_FUNCTION_CALL_ANALYSIS
		}
	}

	return original_close(fd);
}

PUBLIC_FUNCTION int connect(int socket, const struct sockaddr *address,
              socklen_t address_len)
{
	char ip[IP_MAX_SIZE];
	int port;

	if (test_if_fd_is_a_network_socket(socket) == 1)
	{
		if (__START_FUNCTION_CALL_ANALYSIS_OK)
		{
			get_ip_string_and_port((struct sockaddr *)address, ip, IP_MAX_SIZE, &port);
			__REGISTER_INFO_INT("socket", socket);
			__REGISTER_INFO_CHARS("address.ip", ip);
			__REGISTER_INFO_INT("address.port", port);
			__END_FUNCTION_CALL_ANALYSIS
		}
	}

	return original_connect(socket, address, address_len);
}

PUBLIC_FUNCTION void freeaddrinfo(struct addrinfo *res)
{
	if (__START_FUNCTION_CALL_ANALYSIS_OK)
	{
		// no interesting arguments to be logged, only the function itself will be logged
		__END_FUNCTION_CALL_ANALYSIS
	}
	
	original_freeaddrinfo(res);
}

PUBLIC_FUNCTION int getaddrinfo(const char *nodename,
		const char *servname,
		const struct addrinfo *hints,
		struct addrinfo **res)
{
	int ai_family, ai_socktype, ai_flags;

	if (__START_FUNCTION_CALL_ANALYSIS_OK)
	{
		if (hints == NULL)
		{	
			ai_family = 0;
			ai_socktype = 0;
			ai_flags = 0;
		}
		else
		{
			ai_family = hints->ai_family;
			ai_socktype = hints->ai_socktype;
			ai_flags = hints->ai_flags;
		}

		if ((ai_flags & AI_PASSIVE) != 0)
		{ // server
			__REGISTER_INFO_CHARS("ai_flags", "AI_PASSIVE");
		}

		__REGISTER_INFO_CHARS("ai_family", get_family_string(ai_family));
		__REGISTER_INFO_CHARS("ai_socktype", get_sock_type_string(ai_socktype));
		__REGISTER_INFO_CHARS("nodename", nodename);
		__REGISTER_INFO_CHARS("servname", servname);

		switch(ai_family)
		{
			case AF_INET:
				if (strcmp(interpreter_name, "python") == 0)
				{
					write_problem(WARNING, PYTHON_GETADDRINFO_AF_INET_PROBLEM, PYTHON_GETADDRINFO_AF_INET_PROBLEM_DESCRIPTION);
				}
				else
				{
					write_problem(WARNING, GETADDRINFO_AF_INET_PROBLEM, GETADDRINFO_AF_INET_PROBLEM_DESCRIPTION);
				}
				break;
			case AF_INET6:
				write_problem(WARNING, GETADDRINFO_AF_INET6_PROBLEM, GETADDRINFO_AF_INET6_PROBLEM_DESCRIPTION);
				break;
			case AF_UNSPEC:
				// ok
				break;
			default:
				// unknown value (ignored)
				break;
		}

		__END_FUNCTION_CALL_ANALYSIS
	}
	
	return original_getaddrinfo(nodename, servname, hints, res);
}

PUBLIC_FUNCTION struct hostent *gethostbyaddr(const void *addr, socklen_t len, int type)
{
	char ip[IP_MAX_SIZE];

	if (__START_FUNCTION_CALL_ANALYSIS_OK)
	{
		original_inet_ntop(type, addr, ip, IP_MAX_SIZE);
		__REGISTER_INFO_CHARS("addr.ip", ip);
		write_problem(WARNING, GETHOSTBYADDR_PROBLEM, GETHOSTBYADDR_PROBLEM_DESCRIPTION);

		__END_FUNCTION_CALL_ANALYSIS
	}
	
	return original_gethostbyaddr(addr, len, type);
}

PUBLIC_FUNCTION GETHOSTBYXXXX_R_RETURN_TYPE gethostbyaddr_r( GETHOSTBYADDR_R_ARGS_WITH_TYPES )
{
	char ip[IP_MAX_SIZE];

	if (__START_FUNCTION_CALL_ANALYSIS_OK)
	{
		original_inet_ntop(type, addr, ip, IP_MAX_SIZE);
		__REGISTER_INFO_CHARS("addr.ip", ip);
		write_problem(WARNING, GETHOSTBYADDR_R_PROBLEM, GETHOSTBYADDR_R_PROBLEM_DESCRIPTION);

		__END_FUNCTION_CALL_ANALYSIS
	}
	
	return original_gethostbyaddr_r( GETHOSTBYADDR_R_ARGS );
}

PUBLIC_FUNCTION struct hostent *gethostbyname(const char *name)
{
	if (__START_FUNCTION_CALL_ANALYSIS_OK)
	{
		__REGISTER_INFO_CHARS("name", name);
		write_problem(ERROR, GETHOSTBYNAME_PROBLEM, GETHOSTBYNAME_PROBLEM_DESCRIPTION);
		__END_FUNCTION_CALL_ANALYSIS
	}
	
	return original_gethostbyname(name);
}

PUBLIC_FUNCTION GETHOSTBYXXXX_R_RETURN_TYPE gethostbyname_r( GETHOSTBYNAME_R_ARGS_WITH_TYPES )
{
	if (__START_FUNCTION_CALL_ANALYSIS_OK)
	{
		__REGISTER_INFO_CHARS("name", name);

		if (strcmp(interpreter_name, "perl") == 0)
		{
			write_problem(ERROR, PERL_GETHOSTBYNAME_R_PROBLEM, PERL_GETHOSTBYNAME_R_PROBLEM_DESCRIPTION);
		}
		else
		{
			write_problem(ERROR, GETHOSTBYNAME_R_PROBLEM, GETHOSTBYNAME_R_PROBLEM_DESCRIPTION);
		}
		__END_FUNCTION_CALL_ANALYSIS
	}
	
	return original_gethostbyname_r( GETHOSTBYNAME_R_ARGS );
}

PUBLIC_FUNCTION GETNAMEINFO_PROTOTYPE
{
	char ip[IP_MAX_SIZE];
	int port;
	
	if (sa != NULL)		// this would be an error in the calling program
	{
		if (__START_FUNCTION_CALL_ANALYSIS_OK)
		{
			get_ip_string_and_port((struct sockaddr *)sa, ip, IP_MAX_SIZE, &port);
			__REGISTER_INFO_CHARS("sa.ip", ip);
			__REGISTER_INFO_INT("sa.port", port);
			__END_FUNCTION_CALL_ANALYSIS
		}
	}

	return original_getnameinfo(sa, salen, node, nodelen, service, servicelen, flags);
}

PUBLIC_FUNCTION int getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
	if (__START_FUNCTION_CALL_ANALYSIS_OK)
	{
		__REGISTER_INFO_INT("sockfd", sockfd);
		__END_FUNCTION_CALL_ANALYSIS
	}
	
	return original_getpeername(sockfd, addr, addrlen);
}

PUBLIC_FUNCTION int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
	if (__START_FUNCTION_CALL_ANALYSIS_OK)
	{
		__REGISTER_INFO_INT("sockfd", sockfd);
		__END_FUNCTION_CALL_ANALYSIS
	}
	
	return original_getsockname(sockfd, addr, addrlen);
}

PUBLIC_FUNCTION in_addr_t inet_addr(const char *cp)
{
	if (__START_FUNCTION_CALL_ANALYSIS_OK)
	{
		__REGISTER_INFO_CHARS("cp", cp);
		write_problem(ERROR, INET_ADDR_PROBLEM, INET_ADDR_PROBLEM_DESCRIPTION);
		__END_FUNCTION_CALL_ANALYSIS
	}
	
	return original_inet_addr(cp);
}

PUBLIC_FUNCTION int inet_aton(const char *cp, struct in_addr *inp)
{
	if (__START_FUNCTION_CALL_ANALYSIS_OK)
	{
		__REGISTER_INFO_CHARS("cp", cp);
		write_problem(ERROR, INET_ATON_PROBLEM, INET_ATON_PROBLEM_DESCRIPTION);
		__END_FUNCTION_CALL_ANALYSIS
	}
	
	return original_inet_aton(cp, inp);
}

PUBLIC_FUNCTION char *inet_ntoa(struct in_addr in)
{
	char ip[IP_MAX_SIZE];
	int port;
	struct sockaddr_storage sas;
	struct sockaddr_in *sa_in = (struct sockaddr_in *)&sas;

	if (__START_FUNCTION_CALL_ANALYSIS_OK)
	{
		// retrieve the ip for the log
		memset(&sas, 0, sizeof(struct sockaddr_storage));
		sa_in->sin_family = AF_INET;
		memcpy(&sa_in->sin_addr, &in, sizeof(struct in_addr)); 

		get_ip_string_and_port((struct sockaddr *)sa_in, ip, IP_MAX_SIZE, &port);

		__REGISTER_INFO_CHARS("in", ip);
		write_problem(ERROR, INET_NTOA_PROBLEM, INET_NTOA_PROBLEM_DESCRIPTION);
		__END_FUNCTION_CALL_ANALYSIS
	}
	
	return original_inet_ntoa(in);
}

PUBLIC_FUNCTION const char *inet_ntop(int af, const void *src,
              char *dst, socklen_t size)
{
	char ip[IP_MAX_SIZE];
	if (__START_FUNCTION_CALL_ANALYSIS_OK)
	{
		original_inet_ntop(af, src, ip, IP_MAX_SIZE);
		__REGISTER_INFO_CHARS("src", ip);
		write_problem(WARNING, INET_NTOP_PROBLEM, INET_NTOP_PROBLEM_DESCRIPTION);
		__END_FUNCTION_CALL_ANALYSIS
	}
	
	return original_inet_ntop(af, src, dst, size);
}

PUBLIC_FUNCTION int inet_pton(int af, const char *src, void *dst)
{
	if (__START_FUNCTION_CALL_ANALYSIS_OK)
	{
		__REGISTER_INFO_CHARS("src", src);
		write_problem(WARNING, INET_PTON_PROBLEM, INET_PTON_PROBLEM_DESCRIPTION);
		__END_FUNCTION_CALL_ANALYSIS
	}
	
	return original_inet_pton(af, src, dst);
}

PUBLIC_FUNCTION int listen(int socket, int backlog)
{
	if (test_if_fd_is_a_network_socket(socket) == 1)
	{
		if (__START_FUNCTION_CALL_ANALYSIS_OK)
		{
			__REGISTER_INFO_INT("socket", socket);
			__REGISTER_INFO_INT("backlog", backlog);
			__END_FUNCTION_CALL_ANALYSIS
		}
	}

	return original_listen(socket, backlog);
}

PUBLIC_FUNCTION int poll(struct pollfd *fds, nfds_t nfds, int timeout)
{
	if (test_if_pollfd_table_contain_network_sockets(fds, nfds))
	{
		register_last_read_pollfd_table(fds, nfds);
	
		if (__START_FUNCTION_CALL_ANALYSIS_OK)
		{
			register_pollfd_table_parameters(fds, nfds);
			__END_FUNCTION_CALL_ANALYSIS
		}
	}
	
	return original_poll(fds, nfds, timeout);
}

PUBLIC_FUNCTION int ppoll(struct pollfd *fds, nfds_t nfds,
               const struct timespec *timeout, const sigset_t *sigmask)
{
	if (test_if_pollfd_table_contain_network_sockets(fds, nfds))
	{
		register_last_read_pollfd_table(fds, nfds);

		if (__START_FUNCTION_CALL_ANALYSIS_OK)
		{
			register_pollfd_table_parameters(fds, nfds);
			__END_FUNCTION_CALL_ANALYSIS
		}
	}
	
	return original_ppoll(fds, nfds, timeout, sigmask);
}

PUBLIC_FUNCTION int pselect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds,
      const struct timespec *timeout, const sigset_t *sigmask)
{
	if (test_if_fd_sets_contain_network_sockets(nfds, readfds, writefds, errorfds))
	{
		register_last_read_fds(readfds, nfds);

		if (__START_FUNCTION_CALL_ANALYSIS_OK)
		{
			register_fd_sets_parameters(nfds, readfds, writefds, errorfds);
			__END_FUNCTION_CALL_ANALYSIS
		}
	}
	
	return original_pselect(nfds, readfds, writefds, errorfds, timeout, sigmask);
}

PUBLIC_FUNCTION ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
                        struct sockaddr *src_addr, socklen_t *addrlen)
{
	char ip[IP_MAX_SIZE];
	int port;

	if (test_if_fd_is_a_network_socket(sockfd) == 1)
	{
		if (__START_FUNCTION_CALL_ANALYSIS_OK)
		{
			get_ip_string_and_port(src_addr, ip, IP_MAX_SIZE, &port);
			__REGISTER_INFO_INT("sockfd", sockfd);
			__REGISTER_INFO_CHARS("src_addr.ip", ip);
			__REGISTER_INFO_INT("src_addr.port", port);
			__END_FUNCTION_CALL_ANALYSIS
		}
	}

	return original_recvfrom(sockfd, buf, len, flags, src_addr, addrlen);
}

PUBLIC_FUNCTION int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds,
      struct timeval *timeout)
{
	if (test_if_fd_sets_contain_network_sockets(nfds, readfds, writefds, errorfds))
	{
		register_last_read_fds(readfds, nfds);

		if (__START_FUNCTION_CALL_ANALYSIS_OK)
		{
			register_fd_sets_parameters(nfds, readfds, writefds, errorfds);
			__END_FUNCTION_CALL_ANALYSIS
		}
	}
	
	return original_select(nfds, readfds, writefds, errorfds, timeout);
}

PUBLIC_FUNCTION ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
                      const struct sockaddr *dest_addr, socklen_t addrlen)
{
	char ip[IP_MAX_SIZE];
	int port;

	if (test_if_fd_is_a_network_socket(sockfd) == 1)
	{
		if (__START_FUNCTION_CALL_ANALYSIS_OK)
		{
			get_ip_string_and_port((struct sockaddr *)dest_addr, ip, IP_MAX_SIZE, &port);
			__REGISTER_INFO_INT("sockfd", sockfd);
			__REGISTER_INFO_CHARS("dest_addr.ip", ip);
			__REGISTER_INFO_INT("dest_addr.port", port);
			__END_FUNCTION_CALL_ANALYSIS
		}
	}

	return original_sendto(sockfd, buf, len, flags, dest_addr, addrlen);
}

PUBLIC_FUNCTION int setsockopt(int socket, int level, int option_name,
		const void *option_value, socklen_t option_len)
{
	int value;
	
	if ((level == IPPROTO_IPV6) && (option_name == IPV6_V6ONLY))
	{
		if (__START_FUNCTION_CALL_ANALYSIS_OK)
		{
			value = *(int*)option_value;
			__REGISTER_INFO_INT("socket", socket);
			__REGISTER_INFO_CHARS("option_name", "IPV6_V6ONLY");
			__REGISTER_INFO_INT("value", value);
			__END_FUNCTION_CALL_ANALYSIS
		}
	}

	return original_setsockopt(socket, level, option_name, option_value, option_len);
}

PUBLIC_FUNCTION int socket(int domain, int type, int protocol)
{
	if ((domain == AF_INET)||(domain == AF_INET6))
	{
		if (__START_FUNCTION_CALL_ANALYSIS_OK)
		{
			__REGISTER_INFO_CHARS("domain", get_family_string(domain));
			__REGISTER_INFO_CHARS("type", get_sock_type_string(type));
			__REGISTER_INFO_CHARS("protocol", getprotobynumber(protocol)->p_name);
			__END_FUNCTION_CALL_ANALYSIS
		}
	}
	
	return original_socket(domain, type, protocol);
}

