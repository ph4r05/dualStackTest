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
#ifndef __SELECT_AND_POLL_H__
#define __SELECT_AND_POLL_H__

#include <sys/select.h>
#include <poll.h>

void manage_socket_accesses_on_fdset(int *nfds, fd_set *fds, fd_set *final_fds);
void manage_socket_accesses_on_pollfd_table(int nfds, int *final_nfds, struct pollfd *fds, struct pollfd **final_fds);
void remap_changes_to_initial_fdset(int nfds, fd_set *initial_fds, fd_set *final_fds);
void remap_changes_to_initial_pollfd_table(int nfds, int final_nfds, struct pollfd *initial_fds, struct pollfd *final_fds);

#endif
