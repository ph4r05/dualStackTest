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

#include "created_sockets.h"

struct created_socket_data
{
	int created_socket;
	int initial_socket;
};

struct created_socket_entry {
	struct created_socket_data data;
	LIST_ENTRY(created_socket_entry) entries;
};

LIST_HEAD(created_socket_list_head_type, created_socket_entry) created_socket_list_head;

int created_socket_list_initialised = 0;

void init_created_socket_list_if_needed()
{
	if (created_socket_list_initialised == 0)
	{
		LIST_INIT(&created_socket_list_head);
		created_socket_list_initialised = 1;
	}
}

void register_created_socket(int initial_socket, int created_socket)
{
	struct created_socket_entry *entry;

	init_created_socket_list_if_needed();

	entry = malloc(sizeof(struct created_socket_entry));
	entry->data.initial_socket = initial_socket;
	entry->data.created_socket = created_socket;

	LIST_INSERT_HEAD(&created_socket_list_head, entry, entries);
}

int get_created_socket_for_initial_socket(int initial_socket)
{
	struct created_socket_entry *entry;
	int result;

	result = -1;

	for (entry = created_socket_list_head.lh_first; entry != NULL; entry = entry->entries.le_next)
	{
		if (entry->data.initial_socket == initial_socket)
		{
			result = entry->data.created_socket;
		}
	}

	return result;
}

int get_initial_socket_for_created_socket(int created_socket)
{
	struct created_socket_entry *entry;
	int result;

	result = -1;

	for (entry = created_socket_list_head.lh_first; entry != NULL; entry = entry->entries.le_next)
	{
		if (entry->data.created_socket == created_socket)
		{
			result = entry->data.initial_socket;
		}
	}

	return result;
}

void free_created_socket_data(int created_socket)
{
	struct created_socket_entry *entry;

	for (entry = created_socket_list_head.lh_first; entry != NULL; entry = entry->entries.le_next)
	{
		if (entry->data.created_socket == created_socket)
		{	
			LIST_REMOVE(entry, entries);
			free(entry);
			break;
		}
	}
}
