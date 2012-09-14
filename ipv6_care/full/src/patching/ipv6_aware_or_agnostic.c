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

#include "ipv6_aware_or_agnostic.h"
#include "ipv6_to_ipv4_mappings.h"
#include "utils.h"
#include "common_original_functions.h"

char *return_converted_text_ip(char *text_ip, enum ipv6_aware_or_agnostic_conversions conversion)
{
	struct mapping_data *mapping;
	enum mapped_ip_text_forms output_ip_text_form;
	char *new_text_ip = text_ip; // default

	mapping = get_mapping_for_text_form(text_ip);
	if (mapping != NULL)
	{
		if (conversion == from_ipv6_agnostic_to_ipv6_aware)
		{
			output_ip_text_form = full_ipv6;
		}
		else
		{	// from_ipv6_aware_to_ipv6_agnostic
			output_ip_text_form = abbreviated_ipv6;
		}

		new_text_ip = mapping->ip_text_forms[output_ip_text_form];
	}
	
	return new_text_ip;
}

struct polymorphic_addr *return_converted_pa(struct polymorphic_addr *pa, enum ipv6_aware_or_agnostic_conversions conversion)
{
	struct mapping_data *mapping;
	struct polymorphic_addr *new_pa = pa; // default

	if (conversion == from_ipv6_agnostic_to_ipv6_aware)
	{
		if (pa->family == AF_INET) // this should be the case when comming from an ipv6 agnostic world
		{
			mapping = get_mapping_for_address(mapped_ipv4_addr, pa);
			if (mapping != NULL)
			{
				new_pa = &mapping->pa[real_ipv6_addr];
			}
		}
	}
	else // from_ipv6_aware_to_ipv6_agnostic
	{
		if (pa->family == AF_INET6) // otherwise nothing to do
		{
			mapping = get_mapping_for_address(real_ipv6_addr, pa);
			new_pa = &mapping->pa[mapped_ipv4_addr];
		}
	}

	return new_pa;
}

int fill_pa_given_an_ipv6_aware_text_ip(char *text_ip, struct polymorphic_addr *pa)
{
	struct addrinfo hints, *address_list, *first_address;
	struct polymorphic_sockaddr psa;
	int result;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;		// AF_INET or AF_INET6
	hints.ai_socktype = SOCK_STREAM; 	// could also be SOCK_DGRAM, but needs to be initialized
	hints.ai_flags = AI_NUMERICHOST;	// no name resolution, just ip text to binary conversion

	result = original_getaddrinfo(text_ip, NULL, &hints, &address_list);
	if (result == 0)
	{
		first_address = address_list;
	
		// store in pa
		copy_sockaddr_to_psa(first_address->ai_addr, first_address->ai_addrlen, &psa);
		copy_psa_to_pa(&psa, pa);

		// free the memory
		original_freeaddrinfo(address_list);
	}

	return result;
}
