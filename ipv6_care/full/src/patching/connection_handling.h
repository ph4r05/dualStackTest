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
#ifndef __CONNECTION_HANDLING_H__
#define __CONNECTION_HANDLING_H__

#include "address.h"

int try_connect_and_register_connection(int s, struct polymorphic_sockaddr *psa, 
					int *connect_call_result, int *connect_call_errno);
int try_connect_and_register_connection_and_manage_wrong_family(int s, 
					int current_socket_family, struct polymorphic_sockaddr *psa,
				        int *connect_call_result, int *connect_call_errno);

#endif
