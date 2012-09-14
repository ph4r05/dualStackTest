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
#include <string.h>

#include "listening_sockets.h"
#include "common_original_functions.h"
#include "common_networking_tools.h"
#include "created_sockets.h"
#include "addresses_and_names.h"

#define debug_print(...)

void manage_socket_accesses_on_fdset(int *nfds, fd_set *initial_fds, fd_set *final_fds)
{
	int fd, created_socket;
	int initial_nfds;

	initial_nfds = *nfds;

	FD_ZERO(final_fds);

	if (initial_fds != NULL)
	{
		for (fd = 0; fd < initial_nfds; fd++)
		{
			if (FD_ISSET(fd, initial_fds))
			{
				FD_SET(fd, final_fds);

				if (test_if_fd_is_a_network_socket(fd) == 1)
				{
					if (get_socket_state(fd) == socket_state_listening)
					{
						created_socket = get_additional_listening_socket_if_needed(fd);
						if (created_socket != -1)
						{	// a new socket was created, add it in the final_fds
							FD_SET(created_socket, final_fds);

							debug_print(1, "Created additional socket in order to wait on both IPv4 and IPv6 events.\n");

							// update nfds if needed
							if (created_socket >= *nfds)
							{
								*nfds = created_socket +1;
							}
						}
					}
				}
			}
		}
	}
}

void remap_changes_to_initial_fdset(int nfds, fd_set *initial_fds, fd_set *final_fds)
{
	int fd, initial_socket;
	fd_set resulting_initial_fds;

	// It seems doing a FD_ZERO on the provided initial_fds is a bit too intrusive.
	// It caused problems with ssh, there might be a programming bug in it which comes
	// to light when we try to FD_ZERO(initial_fds).
	// So we do it in a less intrusive way:
	// 1) We update a local fd_set called resulting_initial_fds
	// 2) We report values to initial_fds by changing only the values which are different

	if (initial_fds != NULL)
	{ 
		FD_ZERO(&resulting_initial_fds);

		// 1)

		for (fd = 0; fd < nfds; fd++)
		{
			if (FD_ISSET(fd, final_fds))
			{
				if (test_if_fd_is_a_network_socket(fd) == 1)
				{
					initial_socket = get_initial_socket_for_created_socket(fd);
					if (initial_socket < 0)
					{	// fd has no corresponding initial_socket
						FD_SET(fd, &resulting_initial_fds);
					}
					else
					{	// fd was created by IPv6 CARE, set its initial_socket in initial_fds
						FD_SET(initial_socket, &resulting_initial_fds);
					}
				}
				else
				{	// not a network socket => not managed by IPv6 CARE
					FD_SET(fd, &resulting_initial_fds);
				}
			}
		}

		// 2)

		for (fd = 0; fd < nfds; fd++)
		{
			if (FD_ISSET(fd, &resulting_initial_fds) && !FD_ISSET(fd, initial_fds))
			{
				FD_SET(fd, initial_fds);
			}

			if (!FD_ISSET(fd, &resulting_initial_fds) && FD_ISSET(fd, initial_fds))
			{
				FD_CLR(fd, initial_fds);
			}
		}
	}
}

void manage_socket_accesses_on_pollfd_table(int nfds, int *final_nfds, struct pollfd *fds, struct pollfd **final_fds)
{
	int allocated, new_nfds, fd, index;
	int created_socket;

	allocated = nfds+2;
	*final_fds = realloc(*final_fds, allocated*sizeof(struct pollfd));
	memcpy(*final_fds, fds, nfds*sizeof(struct pollfd));
	new_nfds = nfds;

	for (index = 0; index < nfds; index++)
	{
		fd = fds[index].fd;
		if (test_if_fd_is_a_network_socket(fd) == 1)
		{
			if (get_socket_state(fd) == socket_state_listening)
			{
				created_socket = get_additional_listening_socket_if_needed(fd);
				if (created_socket != -1)
				{	// a new socket was created, add it in the final_fds
					debug_print(1, "Created additional socket in order to wait on both IPv4 and IPv6 events.\n");

					// enlarge the table if needed
					if (new_nfds == allocated)
					{
						allocated = 2*allocated;
						*final_fds = realloc(*final_fds, allocated*sizeof(struct pollfd));
					}

					// copy the info and set the fd as the new socket
					memcpy(&(*final_fds)[new_nfds], &fds[index], sizeof(struct pollfd));
					(*final_fds)[new_nfds].fd = created_socket;
					new_nfds ++;
				}
			}
		}
	}

	*final_nfds = new_nfds;
}

void remap_changes_to_initial_pollfd_table(int nfds, int final_nfds, struct pollfd *initial_fds, struct pollfd *final_fds)
{
	int index, index2, fd, initial_socket;

	// start by copying back the nfds first elements
	memcpy(initial_fds, final_fds, nfds*sizeof(struct pollfd));

	// now manage the added elements
	for (index = nfds; index < final_nfds; index++)
	{
		if (final_fds[index].revents > 0) // if something happened there
		{
			fd = final_fds[index].fd;
			if (test_if_fd_is_a_network_socket(fd) == 1)
			{
				initial_socket = get_initial_socket_for_created_socket(fd);
				if (initial_socket >= 0)
				{	// fd was created by IPv6 CARE, set the flags on the initial socket

					// first, find where is this initial socket in the table
					for (index2 = 0; index2 < nfds; index2++)
					{
						if (final_fds[index2].fd == initial_socket)
						{
							// then, OR its flags
							initial_fds[index2].revents |= final_fds[index].revents;
							break;
						}
					}
				}
			}
		}
	}
}

