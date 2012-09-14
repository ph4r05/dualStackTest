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

# AX_CHECK_SYS_GETTID()
# -----------------------------------------------
# Defines HAVE_SYS_GETTID if syscall SYS_gettid exists
AC_DEFUN([AX_CHECK_SYS_GETTID],
[AC_CACHE_CHECK([whether syscall SYS_gettid exists],
                [ax_cv_sys_gettid_exists],
[AC_TRY_COMPILE([
#include <sys/syscall.h>
#include <unistd.h>
], [syscall(SYS_gettid);], [ax_cv_sys_gettid_exists=yes], [ax_cv_sys_gettid_exists=no])]
)
if test x"$ax_cv_sys_gettid_exists" = xyes; then
  AC_DEFINE([HAVE_SYS_GETTID], 1,
            [Define if syscall SYS_gettid exists.])
fi])

