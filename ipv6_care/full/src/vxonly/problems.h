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
#ifndef __PROBLEMS_H__
#define __PROBLEMS_H__

// general comments
#define UNDERLINE					"----------------------------------------------"
#define PROBLEM_DETECTED(problem)			"\n" "PROBLEM DETECTED:" "\n" UNDERLINE "\n" problem "\n" 
#define SOLUTION(solution)				"\n" "SOLUTION:" "\n" UNDERLINE "\n" solution "\n" "\n"

#define THIS_PROGRAM_USES_IPV4_ONLY(function)		"A call to " function "() which is IPv4-only was detected." 

#define THIS_PROGRAM_USES_GETADDRINFO(specified_family)	\
							"A call to getaddrinfo() with the address family set to " specified_family " was detected."

#define THIS_PROGRAM_USES_AF_DEPENDENT(function)	"A call to " function "() was detected. Although this function is IPv6-capable, " \
							"in most cases it makes your code address-family dependent, " \
							"because you have to specify the address-family as a parameter." 

#define USE_GETADDRINFO_AF_UNSPEC			"You should use getaddrinfo() with the address family set to AF_UNSPEC, " \
							"in order to be address-family agnostic."

#define USE_GETADDRINFO_AF_UNSPEC_AI_NUMERICHOST	"You should use getaddrinfo() with nodename=<your_ip_string>, no servname, " \
							"the address family set to AF_UNSPEC " \
							"and with the flag AI_NUMERICHOST, in order to be address-family agnostic."

#define USE_GETNAMEINFO_NI_NUMERICHOST			"You should use getnameinfo() with the flag NI_NUMERICHOST, " \
							"in order to be address-family agnostic."

#define USE_GETNAMEINFO					"You should use getnameinfo() in order to be address-family agnostic."

// accept on an IPv4 socket with no previous select / poll
#define ACCEPT_ONLY_IPV4_PROBLEM			"accept_only_IPv4"
#define ACCEPT_ONLY_IPV4_PROBLEM_DESCRIPTION		PROBLEM_DETECTED( "This program seems to accept only IPv4 clients." )	\
							SOLUTION( "Modify the source code in order to accept both IPv4 and IPv6 clients.\n" \
								"The first way to achieve this is to open two sockets, " \
								"one IPv4 socket and one IPv6 socket with its option IPV6_V6ONLY set to 1; " \
								"then you should wait for a connection event on any of them using 'select()'.\n" \
								"The other way is to open just one IPv6 socket, and set its IPV6_V6ONLY option to 0." )
// gethostbyname
#define GETHOSTBYNAME_PROBLEM				"gethostbyname"
#define GETHOSTBYNAME_PROBLEM_DESCRIPTION		PROBLEM_DETECTED( THIS_PROGRAM_USES_IPV4_ONLY("gethostbyname") )	\
							SOLUTION( USE_GETADDRINFO_AF_UNSPEC )

// gethostbyname_r
#define GETHOSTBYNAME_R_PROBLEM				"gethostbyname_r"
#define GETHOSTBYNAME_R_PROBLEM_DESCRIPTION		PROBLEM_DETECTED( THIS_PROGRAM_USES_IPV4_ONLY("gethostbyname_r") )	\
							SOLUTION( USE_GETADDRINFO_AF_UNSPEC )
#define PERL_GETHOSTBYNAME_R_PROBLEM			"gethostbyname_r"
#define PERL_GETHOSTBYNAME_R_PROBLEM_DESCRIPTION	PROBLEM_DETECTED( THIS_PROGRAM_USES_IPV4_ONLY("gethostbyname_r") "\n" \
								"Note: Perl uses gethostbyname_r() to process the gethostbyname() command; " "\n" \
                                                        	"this warning may be the consequence of a gethostbyname() command in the script. " \
								"gethostbyname() is IPv4-only also.") \
							SOLUTION( USE_GETADDRINFO_AF_UNSPEC )

