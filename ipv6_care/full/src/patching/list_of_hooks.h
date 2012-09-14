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

/* 
This file may be included several time in order to define
various things, depending on the definition of the HOOK macro.
Consequently DON'T SET #ifndef #define #endif HERE!
#ifndef __LIST_OF_HOOKS_H__
#define __LIST_OF_HOOKS_H__
*/

#define PROTOTYPE(p) p
#define PARAMETERS(p) p

HOOK(	accept,
	PROTOTYPE(int accept(int socket, struct sockaddr *address,
		      socklen_t *address_len)),
	PARAMETERS((socket, address, address_len)))

HOOK(	bind,
	PROTOTYPE(int bind(int socket, const struct sockaddr *address,
		      socklen_t address_len)),
	PARAMETERS((socket, address, address_len)))

HOOK(	close,
	PROTOTYPE(int close(int fd)),
	PARAMETERS((fd)))

HOOK(	connect,
	PROTOTYPE(int connect(int s, const struct sockaddr *address,
		      socklen_t address_len)),
	PARAMETERS((s, address, address_len)))

HOOK(	getaddrinfo,
	PROTOTYPE(int getaddrinfo(const char *nodename,
			const char *servname,
			const struct addrinfo *hints,
			struct addrinfo **res)),
	PARAMETERS((nodename, servname, hints, res)))

HOOK(	gethostbyaddr,
	PROTOTYPE(struct hostent *gethostbyaddr(const void *addr, socklen_t len, int type)),
	PARAMETERS((addr, len, type)))

HOOK(   gethostbyaddr_r,
	PROTOTYPE(GETHOSTBYXXXX_R_RETURN_TYPE gethostbyaddr_r(  GETHOSTBYADDR_R_ARGS_WITH_TYPES )),
	PARAMETERS((GETHOSTBYADDR_R_ARGS)))

HOOK(	gethostbyname,
	PROTOTYPE(struct hostent *gethostbyname(const char *name)),
	PARAMETERS((name)))

HOOK(   gethostbyname_r,
	PROTOTYPE(GETHOSTBYXXXX_R_RETURN_TYPE gethostbyname_r(  GETHOSTBYNAME_R_ARGS_WITH_TYPES )),
	PARAMETERS((GETHOSTBYNAME_R_ARGS)))

HOOK(	getnameinfo,
	PROTOTYPE(GETNAMEINFO_PROTOTYPE),
	PARAMETERS((sa, salen, node, nodelen, service, servicelen, flags)))

HOOK(	getpeername,
	PROTOTYPE(int getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen)),
	PARAMETERS((sockfd, addr, addrlen)))

HOOK(	getsockname,
	PROTOTYPE(int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen)),
	PARAMETERS((sockfd, addr, addrlen)))

HOOK(	inet_addr,
	PROTOTYPE(in_addr_t inet_addr(const char *cp)),
	PARAMETERS((cp)))

HOOK(	inet_aton,
	PROTOTYPE(int inet_aton(const char *cp, struct in_addr *inp)),
	PARAMETERS((cp, inp)))

HOOK(	inet_ntoa,
	PROTOTYPE(char *inet_ntoa(struct in_addr in)),
	PARAMETERS((in)))

HOOK(	inet_ntop,
	PROTOTYPE(const char *inet_ntop(int af, const void *src, char *dst, socklen_t size)),
	PARAMETERS((af, src, dst, size)))

HOOK(	inet_pton,
	PROTOTYPE(int inet_pton(int af, const char *src, void *dst)),
	PARAMETERS((af, src, dst)))

HOOK(	listen,
	PROTOTYPE(int listen(int sockfd, int backlog)),
	PARAMETERS((sockfd, backlog)))

HOOK(	poll,
	PROTOTYPE(int poll(struct pollfd *fds, nfds_t nfds, int timeout)),
	PARAMETERS((fds, nfds, timeout)))

HOOK(	ppoll,
	PROTOTYPE(int ppoll(struct pollfd *fds, nfds_t nfds,
		       const struct timespec *timeout, const sigset_t *sigmask)),
	PARAMETERS((fds, nfds, timeout, sigmask)))

HOOK(	pselect,
	PROTOTYPE(int pselect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds,
	      const struct timespec *timeout, const sigset_t *sigmask)),
	PARAMETERS((nfds, readfds, writefds, errorfds, timeout, sigmask)))

HOOK(	select,
	PROTOTYPE(int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds,
	      struct timeval *timeout)),
	PARAMETERS((nfds, readfds, writefds, errorfds, timeout)))

HOOK(	setsockopt,
	PROTOTYPE(int setsockopt(int sockfd, int level, int optname,
			      const void *optval, socklen_t optlen)),
	PARAMETERS((sockfd, level, optname, optval, optlen)))

HOOK(	socket,
	PROTOTYPE(int socket(int domain, int type, int protocol)),
	PARAMETERS((domain, type, protocol)))


