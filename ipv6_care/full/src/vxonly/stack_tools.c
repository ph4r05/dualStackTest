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
#include <execinfo.h>
#include <string.h>
#include <libgen.h>

#include "append_to_string.h"
#include "interpreted_language.h"
#include "system_commands.h"

#define __MAX_STACK_TRACE		200
#define __PROCESS_STACKS_FILENAME 	"process_stacks"

extern int interpreted_language;
extern char *interpreter_name; 

// this function retrieves the details of the backtrace
void get_bt_data(void **array, int array_size, char ***strings, size_t *size)
{
	// get the current back trace of the program
	*size = backtrace (array, array_size);
	*strings = backtrace_symbols (array, *size);
}

// this function analyses a line of the backtrace and returns pointers 
// specifying where are written the object name and the address
void get_bt_line_info(char *line, char **name_start, char **name_end, 
			char **address_start, char **address_end)
{
	// check backtrace type
	if (strstr(line, " at ") != NULL)
	{	// freebsd
		*name_start = strrchr(line, ' ') +1;
		*name_end = line + strlen(line); // end of the line
		*address_start = line;
		*address_end = strchr(line, ' ');
	}
	else
	{ 	// linux or solaris
		*name_start = line;
		*name_end = line + strcspn(line, " ('");
		*address_start = strchr(line, '[') +1;
		*address_end = strchr(line, ']');
	}
}

// This function gets the name of the current executable
// in the case of an interpreted language, it will be the interpreter 
void save_interpreter_name()
{
	void *array[__MAX_STACK_TRACE];
	size_t size;
	char **strings, *name_start, *name_end, *address_start, *address_end;
	int index_first_character_after_name;

	// get the current backtrace of the program
	get_bt_data(array, __MAX_STACK_TRACE, &strings, &size);

	// last line will be the first program called
	get_bt_line_info(strings[size -1], &name_start, &name_end, &address_start, &address_end);
	*name_end = '\0';
	asprintf(&interpreter_name, "%s", basename(name_start));

	free (strings);
}

// This function writes the current process stack in a file in the given directory
void write_stack_file(char *directory)
{
	void *array[__MAX_STACK_TRACE];
	size_t size;
	char **strings, *name_start, *name_end, *address_start, *address_end;
	size_t i;
	char *command = NULL, *last_file = NULL, *index_hexa, *index_end_hexa;
	int index_first_character_after_name;

	// get the current backtrace of the program
	get_bt_data(array, __MAX_STACK_TRACE, &strings, &size);

	// start creating a shell command
	asprintf(&command, "LD_PRELOAD=\"\"; { ");

	// loop for each function of the stack
	for (i = 0; i < size; i++)
	{
		//printf("%d: %s\n", (int)i, strings[i]);

		// we do not take into account the functions within this 
		// ipv6 code checker library.
		if (strstr(strings[i], "libipv6_care") != NULL)
		{	// line ignored
			continue;
		}

		// we end the words
		get_bt_line_info(strings[i], &name_start, &name_end, &address_start, &address_end);
		*name_end = '\0';
		*address_end = '\0';

		// with the object file we have the position in this object file
		// as an hexadecimal number.
		// we want to use the addr2line command to know the name of the function
		// this point in the file corresponds to. 
		// in case the tested program was compiled with -g, we will also get
		// the source file name and line number.
		// since using addr2line can be expensive for execution time, 
		// we try to use it only one time for each different object file, by
		// appending all hexadecimal numbers we want to the same addr2line command.
		if ((last_file == NULL) || (strcmp(last_file, name_start) != 0))
		{
			if (last_file != NULL)
			{
				append_to_string(&command, "; ");
			}

			// in case the object file is not found in the given path we look for
			// it by using the 'whereis' command.
			append_to_string(&command, 	"location=\"%s\" ; echo obj \"$location\" ; "
							"if [ ! -f \"$location\" ] ; "
							"then "
								"set -- $(whereis -b \"$location\") ; "
								"location=\"$2\"; "
							"fi ; "
							"addr2line -f -C -e \"$location\"", name_start);
			last_file = name_start;
		}
		
		// append the hexadecimal number to this current addr2line command.
		append_to_string(&command, " %s", address_start);
	}

	// the end of the shell command. It:
	// - reformat the output
	// - detect if source localisation was found or not (and in this case add a comment)
	// - stops the stack after the function called 'main'.
	append_to_string(&command, "; } 2>/dev/null | {	 "
						"printf \"%%110s\n\" \"\" | tr ' ' '@'; "
						"echo \"One call was done at $(date +%%Hh%%Mmn%%Ss). Process function calls stack was:\"; "
						"echo ;"
						"printf \"%%-70s | %%s\n\" \"Function:\" \"Source file name and line number:\"; "
						"printf \"%%-70s | %%s\n\" \"Function:\" \"Source file name and line number:\" | tr '[:print:]' '-'; "
						"all_code_found=1; "
						"while read word1 word2; "
						"do "
							"if [ \"$word1\" = \"obj\" ] ; "
							"then "
								"current_obj=\"$word2\"; "
							"else "
								"func=\"$word1\"; read loc; "
								"if [ \"$loc\" = \"??:0\" ] ; "
								"then "
									"if [ \"$func\" = \"??\" ] ; "
									"then "
										"printf \"%%-70s | %%s\n\" \"(in $current_obj)\" '*'; "
									"else "
										"printf \"%%-70s | %%s\n\" \"$func\" '*'; "
									"fi; "
									"all_code_found=0; "
								"else "
									"printf \"%%-70s | %%s\n\" \"$func\" \"$loc\"; "
								"fi; "
								"if [ \"$func\" = \"main\" ] ; "
								"then "
									"break; "
								"fi; "
							"fi; "
						"done; "
						"if [ \"$all_code_found\" = \"0\" ]; "
						"then "
							"echo ;"
							"echo '*: code file and line number unavailable (source needs to be compiled with option -g)';"
						"fi; "
						"printf \"%%110s\n\" \"\" | tr ' ' '@'; "
						"interpreted_language=%d; "
						"if [ \"$interpreted_language\" = \"1\" ]; "
						"then "
							"printf \"" WARNING_INTERPRETED_LANGUAGE "\"; "
						"fi; "
						"echo; "
					  "} >> %s/%s", interpreted_language, directory, __PROCESS_STACKS_FILENAME);

	// execute the command
	run_command(command, "create the file called 'process_stacks'");
	free(command);

	free (strings);
}

