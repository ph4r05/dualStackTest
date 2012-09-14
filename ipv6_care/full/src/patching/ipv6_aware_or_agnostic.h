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
#ifndef __IPV6_AWARE_OR_AGNOSTIC_H__
#define __IPV6_AWARE_OR_AGNOSTIC_H__

#include "address.h"

enum ipv6_aware_or_agnostic_conversions
{
	from_ipv6_agnostic_to_ipv6_aware,
	from_ipv6_aware_to_ipv6_agnostic
};

char *return_converted_text_ip(char *text_ip, enum ipv6_aware_or_agnostic_conversions conversion);
struct polymorphic_addr *return_converted_pa(struct polymorphic_addr *pa, enum ipv6_aware_or_agnostic_conversions conversion);
int fill_pa_given_an_ipv6_aware_text_ip(char *text_ip, struct polymorphic_addr *pa);
#endif
