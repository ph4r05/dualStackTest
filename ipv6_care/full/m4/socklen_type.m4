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

# AX_CHECK_SOCKLEN_TYPE()
# -----------------------------------------------
# Checks definitions about socklen related types
AC_DEFUN([AX_CHECK_SOCKLEN_TYPE],
[
CPPFLAGS_SOCKLEN_T=
AC_CACHE_CHECK([whether Psocklen_t is defined correctly or not defined],
                [ax_cv_PSOCKLEN_T_OK],
[AC_TRY_COMPILE([ 
#include <sys/types.h>
#include <sys/socket.h>
#ifdef _SOCKLEN_T
void a(Psocklen_t arg);

void a(socklen_t *arg)
{
}
#endif
], [ ], [ax_cv_PSOCKLEN_T_OK=yes], [ax_cv_PSOCKLEN_T_OK=no])]
)
if test x"$ax_cv_PSOCKLEN_T_OK" != xyes; then
	CPPFLAGS_SOCKLEN_T="-D_SOCKLEN_T -Dsocklen_t=size_t -DPsocklen_t=\"socklen_t *\""
	AC_SUBST([CPPFLAGS_SOCKLEN_T])
fi
])

