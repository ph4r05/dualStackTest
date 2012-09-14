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

#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>

void record_inet6_address_name_match(struct in6_addr *in6_a, char *node)
{
	// TO DO
	// disable compiler warnings
	in6_a = in6_a;
	node = node;
}

void record_inet_address_name_match(struct in_addr *in_a, char *node)
{
	// TO DO
	// disable compiler warnings
	in_a = in_a;
	node = node;
}

void record_sa_address_name_match(struct sockaddr *sa, char *node)
{
	struct sockaddr_in *psockaddr_in;
	struct sockaddr_in6 *psockaddr_in6;

	switch(sa->sa_family)
	{
		case AF_INET:
			psockaddr_in = (struct sockaddr_in*)sa;
			record_inet_address_name_match(&psockaddr_in->sin_addr, node);
			break;
		case AF_INET6:
			psockaddr_in6 = (struct sockaddr_in6*)sa;
			record_inet6_address_name_match(&psockaddr_in6->sin6_addr, node);
			break;
	}
}

void record_hostent(struct hostent *he)
{
	int index;

	for (index = 0; he->h_addr_list[index] != NULL; index++)
	{
		switch (he->h_addrtype)
		{
			case AF_INET:
				record_inet_address_name_match((struct in_addr *)he->h_addr_list[index], he->h_name);
				break;
			case AF_INET6:
				record_inet6_address_name_match((struct in6_addr *)he->h_addr_list[index], he->h_name);
				break;
		}
	}
}

