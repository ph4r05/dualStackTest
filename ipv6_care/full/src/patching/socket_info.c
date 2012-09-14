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
#include <errno.h>
#include <string.h>

#include "address.h"
#include "getxxxxname.h"
#include "socket_info.h"
#include "common_original_functions.h"

#define FLAG_DATA_REGISTERED_LOCAL_ADDRESS 		0x01
#define FLAG_DATA_REGISTERED_REMOTE_ADDRESS 		0x02
#define FLAG_DATA_REGISTERED_SOCKET_TYPE 		0x04
#define FLAG_DATA_REGISTERED_SOCKET_PROTOCOL 		0x08
#define FLAG_DATA_REGISTERED_SOCKET_BACKLOG 		0x10
#define FLAG_DATA_REGISTERED_SOCKET_STATE 		0x20
#define FLAG_DATA_REGISTERED_V6ONLY_OPTION		0x40
#define FLAG_DATA_REGISTERED_BOUND_INTERFACE		0x80

struct socket_data_listening
{
	int backlog;
	int v6only_option;
};

struct socket_data_communicating
{
	struct polymorphic_sockaddr remote_address;
};

union u_socket_data_per_state
{
	struct socket_data_listening listening;
	struct socket_data_communicating communicating;
};

struct socket_data
{
	int fd;
	int type;
	int protocol;
	enum socket_state state;
	struct polymorphic_sockaddr local_address;
	union u_socket_data_per_state data_per_state;
	int flag_data_registered;
#if HAVE_SO_BINDTODEVICE
	struct ifreq bound_interface;
#endif
};

struct socket_info_entry {
       struct socket_data data;
       LIST_ENTRY(socket_info_entry) entries;
};

LIST_HEAD(socket_info_list_head_type, socket_info_entry) socket_info_list_head;

int socket_info_list_initialised = 0;

void init_socket_info_list_if_needed()
{
	if (socket_info_list_initialised == 0)
	{
		LIST_INIT(&socket_info_list_head);
		socket_info_list_initialised = 1;
	}
}

struct socket_data *get_socket_info (int fd)
{
	struct socket_info_entry *entry;
	struct socket_data *result;
#ifdef BUGGY_OPTIMISATION
	// This optimisation causes a segfault and valgrind detects many errors, when patching NTP.
	// I don't know why. I let the code in place for future reference.
	static __thread struct socket_data *last_result = NULL;
	static __thread int last_fd = -1;

	if (fd != last_fd) // optimization in the case of several operations on the same fd
	{
#endif
		init_socket_info_list_if_needed ();

		result = NULL;
		for (entry = socket_info_list_head.lh_first; entry != NULL; entry = entry->entries.le_next)
		{
			if (entry->data.fd == fd)
			{	// socket already known
				result = &entry->data;
				break;
			}
		}

		if (result == NULL)
		{	// socket not known yet, register it
			entry = calloc (1, sizeof (struct socket_info_entry));
			entry->data.fd = fd;
			entry->data.flag_data_registered = 0;
			LIST_INSERT_HEAD(&socket_info_list_head, entry, entries);
				
			result = &entry->data;
		}

#ifdef BUGGY_OPTIMISATION
		last_fd = fd;
		last_result = result;
	}
	else
	{
		result = last_result;
	}
#endif
	return result;
}

void free_socket_info(int fd)
{
	struct socket_info_entry *entry;

	for (entry = socket_info_list_head.lh_first; entry != NULL; entry = entry->entries.le_next)
	{
		if (entry->data.fd == fd)
		{	
			LIST_REMOVE(entry, entries);
			free(entry);
			break;
		}
	}
}

void compute_socket_type (int fd, struct socket_data *data)
{
	unsigned int socktype_size;
	socktype_size = sizeof (data->type);
	getsockopt (fd, 1, 3, (char *) &data->type, &socktype_size);
}

void compute_listening_socket_v6only_option (int fd, struct socket_data *data)
{
	unsigned int sockoption_size;

	sockoption_size = sizeof (data->data_per_state.listening.v6only_option);
	getsockopt (fd, IPPROTO_IPV6, 26, (char *) &data->data_per_state.listening.v6only_option, &sockoption_size);
}

void compute_socket_protocol (int fd __attribute__ ((unused)), struct socket_data *data)
{	// I don't know any way to retrieve the protocol given the file descriptor
	// we will return 0 in this case
	data->protocol = 0;
}

void compute_listening_socket_backlog (int fd __attribute__ ((unused)), struct socket_data *data)
{	// I don't know any way to retrieve the backlog value given the file descriptor
	// we will return a big value in this case
	data->data_per_state.listening.backlog = 128;
}

