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
#ifndef __SOCKET_INFO_H__
#define __SOCKET_INFO_H__

#if HAVE_SO_BINDTODEVICE
#include <net/if.h>
#endif

#include "address.h"

enum socket_state
{
	socket_state_created,
	socket_state_listening,
	socket_state_communicating
};

int get_socket_type(int fd);
void register_socket_type(int fd, int type);
enum socket_state get_socket_state(int fd);
void register_socket_state(int fd, enum socket_state state);
int get_socket_protocol(int fd);
void register_socket_protocol(int fd, int protocol);
int get_listening_socket_backlog(int fd);
void register_listening_socket_backlog(int fd, int backlog);
int get_listening_socket_v6only_option(int fd);
void register_listening_socket_v6only_option(int fd, int v6only_option);
struct polymorphic_sockaddr *get_local_socket_address(int fd);
void register_local_socket_address(int fd, struct polymorphic_sockaddr *sa);
struct polymorphic_sockaddr *get_remote_socket_address(int fd);
void register_remote_socket_address(int fd, struct polymorphic_sockaddr *sa);
#if HAVE_SO_BINDTODEVICE
struct ifreq *get_bound_interface(int fd);
void register_bound_interface(int fd, struct ifreq *interface);
#endif
void free_socket_info(int fd);

#endif
