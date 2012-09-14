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
#include <sys/select.h>
#include <stdio.h>

// global variables
int max_function_depth_reported = 0;
int interpreted_language = 0;
int verbose_level = 0;
char *program_command_line = NULL;
char *program_basename = NULL;
char *ld_preload_value = NULL;
char *interpreter_name = NULL;

// per-thread variables
__thread char *log_file_content = NULL;
__thread char *log_function_line = NULL;
__thread int index_last_line = -1;
__thread int log_file_allocated_size = 0;
__thread fd_set last_read_fds_storage;
__thread fd_set *last_read_fds = NULL;
__thread unsigned int last_read_nfds = 0;