void compute_socket_state (int fd, struct socket_data *data)
{
	unsigned int option_size, sas_size;
	unsigned int option_listening;
	union u_sockaddr sa;

	option_size = sizeof(option_listening);
	getsockopt (fd, SOL_SOCKET, SO_ACCEPTCONN, (char *) &option_listening, &option_size);
	if (option_listening == 1)
	{
		data->state = socket_state_listening;
	}
	else
	{
		sas_size = sizeof (sa.sas);
		if ((original_getpeername(fd, &sa.sa, &sas_size) == -1)
				&& (errno == ENOTCONN))
		{
			data->state = socket_state_created;
		}
		else
		{
			memcpy (&data->data_per_state.communicating.remote_address, &sa,
					sizeof (union u_sockaddr));
			data->state = socket_state_communicating;
			data->flag_data_registered |= FLAG_DATA_REGISTERED_REMOTE_ADDRESS;
		}
	}
}

void fill_address (int fd, struct polymorphic_sockaddr *psa,
	      address_filling_function_t function)
{
	psa->sa_len = sizeof (psa->sockaddr.sas);
	function(fd, &psa->sockaddr.sa, &psa->sa_len);
}

void compute_local_socket_address (int fd, struct socket_data *data)
{
	fill_address(fd, &data->local_address, original_getsockname);
}

void compute_remote_socket_address (int fd, struct socket_data *data)
{
	fill_address(fd, &data->data_per_state.communicating.remote_address, original_getpeername);
}

#if HAVE_SO_BINDTODEVICE
void compute_bound_interface (int fd __attribute__ ((unused)), struct socket_data *data)
{	// it seems that SO_BINDTODEVICE does not work with getsockopt
	// so we have no way to retrieve this
	memset(&data->bound_interface, 0, sizeof(data->bound_interface));
}
#endif

#define indirection_in_get0	
#define indirection_in_get1	&
#define indirection_in_get(_type_is_pointer)	indirection_in_get ## _type_is_pointer

#define __define_get_function(_suffix, _type, _type_is_pointer, _flag, _data_location) 	\
_type get_ ## _suffix (int fd)								\
{											\
  struct socket_data *data;								\
  data = get_socket_info (fd);								\
  if ((data->flag_data_registered & _flag) == 0)					\
    {											\
      compute_ ## _suffix (fd, data);							\
      data->flag_data_registered |= _flag;						\
    }											\
  return indirection_in_get(_type_is_pointer)_data_location;				\
}
	

#define copy_value0(_data_location, value)	_data_location = value
#define copy_value1(_data_location, value)	memcpy(&_data_location, value, sizeof (_data_location))
#define copy_value(_type_is_pointer, _data_location, value)	copy_value ## _type_is_pointer(_data_location, value)

#define __define_register_function(_suffix, _type, _type_is_pointer, _flag, _data_location)	\
void register_ ## _suffix (int fd, _type value)							\
{												\
  struct socket_data *data;									\
  data = get_socket_info (fd);									\
  copy_value(_type_is_pointer, _data_location, value);						\
  data->flag_data_registered |= _flag;								\
}


#define __define_get_and_register_functions(_suffix, _type, _type_is_pointer, _flag, _data_location)  	\
__define_get_function(_suffix, _type, _type_is_pointer, _flag, _data_location)				\
__define_register_function(_suffix, _type, _type_is_pointer, _flag, _data_location)

__define_get_and_register_functions(socket_type, int, 0, FLAG_DATA_REGISTERED_SOCKET_TYPE, data->type)
__define_get_and_register_functions(socket_state, enum socket_state, 0, FLAG_DATA_REGISTERED_SOCKET_STATE, data->state)
__define_get_and_register_functions(socket_protocol, int, 0, FLAG_DATA_REGISTERED_SOCKET_PROTOCOL, data->protocol)
__define_get_and_register_functions(listening_socket_backlog, int, 0, FLAG_DATA_REGISTERED_SOCKET_BACKLOG, 
						data->data_per_state.listening.backlog)
__define_get_and_register_functions(listening_socket_v6only_option, int, 0, FLAG_DATA_REGISTERED_V6ONLY_OPTION, 
						data->data_per_state.listening.v6only_option)
__define_get_and_register_functions(local_socket_address, struct polymorphic_sockaddr *, 1, FLAG_DATA_REGISTERED_LOCAL_ADDRESS, 
						data->local_address)
__define_get_and_register_functions(remote_socket_address, struct polymorphic_sockaddr *, 1, FLAG_DATA_REGISTERED_REMOTE_ADDRESS, 
						data->data_per_state.communicating.remote_address)
#if HAVE_SO_BINDTODEVICE
__define_get_and_register_functions(bound_interface, struct ifreq *, 1, FLAG_DATA_REGISTERED_BOUND_INTERFACE, 
						data->bound_interface)
#endif

