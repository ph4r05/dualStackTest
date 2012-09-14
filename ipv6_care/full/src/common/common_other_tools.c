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
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>

int _ipv6_care_close(int fd)
{
	return syscall(SYS_close, fd);
}

// This function tries to locate the original networking function of the libc
void *get_symbol(int num_args, char *symbol, ... /* optional symbol versions */)
{
	void *p_symbol = NULL;
	char *dlerr, *symbol_version;
	va_list ap;
	int i;

	// avoid calling dlsym() when trying to find 'close()':
	// firefox (and maybe others) uses a redefinition of malloc, calloc (called jemalloc)
	// and it needs an initialisation of these routines. This initialisation
	// calls close(). If we call dlsym() we will have a loop (or mutex hanging)
	// because dlsym() calls malloc().
	if (strcmp(symbol, "close") == 0)
	{	// syscall directly
		return _ipv6_care_close;
	}

#if HAVE_DLVSYM
	// try the given symbol versions provided first, if any
	va_start(ap, symbol);
	for(i=1; i<num_args; i++)
	{
		symbol_version = va_arg(ap, char *);
		p_symbol = dlvsym(RTLD_NEXT,symbol,symbol_version);
		if (p_symbol != NULL)
		{
			break;
		}
	}
	va_end(ap);
#endif
	
	// try dlsym with no version specified
	if (p_symbol == NULL)
	{
		p_symbol = dlsym(RTLD_NEXT,symbol);
	}
	
        if (p_symbol == NULL)
        {
		dlerr= (char*)dlerror(); 
		if (!dlerr)
		{
			dlerr= "Unknown reason";
		}

                fprintf(stderr, "failed to find original function: %s!\n", dlerr);
		errno = ENOSYS;
        }
	return p_symbol;
}

#if HAVE_SYS_GETTID

int get_thread_id()
{
	int thread_id;
	thread_id = syscall(SYS_gettid);
	// if this is the main thread, return 0
	if (thread_id == getpid())
	{
		thread_id = 0;
	}
	return thread_id;
}

#else

int __dummy_thread_id_counter = -1;
__thread int __dummy_thread_id = -1;

// if no SYS_gettid syscall is provided, 
// increment an integer as the thread_id.
// the first walue will be 0, and as an approximation
// we consider that the initial (main) thread will be
// the first one to go through this code, and will get this
// first value (i.e. 0).
int get_thread_id()
{
	if (__dummy_thread_id == -1)
	{	// for simplication the following is considered atomic
		__dummy_thread_id = ++__dummy_thread_id_counter;
	}
	return __dummy_thread_id;
}

#endif
