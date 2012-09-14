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
#include <stdio.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
//#include <stdio.h>

#if !defined(__GLIBC__) || (__GLIBC__ < 2) || ((__GLIBC__ == 2) && (__GLIBC_MINOR__ < 4))
// The following function was introduced in glibc 2.4
// We define it manually for compatibility with older systems
int ppoll(struct pollfd *fds, nfds_t nfds,
               const struct timespec *timeout, const sigset_t *sigmask);
#endif

#include "common_macros.h"

/*
 * Each of the following functions has to be in the following format:
 * -----------------------------------------------------------------------
 * 
 * <return_type> original_<function>(<args>)
 * {
 *	 <return_type> result;
 *
 *       __CALL_ORIGINAL_FUNCTION(	FUNCTION_SYMBOL(<function>),
 *					ARGS(<function_args>),
 *					RETURN_VALUE_IF_FAILURE(<value>),
 *					RESULT(result));
 *	 return result;
 * }
 *
 */


// Start of the network-related functions
// --------------------------------------

int original_accept(int socket, struct sockaddr *address,
              socklen_t *address_len)
{
	int result;

	__CALL_ORIGINAL_FUNCTION(	FUNCTION_SYMBOL(accept),
					ARGS(socket, address, address_len),
					RETURN_VALUE_IF_FAILURE(-1),
					RESULT(result));
	return result;
}

int original_bind(int socket, const struct sockaddr *address,
              socklen_t address_len)
{
	int result;

	__CALL_ORIGINAL_FUNCTION(	FUNCTION_SYMBOL(bind),
					ARGS(socket, address, address_len),
					RETURN_VALUE_IF_FAILURE(-1),
					RESULT(result));
	return result;
}

int original_close(int fd)
{
	int result;

	__CALL_ORIGINAL_FUNCTION(	FUNCTION_SYMBOL(close),
					ARGS(fd),
					RETURN_VALUE_IF_FAILURE(-1),
					RESULT(result));
	return result;
}

int original_connect(int socket, const struct sockaddr *address,
              socklen_t address_len)
{
	int result;

	__CALL_ORIGINAL_FUNCTION(	FUNCTION_SYMBOL(connect),
					ARGS(socket, address, address_len),
					RETURN_VALUE_IF_FAILURE(-1),
					RESULT(result));
	return result;
}

void original_freeaddrinfo(struct addrinfo *res)
{
	__CALL_ORIGINAL_FUNCTION_RETURNING_VOID(	
					FUNCTION_SYMBOL(freeaddrinfo),
					ARGS(res));
}

int original_getaddrinfo(const char *nodename,
		const char *servname,
		const struct addrinfo *hints,
		struct addrinfo **res)
{
	int result;

	__CALL_ORIGINAL_FUNCTION(	FUNCTION_SYMBOL(getaddrinfo),
					ARGS(nodename, servname, hints, res),
					RETURN_VALUE_IF_FAILURE(-1),
					RESULT(result));
	return result;
}

struct hostent *original_gethostbyaddr(const void *addr, socklen_t len, int type)
{
	struct hostent *result;

	__CALL_ORIGINAL_FUNCTION(	FUNCTION_SYMBOL(gethostbyaddr),
					ARGS(addr, len, type),
					RETURN_VALUE_IF_FAILURE(NULL),
					RESULT(result));
	return result;
}

GETHOSTBYXXXX_R_RETURN_TYPE original_gethostbyaddr_r( GETHOSTBYADDR_R_ARGS_WITH_TYPES )
{
	GETHOSTBYXXXX_R_RETURN_TYPE function_result;

	__CALL_ORIGINAL_FUNCTION(	FUNCTION_SYMBOL(gethostbyaddr_r),
					ARGS( GETHOSTBYADDR_R_ARGS ),
					RETURN_VALUE_IF_FAILURE( GETHOSTBYXXXX_R_RETURN_VALUE_IF_NOT_FOUND ),
					RESULT(function_result), 
					TRY_SYMBOL_VERSIONS("GLIBC_2.1.2"));
	return function_result;
}

struct hostent *original_gethostbyname(const char *name)
{
	struct hostent *result;

