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
#include <dirent.h>
#include <errno.h>
#include <time.h>
#include <libgen.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/syscall.h>
extern int errno;

#include "append_to_string.h"
#include "common_fd_tools.h"
#include "common_other_tools.h"
#include "common_colors.h"
#include "log.h"
#include "system_commands.h"

#define __BASE_DIAGNOSIS_DIRECTORY	"/tmp/ipv6_diagnosis"
#define __FULL_COMMAND_LINE_FILE	"full_command_line"
#define __MKDIR_PERMISSIONS		(S_IRWXU | S_IRWXG | S_IRWXO)
#define __FOPEN_PERMISSIONS		"w"

extern char *program_basename;
extern char *program_command_line;

int mkdir_777(char *dir)
{
	mode_t old_umask;
	int result;

	old_umask = umask(0);
	result = mkdir(dir, __MKDIR_PERMISSIONS);
	umask(old_umask);
	return result;
}

FILE *fopen_666(char *file)
{
	mode_t old_umask;
	FILE *result;

	old_umask = umask(0);
	result = fopen(file, __FOPEN_PERMISSIONS);
	umask(old_umask);
	return result;
}

// This function creates a directory path
int recursive_mkdir(char *dir)
{
	int result = -1;
	char *basedir_alloc, *basedir;

	basedir_alloc = (char*) malloc((strlen(dir) + 1)*sizeof(char));

	// if recursion arrived at "/" it is time to stop!
	if (strcmp(dir, "/") != 0)
	{
		// first try
		result = mkdir_777(dir);

		// if dir already exist we do not consider it is an error
		if ((result != 0)&&(errno == EEXIST))
		{
			result = 0;
		}

		if ((result != 0)&&(errno != ENOENT))
		{
			colored_print_if_tty(RED, "** IPv6 CARE failed to create %s (%s).", dir, strerror(errno));
			PRINTF("\n"); 
			fflush(tty_fd);
		}

		// if first try failed with error "no such file or directory"
		if ((result != 0)&&(errno == ENOENT))
		{
			// we calculate the base dir
			// (we first make a copy because dirname modifies the string)
			strcpy(basedir_alloc, dir);
			basedir = dirname((char *)basedir_alloc);
			// and call the function on this base dir
			result = recursive_mkdir(basedir);

			if (result == 0)
			{	// if result was ok on the base dir,
				// we should now be able to create the initial directory
				result = mkdir_777(dir);
			}
		}
	}

	free(basedir_alloc);	

	return result;
}

// This function creates the directory path related to the running program
// and returns a string representing this path.
char *get_or_create_the_directory_related_to_the_program()
{
	static int directory_created = 0;
	static char *directory_path_alloc = NULL;

	char *symlink_dir_path_alloc, *symlink_path_alloc, 
			*full_command_line_path_alloc;
	time_t now;
	struct tm time_details;
	static char* month_name[12] = {
                      "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                      "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
                  };
	FILE *file;
     
     	if (directory_created == 0)
	{
		// build the path of the directory
		asprintf(&directory_path_alloc, "%s/%s/by_pid/pid_%d", __BASE_DIAGNOSIS_DIRECTORY,
				program_basename, getpid());

		// create the directory if the directory do not already exists
		if (recursive_mkdir(directory_path_alloc) != 0)
		{	// error already reported in recursive_mkdir()
			free(directory_path_alloc);
			return NULL;
		}
		else
		{	
			// we create a symbolic link with the date in order to find easily 
			// which pid is the one we want to look at
			time(&now);
			localtime_r(&now, &time_details);

			asprintf(&symlink_dir_path_alloc, "%s/%s/by_time/%s_%d_%d_%02dh%02d",
					__BASE_DIAGNOSIS_DIRECTORY, program_basename, 
					month_name[time_details.tm_mon], time_details.tm_mday, 
					1900 + time_details.tm_year, time_details.tm_hour, 
					time_details.tm_min);

			// create the directory
			recursive_mkdir(symlink_dir_path_alloc);

			asprintf(&symlink_path_alloc, "%s/pid_%d",
					symlink_dir_path_alloc, getpid());
			symlink(directory_path_alloc, symlink_path_alloc);
			free(symlink_path_alloc);
			free(symlink_dir_path_alloc);

			// we also create the full_command_line file
			asprintf(&full_command_line_path_alloc, "%s/%s",
				directory_path_alloc, __FULL_COMMAND_LINE_FILE);
			file = fopen(full_command_line_path_alloc, "w");
			fprintf(file, "%s\n", program_command_line);
			fclose(file);
			free(full_command_line_path_alloc);

			directory_created = 1;
		}
	}

	return directory_path_alloc;
}

// This function creates the directory path related to the running thread
// and returns a string representing this path.
char *get_or_create_the_directory_related_to_the_thread()
{
	static __thread int directory_created = 0;
	static __thread char *directory_path_alloc_thread = NULL;

	char *directory_path;
	int thread_id;

	if (directory_created == 0)
	{
		directory_path = get_or_create_the_directory_related_to_the_program();
		if (directory_path != NULL)
		{	
			thread_id = get_thread_id();

			// if this is the main thread started at execution of the program, put diagnosis in the main directory
			if (thread_id == 0)
			{
				//directory_path_alloc_thread = directory_path;
				asprintf(&directory_path_alloc_thread, "%s", directory_path); 
			}
			else
			{	// this thread was created later on, put diagnosis in a dedicate directory
				asprintf(&directory_path_alloc_thread, "%s/thread_%d", directory_path, thread_id); 
				recursive_mkdir(directory_path_alloc_thread);
			}

			directory_created = 1;
		}
	}

	return directory_path_alloc_thread;
}

// this function opens the log file and returns its file descriptor
FILE *open_log_file()
{
	FILE *file = NULL;
	char *full_filename_alloc;
	char *directory_path;

	directory_path = get_or_create_the_directory_related_to_the_thread();
	if (directory_path != NULL)
	{	
		asprintf(&full_filename_alloc, "%s/log", directory_path); 

		// open the file
		file = fopen_666(full_filename_alloc);
		free(full_filename_alloc);
		if (file == NULL)
		{
			colored_print_if_tty(RED, "** IPv6 CARE failed to create or open %s (%s).", full_filename_alloc, strerror(errno));
			PRINTF("\n"); 
			fflush(tty_fd);
		}
	}

	return file;
}

