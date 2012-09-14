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
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "common_original_functions.h"
#include "socket_info.h"
#include "family.h"
#include "utils.h"
#include "ipv6_aware_or_agnostic.h"
#define MAX_HOST_SIZE	128
#define SA_LEN(paddress)        ((paddress)->sa_family == AF_INET)?(sizeof(struct sockaddr_in)):(sizeof(struct sockaddr_in6))

extern const struct in6_addr in6addr_any;
extern const struct in6_addr in6addr_loopback;

enum list_of_constant_pa
{
	any_addr_6,
	any_addr_4,
	loopback_6,
	loopback_4,
	number_of_constant_pa
};

// this function manage addresses of type loopback or any_addr
int get_equivalent_constant_address(struct polymorphic_sockaddr *data, struct polymorphic_sockaddr *new_data)
{
	static __thread struct polymorphic_addr constant_pa[number_of_constant_pa];
	static __thread int init_done = 0;
	struct in_addr ipv4_addr;
	struct polymorphic_addr pa, *corresponding_pa;
	int index, result;

	result = -1;

	if (init_done == 0)
	{
		copy_ipv6_addr_to_pa((struct in6_addr*)&in6addr_any, &constant_pa[any_addr_6]);
		copy_ipv6_addr_to_pa((struct in6_addr*)&in6addr_loopback, &constant_pa[loopback_6]);
		ipv4_addr.s_addr = INADDR_ANY;
		copy_ipv4_addr_to_pa(&ipv4_addr, &constant_pa[any_addr_4]);
		ipv4_addr.s_addr = INADDR_LOOPBACK;
		copy_ipv4_addr_to_pa(&ipv4_addr, &constant_pa[loopback_4]);
		init_done = 1;
	}

	copy_psa_to_pa(data, &pa);

	corresponding_pa = NULL;

	for (index = 0; index < number_of_constant_pa; index++)
	{
		if (compare_pa(&constant_pa[index], &pa) == 0)
		{
			if (index%2 == 0)
			{
				corresponding_pa = &constant_pa[index+1];
			}
			else
			{
				corresponding_pa = &constant_pa[index-1];
			}
			break;
		}
	}

	if (corresponding_pa != NULL)
	{
		copy_pa_and_port_to_psa(corresponding_pa, get_port_from_psa(data), new_data);
		result = 0;
	}

	return result;
}

int get_equivalent_address(struct polymorphic_sockaddr *data, struct polymorphic_sockaddr *new_data)
{
	int result;
	char host[MAX_HOST_SIZE];
	struct addrinfo hints, *address_list, *first_address;

	result = 0;
	
	// if get_equivalent_constant_address return 0 then we are done and 
	// we do not need any name resolution
	if (get_equivalent_constant_address(data, new_data) != 0)
	{
		// get the hostname
		result = original_getnameinfo(&data->sockaddr.sa, SA_LEN(&data->sockaddr.sa), host, MAX_HOST_SIZE, NULL, 0, NI_NUMERICSERV);
		if (result == 0)
		{
			// get its IP of the other family
			if (strcmp(host, "::") == 0)
			{
				strcpy(host, "0.0.0.0");
			}
			else
			{
				if (strcmp(host, "0.0.0.0") == 0)
				{
					strcpy(host, "::");
				}
			}
			
			memset(&hints, 0, sizeof(hints));
			hints.ai_family = OTHER_FAMILY(data->sockaddr.sa.sa_family);
			hints.ai_socktype = SOCK_STREAM; // could also be SOCK_DGRAM, but needs to be initialized
			result = original_getaddrinfo(host, NULL, &hints, &address_list);
			if (result == 0)
			{
				first_address = address_list;
				// copy address and port to new_data
				copy_sockaddr_to_psa(first_address->ai_addr, first_address->ai_addrlen, new_data);
				set_port_in_psa(new_data, get_port_from_psa(data));
				// free the memory
				original_freeaddrinfo(address_list);
			}
		}
	}
	return result;
}

#define INITIAL_SIZE_NEEDED(name) 									\
					((strlen(name) + 1) * sizeof(char) + 	/* name */		\
					2 * sizeof(char*) +	   		/* alias pointers */ 	\
					sizeof(struct in_addr *)) 		/* final addr pointer */

#define INCREMENT_SIZE_NEEDED 								\
					sizeof(struct in_addr) +   /* addr storage */	\
					sizeof(struct in_addr *)   /* addr pointer */

#define SET_ERRNO(h_errnop, errno) 	if (h_errnop != NULL) *h_errnop = errno;

#if HAVE_ENODATA 
#define ERRNO_ENODATA 	ENODATA
#else
#define ERRNO_ENODATA 	ENOMSG
#endif

#define GAI_ERROR_CASE(error, corresponding_errno, corresponding_herrno)	\
			error:							\
				function_result = corresponding_errno;		\
				SET_ERRNO(h_errnop, corresponding_herrno);	\
				break;