	__CALL_ORIGINAL_FUNCTION(	FUNCTION_SYMBOL(gethostbyname),
					ARGS(name),
					RETURN_VALUE_IF_FAILURE(NULL),
					RESULT(result));
	return result;
}

GETHOSTBYXXXX_R_RETURN_TYPE original_gethostbyname_r( GETHOSTBYNAME_R_ARGS_WITH_TYPES )
{
	GETHOSTBYXXXX_R_RETURN_TYPE function_result;

	__CALL_ORIGINAL_FUNCTION(	FUNCTION_SYMBOL(gethostbyname_r),
					ARGS( GETHOSTBYNAME_R_ARGS ),
					RETURN_VALUE_IF_FAILURE( GETHOSTBYXXXX_R_RETURN_VALUE_IF_NOT_FOUND ),
					RESULT(function_result),
					TRY_SYMBOL_VERSIONS("GLIBC_2.1.2"));
	return function_result;
}

int original_getnameinfo(const struct sockaddr *sa, socklen_t salen,
              char *node, socklen_t nodelen, char *service,
              socklen_t servicelen, unsigned int flags)
{
	int result;

	__CALL_ORIGINAL_FUNCTION(	FUNCTION_SYMBOL(getnameinfo),
					ARGS(sa, salen, node, nodelen, service, servicelen, flags),
					RETURN_VALUE_IF_FAILURE(-1),
					RESULT(result));
	return result;
}

int original_getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
	int result;

	__CALL_ORIGINAL_FUNCTION(	FUNCTION_SYMBOL(getpeername),
					ARGS(sockfd, addr, addrlen),
					RETURN_VALUE_IF_FAILURE(-1),
					RESULT(result));
	return result;
}

int original_getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
	int result;

	__CALL_ORIGINAL_FUNCTION(	FUNCTION_SYMBOL(getsockname),
					ARGS(sockfd, addr, addrlen),
					RETURN_VALUE_IF_FAILURE(-1),
					RESULT(result));
	return result;
}

// no original_getsockopt() should be defined:
// the library should not have to call dlsym in order to find getsockopt()
// because getsockopt() is called in test_if_fd_is_a_network_socket() 
// which is used in many overwritten functions to avoid calling the library's code.
// for example, firefox redefines malloc() and calloc(), their malloc() calls close() which calls 
// test_if_fd_is_a_network_socket(), which calls getsockopt(). dlsym() calls calloc(), 
// so if getsockopt() calls dlsym() we have problems.

in_addr_t original_inet_addr(const char *cp)
{
	in_addr_t result;

	__CALL_ORIGINAL_FUNCTION(	FUNCTION_SYMBOL(inet_addr),
					ARGS(cp),
					RETURN_VALUE_IF_FAILURE(INADDR_NONE),
					RESULT(result));

	return result;
}

int original_inet_aton(const char *cp, struct in_addr *inp)
{
	int result;

	__CALL_ORIGINAL_FUNCTION(	FUNCTION_SYMBOL(inet_aton),
					ARGS(cp, inp),
					RETURN_VALUE_IF_FAILURE(-1),
					RESULT(result));
	return result;
}

char *original_inet_ntoa(struct in_addr in)
{
	char *result;

	__CALL_ORIGINAL_FUNCTION(	FUNCTION_SYMBOL(inet_ntoa),
					ARGS(in),
					RETURN_VALUE_IF_FAILURE(NULL),
					RESULT(result));
	return result;
}

const char *original_inet_ntop(int af, const void *src,
              char *dst, socklen_t size)
{
	const char *result;

	__CALL_ORIGINAL_FUNCTION(	FUNCTION_SYMBOL(inet_ntop),
					ARGS(af, src, dst, size),
					RETURN_VALUE_IF_FAILURE(NULL),
					RESULT(result));
	return result;
}

int original_inet_pton(int af, const char *src, void *dst)
{
	int result;

	__CALL_ORIGINAL_FUNCTION(	FUNCTION_SYMBOL(inet_pton),
					ARGS(af, src, dst),
					RETURN_VALUE_IF_FAILURE(-1),
					RESULT(result));
	return result;
}

int original_listen(int socket, int backlog)
{
	int result;

	__CALL_ORIGINAL_FUNCTION(	FUNCTION_SYMBOL(listen),
					ARGS(socket, backlog),
					RETURN_VALUE_IF_FAILURE(-1),
					RESULT(result));
	return result;
}

