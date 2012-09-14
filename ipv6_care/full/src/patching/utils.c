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
#include <string.h>

#include <netdb.h>
#include <errno.h>

#include "socket_info.h"

#define _MIN(a, b) ((a<b)?a:b)
#define _COMPARE(a, b) ((a<b)?1:((a==b)?0:-1))

void copy_sockaddr_to_psa(struct sockaddr *address, unsigned int address_len, struct polymorphic_sockaddr *psa)
{
	memcpy(&psa->sockaddr.sa, address, address_len);
	psa->sa_len = address_len;
}

void copy_psa_to_sockaddr(struct polymorphic_sockaddr *psa, struct sockaddr *address, unsigned int *address_len)
{
	memcpy(address, &psa->sockaddr.sa, _MIN(*address_len, psa->sa_len));
	*address_len = psa->sa_len;
}

int get_port_from_psa(struct polymorphic_sockaddr *psa)
{
	if (psa->sockaddr.sa.sa_family == AF_INET)
	{
		return psa->sockaddr.sa_in.sin_port;
	}
	else
	{
		return psa->sockaddr.sa_in6.sin6_port;
	}
}

void set_port_in_psa(struct polymorphic_sockaddr *psa, int port)
{
	if (psa->sockaddr.sa.sa_family == AF_INET)
	{
		psa->sockaddr.sa_in.sin_port = port;
	}
	else
	{
		psa->sockaddr.sa_in6.sin6_port = port;
	}
}

void copy_ipv4_addr_to_pa(struct in_addr *address, struct polymorphic_addr *pa)
{
	memcpy(&pa->addr.ipv4_addr, address, sizeof(pa->addr.ipv4_addr));
	pa->addr_len = sizeof(pa->addr.ipv4_addr);
	pa->family = AF_INET;
	pa->void_pointer = (void*)&pa->addr;
}

void copy_ipv6_addr_to_pa(struct in6_addr *address, struct polymorphic_addr *pa)
{
	memcpy(&pa->addr.ipv6_addr, address, sizeof(pa->addr.ipv6_addr));
	pa->addr_len = sizeof(pa->addr.ipv6_addr);
	pa->family = AF_INET6;
	pa->void_pointer = (void*)&pa->addr;
}

int compare_pa(struct polymorphic_addr *pa1, struct polymorphic_addr *pa2)
{
	if (pa1->family != pa2->family)
	{
		return _COMPARE(pa1->family, pa2->family);
	}
	else
	{
		if (pa1->family == AF_INET)
		{
			return _COMPARE(pa1->addr.ipv4_addr.s_addr, pa2->addr.ipv4_addr.s_addr);
		}
		else
		{
			return memcmp(pa1->addr.ipv6_addr.s6_addr, pa2->addr.ipv6_addr.s6_addr, sizeof(pa1->addr.ipv6_addr.s6_addr));
		}
	}
}

void copy_psa_to_pa(struct polymorphic_sockaddr *psa, struct polymorphic_addr *pa)
{
	if (psa->sockaddr.sa.sa_family == AF_INET)
	{
		copy_ipv4_addr_to_pa(&psa->sockaddr.sa_in.sin_addr, pa);
	}
	else
	{
		copy_ipv6_addr_to_pa(&psa->sockaddr.sa_in6.sin6_addr, pa);
	}
}

void copy_pa_and_port_to_psa(struct polymorphic_addr *pa, unsigned int port, struct polymorphic_sockaddr *psa)
{
	memset(psa, 0, sizeof(struct polymorphic_sockaddr));
	if (pa->family == AF_INET)
	{
		psa->sa_len = sizeof(psa->sockaddr.sa_in);
		psa->sockaddr.sa.sa_family = AF_INET;
		psa->sockaddr.sa_in.sin_port = port;
		memcpy(&psa->sockaddr.sa_in.sin_addr, &pa->addr.ipv4_addr, sizeof(psa->sockaddr.sa_in.sin_addr));
	}
	else // AF_INET6
	{
		psa->sa_len = sizeof(psa->sockaddr.sa_in6);
		psa->sockaddr.sa.sa_family = AF_INET6;
		psa->sockaddr.sa_in6.sin6_port = port;
		memcpy(&psa->sockaddr.sa_in6.sin6_addr, &pa->addr.ipv6_addr, sizeof(psa->sockaddr.sa_in6.sin6_addr));
	}
}

void copy_ipv4_addr_and_port_to_psa(struct in_addr *ipv4_addr, unsigned int port, struct polymorphic_sockaddr *psa)
{
	struct polymorphic_addr pa;
	copy_ipv4_addr_to_pa(ipv4_addr, &pa);
	copy_pa_and_port_to_psa(&pa, port, psa);
}

void copy_ipv6_addr_and_port_to_psa(struct in6_addr *ipv6_addr, unsigned int port, struct polymorphic_sockaddr *psa)
{
	struct polymorphic_addr pa;
	copy_ipv6_addr_to_pa(ipv6_addr, &pa);
	copy_pa_and_port_to_psa(&pa, port, psa);
}

