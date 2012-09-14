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
#ifndef __MACROS_H__
#define __MACROS_H__

#include "log.h"

#define __START_FUNCTION_CALL_ANALYSIS_OK					\
	(register_info_start((char *)__FUNCTION__) == 0)

#define __END_FUNCTION_CALL_ANALYSIS						\
	register_info_end();
	
#define __REGISTER_INFO_INT(name, value) 		register_info_int(name, value)
#define __REGISTER_INFO_CHARS(name, value) 		register_info_chars(name, (char *)value)
#endif
