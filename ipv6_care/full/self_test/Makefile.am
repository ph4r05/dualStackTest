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

# we don't specify the SUBDIRS 'libs' and 'binaries' here because
# we don't want any 'install' step to be done.
# so we will manually specify what must be done for the other steps.

# the subdirectories should still be distributed
DIST_SUBDIRS= src

# we have a warning message saying that linking to a shared library is not portable. 
# In our case the handling of shared libraries is a prerequisite, so we can safely ignore 
# this warning. that is why we have the 'grep -v' commands below.
check-subdirs all-subdirs clean-subdirs: 
	@target=`echo $@ | sed s/-subdirs//`;      \
	cd src; $(MAKE) $$target | grep -v 'against the loadable module' | grep -v 'is not portable'; cd ..

check-local: check-subdirs
	@. src/libs/libpreload.la;								\
	if LD_PRELOAD=src/libs/.libs/$$dlname src/binaries/test_preload > /dev/null 2>&1; 	\
	then									  		\
		echo " -------------------------";						\
		echo "| LD_PRELOAD mechanism ok |";						\
		echo " -------------------------";						\
	else											\
		make ld_preload_error;    							\
	fi

all-local: all-subdirs

clean-local: clean-subdirs

ld_preload_error:
	$(error "FAILED: OS seems not to implement the LD_PRELOAD mechanism!!")

