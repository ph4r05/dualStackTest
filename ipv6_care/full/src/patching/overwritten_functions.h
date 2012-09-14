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

#ifndef __OVERWRITTEN_FUNCTIONS_H__
#define __OVERWRITTEN_FUNCTIONS_H__

#include <netdb.h>
#include <sys/socket.h>
#include <poll.h>
#include <sys/select.h>
#include <netinet/in.h>

int overwritten_accept(int socket, struct sockaddr *address,
              socklen_t *address_len);
int overwritten_bind(int socket, const struct sockaddr *address,
              socklen_t address_len);
int overwritten_close(int fd);
int overwritten_connect(int s, const struct sockaddr *address,
              socklen_t address_len);
int overwritten_getaddrinfo(const char *nodename,
		const char *servname,
		const struct addrinfo *hints,
		struct addrinfo **res);
struct hostent *overwritten_gethostbyaddr(const void *addr, socklen_t len, int type);
GETHOSTBYXXXX_R_RETURN_TYPE overwritten_gethostbyaddr_r( GETHOSTBYADDR_R_ARGS_WITH_TYPES );
struct hostent *overwritten_gethostbyname(const char *name);
GETHOSTBYXXXX_R_RETURN_TYPE overwritten_gethostbyname_r( GETHOSTBYNAME_R_ARGS_WITH_TYPES );
int overwritten_getnameinfo(const struct sockaddr *sa, socklen_t salen,
              char *node, socklen_t nodelen, char *service,
              socklen_t servicelen, unsigned int flags);
int overwritten_getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int overwritten_getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
in_addr_t overwritten_inet_addr(const char *cp); 

int overwritten_inet_aton(const char *cp, struct in_addr *inp);
char *overwritten_inet_ntoa(struct in_addr in);
const char *overwritten_inet_ntop(int af, const void *src, char *dst, socklen_t size);
int overwritten_inet_pton(int af, const char *src, void *dst);
int overwritten_listen(int sockfd, int backlog);
int overwritten_poll(struct pollfd *fds, nfds_t nfds, int timeout);
int overwritten_ppoll(struct pollfd *fds, nfds_t nfds,
               const struct timespec *timeout, const sigset_t *sigmask);
int overwritten_pselect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds,
      const struct timespec *timeout, const sigset_t *sigmask);
int overwritten_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds,
      struct timeval *timeout);
int overwritten_setsockopt(int sockfd, int level, int optname,
                      const void *optval, socklen_t optlen);
int overwritten_socket(int domain, int type, int protocol);

#endif