int ipv6_capable_gethostbyname_r(const char *name,
                struct hostent *ret, char *buf, size_t buflen,
                struct hostent **result, int *h_errnop)
{
	int function_result, getaddrinfo_result;
	struct addrinfo hints, *address_list, *paddress;
	struct polymorphic_sockaddr psa;
	struct polymorphic_addr pa, *new_pa;
	unsigned int size_needed;
	int family, num_addresses, addr_index;
	char *p_working_data, **p_working_data_char_pp;
	struct in_addr *addr_table, **addr_pointer_table, *in_addr_to_be_recorded;

	// getaddrinfo parameters
	memset(&hints, 0, sizeof(hints));       // init
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM; // could also be SOCK_DGRAM, but needs to be initialized

	getaddrinfo_result = original_getaddrinfo(name, NULL, &hints, &address_list);
	paddress = address_list;
	if (getaddrinfo_result == 0)
	{
		num_addresses = 0;
		size_needed = INITIAL_SIZE_NEEDED(name);

		// loop once to get the total size needed and number of addresses
		for (paddress = address_list; paddress != NULL; paddress = paddress->ai_next)
		{
			num_addresses++;
			size_needed += INCREMENT_SIZE_NEEDED;
		}

		if (buflen < size_needed)
		{
			function_result = ERANGE;
		}
		else
		{
			p_working_data = buf;
			// official name of host
			strcpy(p_working_data, name);
			ret->h_name = p_working_data;
			p_working_data += strlen(p_working_data) +1; // pass length + \0
			// alias list
			p_working_data_char_pp = (char **)p_working_data;
			p_working_data_char_pp[0] = ret->h_name;
			p_working_data_char_pp[1] = NULL;
			ret->h_aliases = p_working_data_char_pp;
			p_working_data = (char *)&p_working_data_char_pp[2];
			// host address type
			ret->h_addrtype = AF_INET;
			// length of address
			ret->h_length = sizeof(struct in_addr);
			// list of addresses: loop again over the results of getaddrinfo
			addr_pointer_table = (struct in_addr **)p_working_data;
			addr_table = (struct in_addr *)&addr_pointer_table[num_addresses +1];
			addr_index = 0;
			for (paddress = address_list; paddress != NULL; paddress = paddress->ai_next)
			{
				if (paddress->ai_family == AF_INET6)
				{
					copy_sockaddr_to_psa(paddress->ai_addr, paddress->ai_addrlen, &psa);
					copy_psa_to_pa(&psa, &pa);
					new_pa = return_converted_pa(&pa, from_ipv6_aware_to_ipv6_agnostic);
					in_addr_to_be_recorded = &new_pa->addr.ipv4_addr;
				}
				else
				{
					in_addr_to_be_recorded = &((struct sockaddr_in*)&paddress->ai_addr)->sin_addr;
				}
				
				memcpy(&addr_table[addr_index], in_addr_to_be_recorded, sizeof(struct in_addr)); // record address
				addr_pointer_table[addr_index] = &addr_table[addr_index]; // record pointer to address
				addr_index++;
			}
			addr_pointer_table[num_addresses] = NULL; 	// end of this table
			ret->h_addr_list = (char **)addr_pointer_table; // pointer to address list 

			function_result = 0;
		}
		// free the list
		original_freeaddrinfo(address_list);
	}
	else
	{
		switch (getaddrinfo_result)
		{
			// no ip address available
#if HAVE_EAI_ADDRFAMILY
			GAI_ERROR_CASE(case EAI_ADDRFAMILY, ERRNO_ENODATA, NO_ADDRESS)
#endif
#if HAVE_EAI_NODATA
			GAI_ERROR_CASE(case EAI_NODATA, ERRNO_ENODATA, NO_ADDRESS)
#endif
			// no host found with this name
#if HAVE_EAI_NONAME
			GAI_ERROR_CASE(case EAI_NONAME, ERRNO_ENODATA, HOST_NOT_FOUND)
#endif
			// try again
#if HAVE_EAI_AGAIN
			GAI_ERROR_CASE(case EAI_AGAIN, EAGAIN, TRY_AGAIN)
#endif
			// unrecoverable error: no more memory
#if HAVE_EAI_MEMORY
			GAI_ERROR_CASE(case EAI_MEMORY, ENOMEM, NO_RECOVERY)
#endif
			// unrecoverable error specified by errno
#if HAVE_EAI_SYSTEM
			GAI_ERROR_CASE(case EAI_SYSTEM, errno, NO_RECOVERY)
#endif
			// other unrecoverable errors
#if HAVE_EAI_FAIL
			GAI_ERROR_CASE(case EAI_FAIL, ERRNO_ENODATA, NO_RECOVERY)
#endif
			GAI_ERROR_CASE(default, ERRNO_ENODATA, NO_RECOVERY)
		}
	}

	if (function_result == 0)
	{
		*result = ret;
		SET_ERRNO(h_errnop, NETDB_SUCCESS);
	}
	else
	{
		*result = NULL;
	}

	return function_result;
}

