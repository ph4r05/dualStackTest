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

# AX_CHECK_GETADDRINFO_CONSTANTS()
# -----------------------------------------------
# Checks existence of getaddrinfo related constants
AC_DEFUN([AX_CHECK_GETADDRINFO_CONSTANTS],
[
AC_CACHE_CHECK([whether EAI_ADDRFAMILY exists],
                [ax_cv_EAI_ADDRFAMILY],
[AC_TRY_COMPILE([ 
#define _GNU_SOURCE
#include <netdb.h>
], [ return EAI_ADDRFAMILY; ], [ax_cv_EAI_ADDRFAMILY=yes], [ax_cv_EAI_ADDRFAMILY=no])]
)
if test x"$ax_cv_EAI_ADDRFAMILY" = xyes; then
  AC_DEFINE([HAVE_EAI_ADDRFAMILY], 1,
            [Define whether EAI_ADDRFAMILY exists.])
fi

AC_CACHE_CHECK([whether EAI_NODATA exists],
                [ax_cv_EAI_NODATA],
[AC_TRY_COMPILE([
#define _GNU_SOURCE
#include <netdb.h>
], [ return EAI_NODATA; ], [ax_cv_EAI_NODATA=yes], [ax_cv_EAI_NODATA=no])]
)
if test x"$ax_cv_EAI_NODATA" = xyes; then
  AC_DEFINE([HAVE_EAI_NODATA], 1,
            [Define whether EAI_NODATA exists.])
fi

AC_CACHE_CHECK([whether EAI_NONAME exists],
                [ax_cv_EAI_NONAME],
[AC_TRY_COMPILE([
#define _GNU_SOURCE
#include <netdb.h>
], [ return EAI_NONAME; ], [ax_cv_EAI_NONAME=yes], [ax_cv_EAI_NONAME=no])]
)
if test x"$ax_cv_EAI_NONAME" = xyes; then
  AC_DEFINE([HAVE_EAI_NONAME], 1,
            [Define whether EAI_NONAME exists.])
fi

AC_CACHE_CHECK([whether EAI_AGAIN exists],
                [ax_cv_EAI_AGAIN],
[AC_TRY_COMPILE([
#define _GNU_SOURCE
#include <netdb.h>
], [ return EAI_AGAIN; ], [ax_cv_EAI_AGAIN=yes], [ax_cv_EAI_AGAIN=no])]
)
if test x"$ax_cv_EAI_AGAIN" = xyes; then
  AC_DEFINE([HAVE_EAI_AGAIN], 1,
            [Define whether EAI_AGAIN exists.])
fi

AC_CACHE_CHECK([whether EAI_MEMORY exists],
                [ax_cv_EAI_MEMORY],
[AC_TRY_COMPILE([
#define _GNU_SOURCE
#include <netdb.h>
], [ return EAI_MEMORY; ], [ax_cv_EAI_MEMORY=yes], [ax_cv_EAI_MEMORY=no])]
)
if test x"$ax_cv_EAI_MEMORY" = xyes; then
  AC_DEFINE([HAVE_EAI_MEMORY], 1,
            [Define whether EAI_MEMORY exists.])
fi

AC_CACHE_CHECK([whether EAI_SYSTEM exists],
                [ax_cv_EAI_SYSTEM],
[AC_TRY_COMPILE([
#define _GNU_SOURCE
#include <netdb.h>
], [ return EAI_SYSTEM; ], [ax_cv_EAI_SYSTEM=yes], [ax_cv_EAI_SYSTEM=no])]
)
if test x"$ax_cv_EAI_SYSTEM" = xyes; then
  AC_DEFINE([HAVE_EAI_SYSTEM], 1,
            [Define whether EAI_SYSTEM exists.])
fi

AC_CACHE_CHECK([whether EAI_FAIL exists],
                [ax_cv_EAI_FAIL],
[AC_TRY_COMPILE([
#define _GNU_SOURCE
#include <netdb.h>
], [ return EAI_FAIL; ], [ax_cv_EAI_FAIL=yes], [ax_cv_EAI_FAIL=no])]
)
if test x"$ax_cv_EAI_FAIL" = xyes; then
  AC_DEFINE([HAVE_EAI_FAIL], 1,
            [Define whether EAI_FAIL exists.])
fi

AC_CACHE_CHECK([whether ENODATA exists],
                [ax_cv_ENODATA],
[AC_TRY_COMPILE([
#include <errno.h>
], [ return ENODATA; ], [ax_cv_ENODATA=yes], [ax_cv_ENODATA=no])]
)
if test x"$ax_cv_ENODATA" = xyes; then
  AC_DEFINE([HAVE_ENODATA], 1,
            [Define whether ENODATA exists.])
fi

])

