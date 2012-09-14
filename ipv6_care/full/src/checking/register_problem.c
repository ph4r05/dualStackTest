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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
extern int errno;

#include "filesystem.h"
#include "append_to_string.h"
#include "stack_tools.h"
#include "log.h"
#include "register_problem.h"
#include "interpreted_language.h"
#include "common_fd_tools.h"

#define __IPV6_COMPLIANCE_ERRORS_DIR 	"possible_ipv6_related_errors"
#define __IPV6_COMPLIANCE_WARNINGS_DIR 	"possible_ipv6_related_warnings"
#define __PROBLEM_DESCRIPTION_FILENAME 	"problem_description"

extern int interpreted_language;

// this function writes an IPv6 compliance warning, with its description and
// the current process stack, and writes the log
void write_problem(int is_an_error, char *title, char *content)
{
	FILE *file;
	char *directory_path_alloc, *directory_path;
	char *full_filename_alloc;

	directory_path = get_or_create_the_directory_related_to_the_thread();
	if (directory_path != NULL)
	{	
		asprintf(&directory_path_alloc, "%s/%s/%s", directory_path, 
			(is_an_error == ERROR) ? __IPV6_COMPLIANCE_ERRORS_DIR : __IPV6_COMPLIANCE_WARNINGS_DIR, 
			title);
	
		// try to create the directory
		if (recursive_mkdir(directory_path_alloc) != 0)
		{	// error already reported in recursive_mkdir()
			free(directory_path_alloc);
			return;
		}
		else
		{
			// write stack 
			write_stack_file(directory_path_alloc);

			// create problem description filename
			asprintf(&full_filename_alloc, "%s/%s", directory_path_alloc, __PROBLEM_DESCRIPTION_FILENAME); 
			free(directory_path_alloc);

			// open the file
			file = fopen(full_filename_alloc, "w");
			free(full_filename_alloc);
				
			// write content
			fprintf(file, "%s", content);
			if (interpreted_language == 1)
			{
				fprintf(file, "%s", WARNING_INTERPRETED_LANGUAGE);
			}
			
			// close
			fclose(file);

			log_if_needed();
		}
	}
}