// getaddrinfo with AF_INET
#define GETADDRINFO_AF_INET_PROBLEM			"getaddrinfo_AF_INET"
#define GETADDRINFO_AF_INET_PROBLEM_DESCRIPTION		PROBLEM_DETECTED( THIS_PROGRAM_USES_GETADDRINFO("AF_INET") ) 	 \
							SOLUTION( USE_GETADDRINFO_AF_UNSPEC )
#define PYTHON_GETADDRINFO_AF_INET_PROBLEM		"getaddrinfo_AF_INET"
#define PYTHON_GETADDRINFO_AF_INET_PROBLEM_DESCRIPTION	PROBLEM_DETECTED( THIS_PROGRAM_USES_GETADDRINFO("AF_INET") "\n" \
								"Note: Python uses getaddrinfo() to process the gethostbyname() command; " "\n" \
								"if you do not see any getaddrinfo() in your code, this warning may be the consequence of " \
								"a gethostbyname() command in the script. " \
								"gethostbyname() is IPv4-only.") \
							SOLUTION( USE_GETADDRINFO_AF_UNSPEC )

// getaddrinfo with AF_INET6
#define GETADDRINFO_AF_INET6_PROBLEM			"getaddrinfo_AF_INET6"
#define GETADDRINFO_AF_INET6_PROBLEM_DESCRIPTION	PROBLEM_DETECTED( THIS_PROGRAM_USES_GETADDRINFO("AF_INET6") ) 	\
							SOLUTION( USE_GETADDRINFO_AF_UNSPEC )

// gethostbyaddr
#define GETHOSTBYADDR_PROBLEM				"gethostbyaddr"
#define GETHOSTBYADDR_PROBLEM_DESCRIPTION		PROBLEM_DETECTED( THIS_PROGRAM_USES_AF_DEPENDENT("gethostbyaddr") )      \
							SOLUTION( USE_GETNAMEINFO )

// gethostbyaddr_r
#define GETHOSTBYADDR_R_PROBLEM				"gethostbyaddr_r"
#define GETHOSTBYADDR_R_PROBLEM_DESCRIPTION		PROBLEM_DETECTED( THIS_PROGRAM_USES_AF_DEPENDENT("gethostbyaddr_r") )      \
							SOLUTION( USE_GETNAMEINFO )

// inet_addr
#define INET_ADDR_PROBLEM				"inet_addr"
#define INET_ADDR_PROBLEM_DESCRIPTION			PROBLEM_DETECTED( THIS_PROGRAM_USES_IPV4_ONLY("inet_addr") )      \
							SOLUTION( USE_GETNAMEINFO_NI_NUMERICHOST )

// inet_ntoa
#define INET_NTOA_PROBLEM				"inet_ntoa"
#define INET_NTOA_PROBLEM_DESCRIPTION			PROBLEM_DETECTED( THIS_PROGRAM_USES_IPV4_ONLY("inet_ntoa") )      \
							SOLUTION( USE_GETNAMEINFO_NI_NUMERICHOST )

// inet_aton
#define INET_ATON_PROBLEM				"inet_aton"
#define INET_ATON_PROBLEM_DESCRIPTION			PROBLEM_DETECTED( THIS_PROGRAM_USES_IPV4_ONLY("inet_aton") )      \
							SOLUTION( USE_GETADDRINFO_AF_UNSPEC_AI_NUMERICHOST )

// inet_ntop
#define INET_NTOP_PROBLEM				"inet_ntop"
#define INET_NTOP_PROBLEM_DESCRIPTION			PROBLEM_DETECTED( THIS_PROGRAM_USES_AF_DEPENDENT("inet_ntop") )      \
							SOLUTION( USE_GETNAMEINFO_NI_NUMERICHOST )

// inet_pton
#define INET_PTON_PROBLEM				"inet_pton"
#define INET_PTON_PROBLEM_DESCRIPTION			PROBLEM_DETECTED( THIS_PROGRAM_USES_AF_DEPENDENT("inet_pton") )      \
							SOLUTION( USE_GETADDRINFO_AF_UNSPEC_AI_NUMERICHOST )

#endif
