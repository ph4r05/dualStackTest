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

#include <stdlib.h>
#include <stdarg.h>

// Generic function used to append something to a string, 
// in a printf format. The string is dynamically reallocated if needed.
void append_to_string(char **string, char *format, ...)
{
        va_list arguments;
	char *new_string = NULL, *new_appended_string = NULL;

	// if the string is not yet allocated, we allocate an empty string
	if (*string == NULL)
	{
		asprintf(string, "%s", "");
	}

	// get the variable arguments in the arguments variable
        va_start(arguments, format);

	// format the string to be appended
	vasprintf(&new_string, format, arguments);

	// append it
	asprintf(&new_appended_string, "%s%s", *string, new_string);
	
	// free memory
	free(new_string);
	free(*string);
	
	// point to the resulting string
	*string = new_appended_string;
	
	// free the arguments variable
        va_end ( arguments ); 
}

