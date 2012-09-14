#!/usr/bin/env bash
#
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
#     http://www.apache.org/licenses/LICENSE-2.0 
# 
# Unless required by applicable law or agreed to in writing, software 
# distributed under the License is distributed on an "AS IS" BASIS, 
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
# See the License for the specific language governing permissions and 
# limitations under the License.
# 
# ---------------------------------------------------------------------
# Developed by Etienne DUBLE - CNRS LIG (http://www.liglab.fr)
# etienne __dot__ duble __at__ imag __dot__ fr
# ---------------------------------------------------------------------

file=$1
library_path=$2
if [ "$library_path" = "" ]
then
	echo "Usage: IPv6_CARE_binary_lookup.sh <path_of_binary_file> <path_of_ld_preloadable_library>" >&2
	exit
fi

echo -n "Testing... "
# freebsd needs ldd option '-a' in order to have the LD_PRELOADed library displayed.
# linux does not know this option.
# opensolaris' ldd fails when LD_PRELOAD is set, we use LD_PRELOAD_32 instead which works.
# So we try all these and ignore errors.
ldd_result=$(
	LD_PRELOAD=$library_path ldd $file 2>/dev/null
	LD_PRELOAD=$library_path ldd -a $file 2>/dev/null
	LD_PRELOAD_32=$library_path ldd $file 2>/dev/null
)
echo "done."
if [ $(echo "$ldd_result" | grep '=>' | wc -l) -eq 0 ]
then
	file $file
	echo "$file is not a dynamically linked binary executable, IPv6 CARE will not be loaded." 
	exit
fi

set -- $(ls -l $file)
rights=$1
owner=$3
owner_group=$4
if [ $(echo "$rights" | cut -c 4) = "s" -a "$owner" != "$(whoami)" ]
then
	echo "Test failed. This binary file has its setuid bit set and its owner is $owner."
	echo "Please login as $owner in order to use IPv6 CARE with this binary file."
else
	if [ $(echo "$rights" | cut -c 7) = "s" -a "$owner_group" != "$(id -gn)" ]
	then
		echo "Test failed. This binary file has its setgid bit set and its owner's group is $owner_group."
		echo "Please login as $owner in order to use IPv6 CARE with this binary file."
	else
		if [ $(echo "$ldd_result" | grep "$library_path" | wc -l) -eq 1 ]
		then
			echo "Test succeeded: IPv6 CARE should work with this binary file."
		else
			echo "Test failed for an unknown reason. IPv6 CARE will not be loaded."
			echo "Please check that a securing environment like SELinux or AppArmor is not blocking the LD_PRELOAD based mechanism on this binary file."
		fi
	fi
fi

