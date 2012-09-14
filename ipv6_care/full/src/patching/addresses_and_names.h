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
#ifndef __ADDRESSES_AND_NAMES_H__
#define __ADDRESSES_AND_NAMES_H__

#include "socket_info.h"
int get_equivalent_address(struct polymorphic_sockaddr *data, struct polymorphic_sockaddr *new_data);
int ipv6_capable_gethostbyname_r(const char *name,
                struct hostent *ret, char *buf, size_t buflen,
                struct hostent **result, int *h_errnop);

#endif