int original_poll(struct pollfd *fds, nfds_t nfds, int timeout)
{
	int result;

	__CALL_ORIGINAL_FUNCTION(	FUNCTION_SYMBOL(poll),
					ARGS(fds, nfds, timeout),
					RETURN_VALUE_IF_FAILURE(-1),
					RESULT(result));

	return result;
}

int original_ppoll(struct pollfd *fds, nfds_t nfds,
               const struct timespec *timeout, const sigset_t *sigmask)
{
	int result;

	__CALL_ORIGINAL_FUNCTION(	FUNCTION_SYMBOL(ppoll),
					ARGS(fds, nfds, timeout, sigmask),
					RETURN_VALUE_IF_FAILURE(-1),
					RESULT(result));

	return result;
}

int original_pselect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds,
      const struct timespec *timeout, const sigset_t *sigmask)
{
	int result;

	__CALL_ORIGINAL_FUNCTION(	FUNCTION_SYMBOL(pselect),
					ARGS(nfds, readfds, writefds, errorfds, timeout, sigmask),
					RETURN_VALUE_IF_FAILURE(-1),
					RESULT(result));

	return result;
}

ssize_t original_recvfrom(int sockfd, void *buf, size_t len, int flags,
                        struct sockaddr *src_addr, socklen_t *addrlen)
{
	int result;

	__CALL_ORIGINAL_FUNCTION(	FUNCTION_SYMBOL(recvfrom),
					ARGS(sockfd, buf, len, flags, src_addr, addrlen),
					RETURN_VALUE_IF_FAILURE(-1),
					RESULT(result));
	return result;
}

ssize_t original_recvmsg(int sockfd, struct msghdr *msg, int flags)
{
	int result;

	__CALL_ORIGINAL_FUNCTION(	FUNCTION_SYMBOL(recvmsg),
					ARGS(sockfd, msg, flags),
					RETURN_VALUE_IF_FAILURE(-1),
					RESULT(result));
	return result;
}

int original_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds,
      struct timeval *timeout)
{
	int result;

	__CALL_ORIGINAL_FUNCTION(	FUNCTION_SYMBOL(select),
					ARGS(nfds, readfds, writefds, errorfds, timeout),
					RETURN_VALUE_IF_FAILURE(-1),
					RESULT(result));
	return result;
}

ssize_t original_sendto(int sockfd, const void *buf, size_t len, int flags,
                      const struct sockaddr *dest_addr, socklen_t addrlen)
{
	int result;

	__CALL_ORIGINAL_FUNCTION(	FUNCTION_SYMBOL(sendto),
					ARGS(sockfd, buf, len, flags, dest_addr, addrlen),
					RETURN_VALUE_IF_FAILURE(-1),
					RESULT(result));
	return result;
}

ssize_t original_sendmsg(int socket, const struct msghdr *message, int flags)
{
	int result;

	__CALL_ORIGINAL_FUNCTION(	FUNCTION_SYMBOL(sendmsg),
					ARGS(socket, message, flags),
					RETURN_VALUE_IF_FAILURE(-1),
					RESULT(result));
	return result;
}


int original_setsockopt(int socket, int level, int option_name,
		const void *option_value, socklen_t option_len)
{
	int result;

	__CALL_ORIGINAL_FUNCTION(	FUNCTION_SYMBOL(setsockopt),
					ARGS(socket, level, option_name, option_value, option_len),
					RETURN_VALUE_IF_FAILURE(-1),
					RESULT(result));
	return result;
}

int original_socket(int domain, int type, int protocol)
{
	int result;

	__CALL_ORIGINAL_FUNCTION(	FUNCTION_SYMBOL(socket),
					ARGS(domain, type, protocol),
					RETURN_VALUE_IF_FAILURE(-1),
					RESULT(result));
	return result;
}
/*
FILE * original_fopen(const char *path, const char *mode)
{
	FILE *result;

	__CALL_ORIGINAL_FUNCTION(	FUNCTION_SYMBOL(fopen),
					ARGS(path, mode),
					RETURN_VALUE_IF_FAILURE(NULL),
					RESULT(result));
	return result;
}
*/
