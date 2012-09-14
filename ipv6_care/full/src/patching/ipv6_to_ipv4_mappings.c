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
#include <sys/queue.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>

#include "common_original_functions.h"
#include "ipv6_to_ipv4_mappings.h"
#include "utils.h"
#include "common_macros.h"

struct mapping_entry {
	struct mapping_data data;
	LIST_ENTRY(mapping_entry) entries;
};

LIST_HEAD(mapping_list_head_type, mapping_entry) mapping_list_head;

int mapping_list_initialised = 0;
unsigned int mapping_index = 0;

void init_mapping_list_if_needed()
{
	if (mapping_list_initialised == 0)
	{
		LIST_INIT(&mapping_list_head);
		mapping_list_initialised = 1;
	}
}

struct ipv4_pool_info {
	unsigned int start;
	unsigned int end;
};

void fill_mapping_data_for_ipv6_addr(struct polymorphic_addr *real_ipv6_pa, struct mapping_data *data)
{
	unsigned int my_index = mapping_index++; // this is considered atomic (in a multi-threaded program)
	struct in_addr ipv4_addr;
	int length;
	static __thread struct ipv4_pool_info pool;
	static __thread int pool_initialized = 0;

	if (pool_initialized == 0)
	{
		original_inet_aton(stringified(IPV4_POOL_START), &ipv4_addr);
		pool.start = inet_lnaof(ipv4_addr);
		original_inet_aton(stringified(IPV4_POOL_END), &ipv4_addr);
		pool.end = inet_lnaof(ipv4_addr);
		pool_initialized = 1;
	}
	
	// detect overflows...
	if (my_index > pool.end - pool.start)
	{
		printf("IPV6 CARE: IPV4 POOL OVERFLOW! YOU SHOULD DESACTIVATE IPV6 CARE.\n");
		printf("IPV6 CARE: THIS PROCESS MAY SHOW UNEXPECTED BEHAVIORS IN NAME RESOLUTIONS FROM NOW ON!\n");
		// maybe we are running a critical application, so we should not stop.
		// let's get a correct value, even if it's already used...
		my_index %= pool.end - pool.start + 1;
	}

	// pa[real_ipv6_addr]
	memcpy(&data->pa[real_ipv6_addr], real_ipv6_pa, sizeof(*real_ipv6_pa));

	// pa[mapped_ipv4_addr]
	ipv4_addr = inet_makeaddr(INADDR_ANY, pool.start + my_index);
	copy_ipv4_addr_to_pa(&ipv4_addr, &data->pa[mapped_ipv4_addr]);

	// ip_text_forms[mapped_ipv4]
	original_inet_ntop(AF_INET, &ipv4_addr, data->ip_text_forms[mapped_ipv4], INET_ADDRSTRLEN);

	// ip_text_forms[full_ipv6]
	original_inet_ntop(AF_INET6, &real_ipv6_pa->addr.ipv6_addr, data->ip_text_forms[full_ipv6], INET6_ADDRSTRLEN);

	// ip_text_forms[abbreviated_ipv6]
	if (strlen(data->ip_text_forms[full_ipv6]) > INET_ADDRSTRLEN -1)
	{
		data->ip_text_forms[abbreviated_ipv6][0] = '.';
		data->ip_text_forms[abbreviated_ipv6][1] = '.';
		length = strlen(data->ip_text_forms[full_ipv6]);
		strcpy(&data->ip_text_forms[abbreviated_ipv6][2], &data->ip_text_forms[full_ipv6][length - INET_ADDRSTRLEN +3]);
	}
	else
	{
		strcpy(data->ip_text_forms[abbreviated_ipv6], data->ip_text_forms[full_ipv6]);
	}
}

struct mapping_data *get_mapping_for_address(enum address_type_in_mapping type_of_address, struct polymorphic_addr *pa)
{
	struct mapping_entry *entry;
	struct mapping_data *result;

	init_mapping_list_if_needed();
	result = NULL;

	for (entry = mapping_list_head.lh_first; entry != NULL; entry = entry->entries.le_next)
	{
		if (compare_pa(&entry->data.pa[type_of_address], pa) == 0)
		{
			result = &entry->data;
			break;
		}
	}

	if ((result == NULL)&&(type_of_address == real_ipv6_addr))
	{
		entry = malloc(sizeof(struct mapping_entry));

		fill_mapping_data_for_ipv6_addr(pa, &entry->data);

		LIST_INSERT_HEAD(&mapping_list_head, entry, entries);
		result = &entry->data;
	}

	return result;
}

struct mapping_data *get_mapping_for_text_form(char *text_form)
{
	struct mapping_entry *entry;
	struct mapping_data *result;

	init_mapping_list_if_needed();
	result = NULL;
	int text_form_index;

	for (entry = mapping_list_head.lh_first; entry != NULL; entry = entry->entries.le_next)
	{
		for (text_form_index = 0; text_form_index < number_of_mapped_ip_text_forms; text_form_index++)
		{
			if (strncmp(entry->data.ip_text_forms[text_form_index], text_form, INET6_ADDRSTRLEN) == 0)
			{
				result = &entry->data;
				break;
			}
		}
	}

	return result;
}

