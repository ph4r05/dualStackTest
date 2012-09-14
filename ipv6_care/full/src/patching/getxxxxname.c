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
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>

#include "common_networking_tools.h"
#include "ipv6_to_ipv4_mappings.h"
#include "utils.h"
#include "created_sockets.h"
#include "getxxxxname.h"
#include "common_original_functions.h"
#include "socket_info.h"
#include "ipv6_aware_or_agnostic.h"

void register_socket_address(int sockfd, struct polymorphic_sockaddr *psa, address_filling_function_t original_function)
{
	if (original_function == original_getsockname)
	{
		register_local_socket_address(sockfd, psa);
	}
	else
	{
		register_remote_socket_address(sockfd, psa);
	}
}

int getxxxxname(int sockfd, struct sockaddr *addr, socklen_t *addrlen, address_filling_function_t original_function)
{
	int initial_socket, original_function_result;
	struct polymorphic_sockaddr created_socket_psa, resulting_psa;
	struct polymorphic_addr created_socket_pa, *new_pa;
	
	if (test_if_fd_is_a_network_socket(sockfd) == 1)
	{
		initial_socket = get_initial_socket_for_created_socket(sockfd);
		if (initial_socket == -1)
		{	// sockfd was not created by IPv6 CARE
			original_function_result = original_function(sockfd, addr, addrlen);
			if (original_function_result != -1)
			{
				copy_sockaddr_to_psa(addr, *addrlen, &resulting_psa);
				register_socket_address(sockfd, &resulting_psa, original_function);
			}
			return original_function_result;
		}
		else
		{	// sockfd was created by IPv6 CARE
			created_socket_psa.sa_len = sizeof(created_socket_psa.sockaddr.sas);
			original_function_result = original_function(sockfd, 
						&created_socket_psa.sockaddr.sa, &created_socket_psa.sa_len);
			if (original_function_result == -1)
			{
				return -1;
			}
			else
			{
				// convert to ipv6 agnostic data
				copy_psa_to_pa(&created_socket_psa, &created_socket_pa);
				new_pa = return_converted_pa(&created_socket_pa, from_ipv6_aware_to_ipv6_agnostic);

				// copy to arguments
				copy_pa_and_port_to_psa(new_pa, get_port_from_psa(&created_socket_psa), &resulting_psa);
				copy_psa_to_sockaddr(&resulting_psa, addr, addrlen);

				// register data
				register_socket_address(sockfd, &resulting_psa, original_function);
				return 0;
			}
		}
	}
	else
	{	// not a network socket
		return original_function(sockfd, addr, addrlen);
	}
}

