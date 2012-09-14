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

#include <string.h>
#include <errno.h>
#include <unistd.h>


#include "family.h"
#include "socket_info.h"
#include "common_original_functions.h"
#include "fd_tools.h"
#include "ipv6_to_ipv4_mappings.h"
#include "socket_info.h"
#include "created_sockets.h"
#include "report_socket_options.h"

#define debug_print(...)

int reopen_socket_with_other_family(int s, int family)
{
	int type, protocol, result, saved_fd;

	result = 0;

	// duplicate the file descriptor
	saved_fd = dup(s);

	// record the type, protocol and fd of the existing socket
	type = get_socket_type(s);
	protocol = get_socket_protocol(s);
	
	// close the socket - this way the fd integer should be available for our new socket
	// and the calling program will not notice that the socket is not the same
	original_close(s);

	// create an IPv6 socket on the same file descriptor s
	if (create_socket_on_specified_free_fd(s, family, type, protocol) == -1)
	{	// failed ! recreate file descriptor previously closed
		dup2(saved_fd, s);
		result = -1;
	}
	else
	{
		report_socket_options(saved_fd, s);
	}

	// saved_fd is not useful anymore
	original_close(saved_fd);

	return result;
}

int try_connect_and_register_connection(int s, struct polymorphic_sockaddr *psa, 
				int *connect_call_result, int *connect_call_errno)
{
	int result = 0;

	// try to connect
	*connect_call_result = original_connect(s, &psa->sockaddr.sa, psa->sa_len);
	*connect_call_errno = errno;

	// EINPROGRESS means we have a non-blocking socket, so the call 
	// returned before the connection was completed. We consider it is ok in this case.
	if ((*connect_call_result == -1) && (*connect_call_errno != EINPROGRESS))
	{
		result = -1;
	}

	if (result == 0)
	{
		register_socket_state(s, socket_state_communicating);
		register_remote_socket_address(s, psa);
	}
	else
	{
		debug_print(1, "connection failed: %s\n", strerror(errno));
	}

	return result;
}

int try_connect_and_register_connection_and_manage_wrong_family(int s, 
				int current_socket_family, struct polymorphic_sockaddr *psa,
                                int *connect_call_result, int *connect_call_errno)
{
	int result = -1;
	int saved_fd;

	// duplicate the file descriptor
	saved_fd = dup(s);

	if (current_socket_family != psa->sockaddr.sa.sa_family)
	{
		if (reopen_socket_with_other_family(s, psa->sockaddr.sa.sa_family) == 0)
		{ 	// try to connect
			result = try_connect_and_register_connection(s, psa, connect_call_result, connect_call_errno);
			if (result == 0)
			{
				debug_print(1, "connection ok!\n");
				register_created_socket(INITIAL_SOCKET_WAS_CLOSED, s);
			}
			else
			{	// reopen socket as initially
				debug_print(1, "connection failed. recreating socket as ipv4.\n");
				original_close(s);
				dup2(saved_fd, s);
			}
		}
		else
		{	// could not recreate the socket as AF_INET6
			debug_print(1, "could not reopen socket as IPv6.\n");
			*connect_call_result = -1;
			*connect_call_errno = EHOSTUNREACH;
		}
	}
	else
	{
		result = try_connect_and_register_connection(s, psa, connect_call_result, connect_call_errno);
	}

	// saved_fd is not useful anymore
	original_close(saved_fd);

	return result;
}

