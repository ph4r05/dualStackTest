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
#ifndef __FD_SET_TOOLS_H__
#define __FD_SET_TOOLS_H__
#include <sys/types.h>

void register_last_read_fds(fd_set *readfds, nfds_t nfds);
int test_if_accepting_only_IPv4(int socket);
int test_if_fd_set_contains_ipv6_sockets(int nfds, fd_set *fds);
int test_if_fd_sets_contain_network_sockets(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds);
void register_fd_sets_parameters(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds);
#endif
