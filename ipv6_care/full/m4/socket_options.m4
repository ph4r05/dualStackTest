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

# AX_CHECK_SOCKET_OPTIONS()
# -----------------------------------------------
# Checks existence of some specific socket options
AC_DEFUN([AX_CHECK_SOCKET_OPTIONS],
[
AC_CACHE_CHECK([whether SO_BINDTODEVICE exists],
                [ax_cv_SO_BINDTODEVICE],
[AC_TRY_COMPILE([ 
#include <sys/socket.h>
], [ return SO_BINDTODEVICE; ], [ax_cv_SO_BINDTODEVICE=yes], [ax_cv_SO_BINDTODEVICE=no])]
)
if test x"$ax_cv_SO_BINDTODEVICE" = xyes; then
  AC_DEFINE([HAVE_SO_BINDTODEVICE], 1,
            [Define whether SO_BINDTODEVICE exists.])
fi

AC_CACHE_CHECK([whether SO_PRIORITY exists],
                [ax_cv_SO_PRIORITY],
[AC_TRY_COMPILE([ 
#include <sys/socket.h>
], [ return SO_PRIORITY; ], [ax_cv_SO_PRIORITY=yes], [ax_cv_SO_PRIORITY=no])]
)
if test x"$ax_cv_SO_PRIORITY" = xyes; then
  AC_DEFINE([HAVE_SO_PRIORITY], 1,
            [Define whether SO_PRIORITY exists.])
fi

])

