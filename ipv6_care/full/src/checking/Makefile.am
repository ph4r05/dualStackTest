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
include $(srcdir)/Makefile.src_info
include $(top_srcdir)/Makefile.flags
lib_LTLIBRARIES = libipv6_care_checking.la
libipv6_care_checking_la_LIBADD = ../common/libipv6_care_common.la
libipv6_care_checking_la_CPPFLAGS = $(AM_CPPFLAGS) -I$(top_srcdir)/src/common
