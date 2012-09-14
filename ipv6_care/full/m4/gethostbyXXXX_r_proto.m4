# Copyright (c) Centre National de la Recherche Scientifique (CNRS,
# France). 2010. 
# Copyright (c) Members of the EGEE Collaboration. 2008-2010. See 
# http://www.eu-egee.org/partners/ for details on the copyright
# holders.  
#
# Licensed under the Apache License, Version 2.0 (the "License"); 
# you may not use this file except in compliance with the License. 
# You may obtain a copy of the License at 
#
#    http://www.apache.org/licenses/LICENSE-2.0 
#
# Unless required by applicable law or agreed to in writing, software 
# distributed under the License is distributed on an "AS IS" BASIS, 
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
# See the License for the specific language governing permissions and 
# limitations under the License.

# AX_CHECK_GHBX_R_PROTOTYPE()
# -----------------------------------------------
# Check the prototype of gethostby<something>_r() functions.

AC_DEFUN([AX_CHECK_GHBX_R_PROTOTYPE],
[
	AC_CHECK_LIB(c, gethostbyname_r, [],
		AC_CHECK_LIB(nsl_r, gethostbyname_r, [],
			AC_CHECK_LIB(nsl, gethostbyname_r)))
	AC_CHECK_FUNC([gethostbyname_r], 
	[
		AC_CACHE_CHECK([what is the prototype of gethostby<something>_r() functions],
				[ax_cv_gethostbyXXXX_r_type],
		[
		AC_TRY_COMPILE(
			[
			#include <netdb.h>

			int gethostbyname_r(const char *name,
				       struct hostent *ret, char *buf, size_t buflen,
				       struct hostent **result, int *h_errnop)
			{
				return gethostbyname_r(name, ret, buf, buflen, result, h_errnop);
			}
			], [], [ax_cv_gethostbyXXXX_r_type=linux], [
				AC_TRY_COMPILE([
				#include <netdb.h>

				struct hostent *gethostbyname_r(const char *name,
					  struct hostent *ret, char *buf, int buflen,
					  int *h_errnop)
				{
					return gethostbyname_r(name, ret, buf, buflen, h_errnop);
				}
				], [], [ax_cv_gethostbyXXXX_r_type=solaris], [ax_cv_gethostbyXXXX_r_type=unknown]
				)
			])
		])
		if test x"$ax_cv_gethostbyXXXX_r_type" = xlinux; then
		  AC_DEFINE([HAVE_GETHBX_R_DOUBLE_POINTER_PARAM], 1,
			    [Define whether the prototype of gethostby<something>_r() functions includes a 'struct hostent **' parameter.])
		  AC_DEFINE([GETHOSTBYNAME_R_ARGS], [name, ret, buf, buflen, result, h_errnop],
			    [Record args of gethostbyname_r.])
		  AC_DEFINE([GETHOSTBYNAME_R_ARGS_WITH_TYPES], [const char *name, struct hostent *ret, char *buf, size_t buflen, struct hostent **result, int *h_errnop],
			    [Record args of gethostbyname_r with their types.])
		  AC_DEFINE([GETHOSTBYADDR_R_ARGS], [addr, len, type, ret, buf, buflen, result, h_errnop],
			    [Record args of gethostbyaddr_r.])
		  AC_DEFINE([GETHOSTBYADDR_R_ARGS_WITH_TYPES], [const void *addr, socklen_t len, int type, struct hostent *ret, char *buf, size_t buflen, struct hostent **result, int *h_errnop],
			    [Record args of gethostbyaddr_r with their types.])
		  AC_DEFINE([GETHOSTBYXXXX_R_RETURN_TYPE], int,
			    [Record the return type of gethostby<something>_r() functions.])
		  AC_DEFINE([GETHOSTBYXXXX_R_RETURN_VALUE_IF_NOT_FOUND], -1,
			    [Set the return value if dlsym fails when loading a gethostby<something>_r function symbol.])
		else
		if test x"$ax_cv_gethostbyXXXX_r_type" = xsolaris; then
		  AC_DEFINE([GETHOSTBYXXXX_HOSTENT_RESULT_IS_RETURNED], 1,
			    [Define whether the 'struct hostent *' result is returned as the return value of the function.])
		  AC_DEFINE([GETHOSTBYNAME_R_ARGS], [name, ret, buf, buflen, h_errnop],
			    [Record args of gethostbyname_r.])
		  AC_DEFINE([GETHOSTBYNAME_R_ARGS_WITH_TYPES], [const char *name, struct hostent *ret, char *buf, int buflen, int *h_errnop],
			    [Record args of gethostbyname_r with their types.])
		  AC_DEFINE([GETHOSTBYADDR_R_ARGS], [addr, len, type, ret, buf, buflen, h_errnop],
			    [Record args of gethostbyaddr_r.])
		  AC_DEFINE([GETHOSTBYADDR_R_ARGS_WITH_TYPES], [const char *addr, int len, int type, struct hostent *ret, char *buf, int buflen, int *h_errnop],
			    [Record args of gethostbyaddr_r with their types.])
		  AC_DEFINE([GETHOSTBYXXXX_R_RETURN_TYPE], [struct hostent *],
			    [Record the return type of gethostby<something>_r() functions.])
		  AC_DEFINE([GETHOSTBYXXXX_R_RETURN_VALUE_IF_NOT_FOUND], [NULL],
			    [Set the return value if dlsym fails when loading a gethostby<something>_r function symbol.])
		else
		  AC_MSG_ERROR([Prototypes of gethostby_<something>_r functions were not recognised.])
		fi
		fi
	], [])
])

