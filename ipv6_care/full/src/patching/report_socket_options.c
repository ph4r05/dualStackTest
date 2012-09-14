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

#include <string.h>
#include "common_original_functions.h"
#include "socket_info.h"

#define REPORT_SOCKET_OPTION(type, option, old_socket, new_socket)			\
{											\
	type option_value;								\
	socklen_t option_size;								\
											\
	option_size = sizeof(option_value);						\
	if (getsockopt(old_socket, SOL_SOCKET, option, 			\
			(char *)&option_value, &option_size) == 0) {			\
		original_setsockopt(new_socket, SOL_SOCKET, option,			\
			(char *)&option_value, sizeof(option_value));			\
	}										\
}

// SO_RCVBUF and SO_SNDBUF are a particular case:
// the option is doubled by the kernel
void REPORT_SND_OR_RCV_BUF(int option, int old_socket, int new_socket)
{
	int option_value;
	socklen_t option_size;

	option_size = sizeof(option_value);
	if (getsockopt(old_socket, SOL_SOCKET, option, 
			(char *)&option_value, &option_size) == 0) {
		option_value /= 2;
		original_setsockopt(new_socket, SOL_SOCKET, option,
			(char *)&option_value, sizeof(option_value));
	}
}


void report_socket_options(int old_socket, int new_socket)
{
#if HAVE_SO_BINDTODEVICE
	struct ifreq *bound_interface;
#endif

	REPORT_SOCKET_OPTION(int, SO_BROADCAST, old_socket, new_socket);
	REPORT_SOCKET_OPTION(int, SO_DEBUG, old_socket, new_socket);
	REPORT_SOCKET_OPTION(int, SO_DONTROUTE, old_socket, new_socket);
	REPORT_SOCKET_OPTION(int, SO_KEEPALIVE, old_socket, new_socket);
	REPORT_SOCKET_OPTION(int, SO_OOBINLINE, old_socket, new_socket);
#if HAVE_SO_PRIORITY
	REPORT_SOCKET_OPTION(int, SO_PRIORITY, old_socket, new_socket);
#endif
	REPORT_SOCKET_OPTION(int, SO_REUSEADDR, old_socket, new_socket);
	REPORT_SOCKET_OPTION(struct linger, SO_LINGER, old_socket, new_socket);
	REPORT_SOCKET_OPTION(struct timeval, SO_RCVTIMEO, old_socket, new_socket);
	REPORT_SOCKET_OPTION(struct timeval, SO_SNDTIMEO, old_socket, new_socket);
	REPORT_SND_OR_RCV_BUF(SO_RCVBUF, old_socket, new_socket);
	REPORT_SND_OR_RCV_BUF(SO_SNDBUF, old_socket, new_socket);

#if HAVE_SO_BINDTODEVICE
	// it seems that getsockopt does not work with SO_BINDTODEVICE
	// so we use register_bound_interface() in overwritten_functions.c
	// and get_bound_interface() here (see socket_info.[hc])
	bound_interface = get_bound_interface(old_socket);
	if (strlen(bound_interface->ifr_ifrn.ifrn_name) > 0)
	{
		original_setsockopt(new_socket, SOL_SOCKET, SO_BINDTODEVICE,
			(char *)bound_interface, sizeof(struct ifreq));
	}
#endif
}

