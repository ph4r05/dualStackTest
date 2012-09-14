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

#include "append_to_string.h"
#include "common_colors.h"
#include "common_fd_tools.h"

#define FAILURE_DIAGNOSIS_PART1 "** IPv6 CARE failed to"
#define FAILURE_DIAGNOSIS_PART2 "because"
#define FAILURE_DIAGNOSIS_PART3 ".\n** This usually means that a mechanism like AppArmor or SELinux is preventing this program to do this.\n"

extern char *ld_preload_value;

void unset_ld_preload()
{
	// save LD_PRELOAD if it was not done yet
	if (ld_preload_value == NULL)
	{
		asprintf(&ld_preload_value, "%s", getenv("LD_PRELOAD"));
	}

	unsetenv("LD_PRELOAD");
}

void set_ld_preload()
{
	setenv("LD_PRELOAD", ld_preload_value, 1);
}

void write_failure_diagnosis(char *description, char *reason)
{
	colored_print_if_tty(RED, 
			"** IPv6 CARE failed to %s because %s.\n"
		  	"** This usually means that a mechanism like AppArmor or SELinux is preventing this program to do this.",
		description, reason);
	PRINTF("\n"); 
	fflush(tty_fd);
}

int get_result_of_command(char **storage_string, char *command, char *description)
{
	FILE *fp;
	char c[2];
	int result;

	// we temporarily unset LD_PRELOAD (we don't want the 
	// subprocess created by popen to be monitored by ipv6-care)
	unset_ld_preload();

	c[1] = '\0';

	// let's run the command
	fp = popen(command, "r");
	if (fp == NULL)
	{
		// provide diagnosis in case of problems
		write_failure_diagnosis(description, "running the popen() function failed");
		result = -1;
	}
	else
	{
		// let's read its output
		while (!feof(fp))
		{
			fread(c, 1, 1, fp);
			if ((c[0] != '\n') && (c[0] != '\r'))
			{
				append_to_string(storage_string, c);
			}
		}
		result = 0;
	}
	pclose(fp);

	// we restore LD_PRELOAD
	set_ld_preload();

	return result;
}

int run_command(char *command, char *description)
{
	int result;

	// temporarily unset LD_PRELOAD (we don't want the 
	// subprocess created by system() to be monitored by ipv6-care)
	unset_ld_preload();

	// let's run the command
	result = system(command);

	// provide diagnosis in case of problems
	if (result != 0)
	{
		write_failure_diagnosis(description, "an attempt to create a subprocess executing a shell command failed");
	}
	
	// restore LD_PRELOAD
	set_ld_preload();

	return result;
}

