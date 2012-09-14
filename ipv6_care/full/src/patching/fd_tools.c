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
#include "common_original_functions.h"
#include "created_sockets.h"

#define debug_print(...)

/*
	Consider the following scenario:

	// this is what the code of the program does
	... 				// initial state: fd == 0 -> stdin, fd == 1 -> stdout, fd == 2 -> stderr
	1- file = open(...); 		// => file = 3	
	2- s = socket(...IPv4...);	// => s = 4	
	3- close(file);			// => 3 is now free
	4- connect(s, ...) ... 		// this connect() call will call the IPv6 CARE handler:
		// this part is handled by IPv6 CARE 
		5- result = original_connect(s, ...);	// call of connect() of the libc
		   ... if this first connection attempt fails, IPv6 CARE will want to reopen s as an IPv6 socket:
		7- close(s);				// => 4 is now free 
		8- s = socket(...IPv6...);		// => s = 3, not 4!!! (because of line 3) 
		9- original_connect(s, ...);		// try to connect with the IPv6 socket
		
	In this case the calling program would be confused by the behavior of IPv6 CARE
	because the socket's fd would not be the same. 
	
	A correct code in order to reopen the socket on fd 4 would be, instead of line 7 and 8:
		close(s);               	// => 4 is now free 
		dummy_s = socket(...IPv6...); 	// => dummy_s = 3
		s = socket(...IPv6...); 	// => s = 4, ok
		close(dummy_s);	
	
	That's basically what the following code does. 
*/
int create_socket_on_specified_free_fd(int fd, int family, int socktype, int protocol)
{
	int result, new_socket;

	result = -1;

	debug_print(1, "Trying to create socket on fd = %d.\n", fd);

	// create the socket
	new_socket = original_socket(family, socktype, protocol);

	if (new_socket != -1)
	{	// if ok check if the file descriptor is the correct one
		if (new_socket == fd)
		{
			result = 0; // ok
			debug_print(1, "socket creation ok!\n");
		}
		else
		{
			// if not create a copy of the file descriptor
			// with the expected integer ...
			if (dup2(new_socket, fd) != -1)
			{	
				result = 0; // ok
				debug_print(1, "socket creation ok!\n");
			}

			// ... and close the original file descriptor
			original_close(new_socket);
		}
	}
	return result;
}

void close_sockets_related_to_fd(int fd)
{
	int created_socket;
	
	created_socket = get_created_socket_for_initial_socket(fd);
	if (created_socket != -1)
	{	// do not call the original function here, call the overwritten one
		close(created_socket);
	}
}

