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

#include "common_original_functions.h"

// These functions are useful for networking operations
// ----------------------------------------------------

// this function returns 0 if fd is a socket, -1 otherwise
int test_if_fd_is_a_socket(int fd, struct sockaddr *sa, unsigned int *size)
{
	int optval;
	unsigned int opt_size;

	opt_size = sizeof(optval);
	if (getsockopt(fd, SOL_SOCKET, SO_TYPE, &optval, &opt_size) == -1)
	{
		return -1;
	}

	if ((optval != SOCK_STREAM) && (optval != SOCK_DGRAM))
	{
		return -1;
	}

	if(original_getsockname(fd, sa, size) == -1)
	{
		return -1;
	}

	return 0;
}

// this function returns 1 if the family of the socket is AF_INET6, 
// 0 otherwise, and -1 if an error occurs (fd is not a socket, etc.)
int test_if_fd_is_an_ipv6_socket(int fd)
{
	struct sockaddr_storage sas;
	unsigned int size = sizeof(sas);
	struct sockaddr *sa = (struct sockaddr *)&sas;
	int result;
	
	result = test_if_fd_is_a_socket(fd, sa, &size);
	
	if (result != 0)
	{
		return result;
	}

	if (sa->sa_family == AF_INET6)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

// this function returns 1 if the family of the socket is AF_INET or AF_INET6, 
// 0 otherwise, and -1 if an error occurs (fd is not a socket, etc.)
int test_if_fd_is_a_network_socket(int fd)
{
	struct sockaddr_storage sas;
	unsigned int size = sizeof(sas);
	struct sockaddr *sa = (struct sockaddr *)&sas;
	int result;
	
	result = test_if_fd_is_a_socket(fd, sa, &size);
	
	if (result != 0)
	{
		return result;
	}

	if ((sa->sa_family == AF_INET)||(sa->sa_family == AF_INET6))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

