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
#ifndef __COMMON_MACROS_H__
#define __COMMON_MACROS_H__

#include "common_other_tools.h"

extern __thread int function_depth;

#if HAVE_VISIBILITY
#define PUBLIC_FUNCTION __attribute__((__visibility__("default")))
#else
#define PUBLIC_FUNCTION
#endif

/* The PP_NARG macro returns the number of arguments that have been
 * passed to it. 
 * (Idea from Laurent Deniau 
 * http://groups.google.com/group/comp.std.c/browse_frm/thread/77ee8c8f92e4a3fb)
 */

#define PP_NARG(...) \
	PP_NARG_(__VA_ARGS__,PP_RSEQ_N())
#define PP_NARG_(...) \
	PP_ARG_N(__VA_ARGS__)
#define PP_ARG_N( \
	_1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
	_11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
	_21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
	_31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
	_41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
	_51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
	_61,_62,_63,N,...) N
#define PP_RSEQ_N() \
	63,62,61,60,                   \
	59,58,57,56,55,54,53,52,51,50, \
	49,48,47,46,45,44,43,42,41,40, \
	39,38,37,36,35,34,33,32,31,30, \
	29,28,27,26,25,24,23,22,21,20, \
	19,18,17,16,15,14,13,12,11,10, \
	9,8,7,6,5,4,3,2,1,0 

#define FUNCTION_SYMBOL(x)		x
#define ARGS(...)			(__VA_ARGS__)
#define RETURN_VALUE_IF_FAILURE(x)	x
#define TRY_SYMBOL_VERSIONS(...)	__VA_ARGS__

#define GET_SYMBOL_COUNT_ARGS(...)	get_symbol(PP_NARG(__VA_ARGS__), __VA_ARGS__) 
#define RESULT(x)	x

// in some cases two levels of macros are needed 
// (http://gcc.gnu.org/onlinedocs/cpp/Stringification.html)
#define _stringified(x) #x
#define stringified(x) _stringified(x)

// the construction with 'static' below is an optimisation which avoids
// to look several times for the same symbol.

#define __DEFINE_ORIGINAL_SYMBOL(__symbol_variable, __func_name, ...)					\
													\
	static typeof(__func_name) *__symbol_variable = NULL;						\
													\
	if (__symbol_variable == NULL)									\
	{												\
		__symbol_variable = (typeof(__func_name)*)						\
			GET_SYMBOL_COUNT_ARGS((char *) stringified(__func_name), ##__VA_ARGS__);	\
	}

/*
 * Please first look at the file common_original_functions.c to understand 
 * how __CALL_ORIGINAL_FUNCTION is used. 
 * 
 * __CALL_ORIGINAL_FUNCTION calls the original function (of the standard libc).
 */
#define __CALL_ORIGINAL_FUNCTION(__func_name, 					\
		__func_args, __func_return_value_if_error, __result, ...)	\
										\
	__DEFINE_ORIGINAL_SYMBOL(p_original_func, __func_name, ##__VA_ARGS__)	\
	if (p_original_func != NULL) 						\
	{									\
		function_depth += 1;						\
		__result = p_original_func __func_args;				\
		function_depth -= 1;						\
	}									\
	else 									\
	{									\
		__result = __func_return_value_if_error;			\
	}

#define __CALL_ORIGINAL_FUNCTION_RETURNING_VOID(__func_name, 			\
		__func_args, ...)						\
										\
	__DEFINE_ORIGINAL_SYMBOL(p_original_func, __func_name, ##__VA_ARGS__)	\
	if (p_original_func != NULL) 						\
	{									\
		function_depth += 1;						\
		p_original_func __func_args;					\
		function_depth -= 1;						\
	}									\

#endif

