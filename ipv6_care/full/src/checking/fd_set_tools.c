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
#include <sys/socket.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>

#include "macros.h"
#include "common_networking_tools.h"
#include "common_original_functions.h"

extern __thread fd_set last_read_fds_storage;
extern __thread fd_set *last_read_fds;
extern __thread unsigned int last_read_nfds;

// These functions are useful for the management of fd-sets 
// in 'select' and 'pselect'
// ----------------------------------------------------------
void register_last_read_fds(fd_set *readfds, nfds_t nfds)
{
	if (last_read_fds == NULL)
	{
		last_read_fds = &last_read_fds_storage;
	}
	
	memcpy(last_read_fds, readfds, sizeof(*last_read_fds));
	last_read_nfds = nfds;
}


int test_if_accepting_only_IPv4(int socket)
{
	struct sockaddr_storage sas;
	unsigned int size = sizeof(sas);
	struct sockaddr *sa = (struct sockaddr *)&sas;

	// we will check that we are not accepting only IPv4 clients, i.e.:
	// 1) the socket is IPv4
	// 2) no select / poll involving both this socket and at least one IPv6 socket was previously done

	original_getsockname(socket, sa, &size);
	if (sa->sa_family == AF_INET)
	{	// test if a select / poll was previously done
		if (last_read_fds == NULL)
			return 1;
		// test if this previous select / poll involved this socket
		if (!FD_ISSET(socket, last_read_fds))
			return 1;
		// test if this previous select / poll also involved at least one IPv6 socket
		if (test_if_fd_set_contains_ipv6_sockets(last_read_nfds, last_read_fds) == 0)
			return 1;
	}

	return 0;
}

int test_if_fd_set_contains_ipv6_sockets(int nfds, fd_set *fds)
{
	int n;
	int result = 0; // false by default

	for (n=0; n<nfds; n++)
	{
		if ((fds != 0)&&(FD_ISSET(n, fds) != 0))
		{
			if (test_if_fd_is_an_ipv6_socket(n) == 1)
			{
				result = 1; // true
				break;
			}
		}
	}

	return result;
}

int test_if_fd_sets_contain_network_sockets(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds)
{
	int n;
	int result = 0; // false by default

	for (n=0; n<nfds; n++)
	{
		if ((readfds != 0)&&(FD_ISSET(n, readfds) != 0))
		{
			if (test_if_fd_is_a_network_socket(n) == 1)
			{
				result = 1; // true
				break;
			}
		}

		if ((writefds != 0)&&(FD_ISSET(n, writefds) != 0))
		{
			if (test_if_fd_is_a_network_socket(n) == 1)
			{
				result = 1; // true
				break;
			}
		}
		if ((errorfds != 0)&&(FD_ISSET(n, errorfds) != 0))
		{
			if (test_if_fd_is_a_network_socket(n) == 1)
			{
				result = 1; // true
				break;
			}
		}
	}

	return result;
}

void register_fd_sets_parameters(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds)
{
	char info_name[32];
	int n;

	for (n=0; n<nfds; n++)
	{
		if ((readfds != 0)&&(FD_ISSET(n, readfds) != 0))
		{
			if (test_if_fd_is_a_network_socket(n) == 1)
			{
				snprintf(info_name, sizeof(info_name) -1, "readfds[%d]", n);
				__REGISTER_INFO_CHARS(info_name, "1");
			}
		}

		if ((writefds != 0)&&(FD_ISSET(n, writefds) != 0))
		{
			if (test_if_fd_is_a_network_socket(n) == 1)
			{
				snprintf(info_name, sizeof(info_name) -1, "writefds[%d]", n);
				__REGISTER_INFO_CHARS(info_name, "1");
			}
		}
		if ((errorfds != 0)&&(FD_ISSET(n, errorfds) != 0))
		{
			if (test_if_fd_is_a_network_socket(n) == 1)
			{
				snprintf(info_name, sizeof(info_name) -1, "errorfds[%d]", n);
				__REGISTER_INFO_CHARS(info_name, "1");
			}
		}
	}
}

