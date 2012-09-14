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

#include <unistd.h>
#include <string.h>
#include "common_original_functions.h"
#include "common_networking_tools.h"
#include "created_sockets.h"
#include "addresses_and_names.h"
#include "report_socket_options.h"

int get_additional_listening_socket_if_needed(int initial_socket)
{
	int on = 1, additional_listening_socket_needed;
	int created_socket;
	struct polymorphic_sockaddr *initial_socket_psa, created_socket_psa; 
	int type, protocol, backlog, v6only_option; 
	
	created_socket = -1;

	if (test_if_fd_is_a_network_socket(initial_socket) == 1)
	{
		created_socket = get_created_socket_for_initial_socket(initial_socket);

		if (created_socket != -1)
		{	// such a socket has already been created previously
			return created_socket;
		}
		else
		{
			initial_socket_psa = get_local_socket_address(initial_socket);

			additional_listening_socket_needed = 1;
			if (initial_socket_psa->sockaddr.sa.sa_family == AF_INET6) // the existing socket is an IPv6 socket
			{
				// retrieve the IPV6_V6ONLY option in order to know if IPv4 clients are also accepted
				if (get_listening_socket_v6only_option(initial_socket) == 0)
				{	// this IPv6 socket also accepts IPv4 connections, so no need to continue
					additional_listening_socket_needed = 0;
				}
			}

			if (additional_listening_socket_needed == 1)
			{
				if (get_equivalent_address(initial_socket_psa, &created_socket_psa) == 0)
				{
					type = get_socket_type(initial_socket);
					protocol = get_socket_protocol(initial_socket);
					created_socket = original_socket(created_socket_psa.sockaddr.sa.sa_family, type, protocol);

					if (created_socket != -1)
					{
						report_socket_options(initial_socket, created_socket);
						
						// in case of an IPv6 socket, being an additional socket 
						// (i.e. there is already another socket listening for IPv4 clients)
						// we must set IPV6_V6ONLY to 1 
						v6only_option = 0;
						if (created_socket_psa.sockaddr.sa.sa_family == AF_INET6)
						{
							original_setsockopt(created_socket, IPPROTO_IPV6, IPV6_V6ONLY, (char *)&on, sizeof(on));
							v6only_option = 1;
						}
						if (original_bind(created_socket, &created_socket_psa.sockaddr.sa, created_socket_psa.sa_len) == -1)
						{
							original_close(created_socket);
							created_socket = -1;
						}
					}
					if (created_socket != -1)
					{
						backlog = get_listening_socket_backlog(initial_socket);
						if (original_listen(created_socket, backlog) == -1)
						{
							original_close(created_socket);
							created_socket = -1;
						}
						else
						{
							// register all the data about the socket we created
							register_created_socket(initial_socket, created_socket);
							register_socket_type(created_socket, type);
							register_socket_state(created_socket, socket_state_listening);
							register_socket_protocol(created_socket, protocol);
							register_local_socket_address(created_socket, &created_socket_psa);
							register_listening_socket_backlog(created_socket, backlog);
							register_listening_socket_v6only_option(created_socket, v6only_option);
						}
					}
				}
			}
		}
	}
	return created_socket;
}

int wait_on_two_sockets(int socket1, int socket2)
{
	fd_set socket_list_mask;
	int nbfds;

	// prepare 'select' parameter
	FD_ZERO(&socket_list_mask);
	FD_SET(socket1, &socket_list_mask);
	FD_SET(socket2, &socket_list_mask);

	// wait on server sockets
	nbfds = original_select((socket1>socket2)?(socket1+1):(socket2+1), &socket_list_mask, NULL, NULL, NULL);

	// get the server socket which woke up the 'select'
	if (FD_ISSET(socket1, &socket_list_mask))
	{
		return socket1;
	}
	else
	{
		return socket2;
	}
}

