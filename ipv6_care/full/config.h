/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Record args of gethostbyaddr_r. */
#define GETHOSTBYADDR_R_ARGS addr, len, type, ret, buf, buflen, result, h_errnop

/* Record args of gethostbyaddr_r with their types. */
#define GETHOSTBYADDR_R_ARGS_WITH_TYPES const void *addr, socklen_t len, int type, struct hostent *ret, char *buf, size_t buflen, struct hostent **result, int *h_errnop

/* Record args of gethostbyname_r. */
#define GETHOSTBYNAME_R_ARGS name, ret, buf, buflen, result, h_errnop

/* Record args of gethostbyname_r with their types. */
#define GETHOSTBYNAME_R_ARGS_WITH_TYPES const char *name, struct hostent *ret, char *buf, size_t buflen, struct hostent **result, int *h_errnop

/* Define whether the 'struct hostent *' result is returned as the return
   value of the function. */
/* #undef GETHOSTBYXXXX_HOSTENT_RESULT_IS_RETURNED */

/* Record the return type of gethostby<something>_r() functions. */
#define GETHOSTBYXXXX_R_RETURN_TYPE int

/* Set the return value if dlsym fails when loading a gethostby<something>_r
   function symbol. */
#define GETHOSTBYXXXX_R_RETURN_VALUE_IF_NOT_FOUND -1

/* Record the prototype of getnameinfo(). */
/* #undef GETNAMEINFO_PROTOTYPE */

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the `dlvsym' function. */
#define HAVE_DLVSYM 1

/* Define whether EAI_ADDRFAMILY exists. */
#define HAVE_EAI_ADDRFAMILY 1

/* Define whether EAI_AGAIN exists. */
#define HAVE_EAI_AGAIN 1

/* Define whether EAI_FAIL exists. */
#define HAVE_EAI_FAIL 1

/* Define whether EAI_MEMORY exists. */
#define HAVE_EAI_MEMORY 1

/* Define whether EAI_NODATA exists. */
#define HAVE_EAI_NODATA 1

/* Define whether EAI_NONAME exists. */
#define HAVE_EAI_NONAME 1

/* Define whether EAI_SYSTEM exists. */
#define HAVE_EAI_SYSTEM 1

/* Define whether ENODATA exists. */
#define HAVE_ENODATA 1

/* Define to 1 if you have the <execinfo.h> header file. */
#define HAVE_EXECINFO_H 1

/* Define whether the prototype of gethostby<something>_r() functions includes
   a 'struct hostent **' parameter. */
#define HAVE_GETHBX_R_DOUBLE_POINTER_PARAM 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the `c' library (-lc). */
#define HAVE_LIBC 1

/* Define to 1 if you have the `dl' library (-ldl). */
#define HAVE_LIBDL 1

/* Define to 1 if you have the `execinfo' library (-lexecinfo). */
/* #undef HAVE_LIBEXECINFO */

/* Define to 1 if you have the `nsl' library (-lnsl). */
/* #undef HAVE_LIBNSL */

/* Define to 1 if you have the `nsl_r' library (-lnsl_r). */
/* #undef HAVE_LIBNSL_R */

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define whether SO_BINDTODEVICE exists. */
#define HAVE_SO_BINDTODEVICE 1

/* Define whether SO_PRIORITY exists. */
#define HAVE_SO_PRIORITY 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define if syscall SYS_gettid exists. */
#define HAVE_SYS_GETTID 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 or 0, depending whether the compiler supports simple visibility
   declarations. */
#define HAVE_VISIBILITY 1

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* Name of package */
#define PACKAGE "ipv6_care"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "etienne.duble@imag.fr"

/* Define to the full name of this package. */
#define PACKAGE_NAME "ipv6_care"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "ipv6_care 3.2c"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "ipv6_care"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "3.2c"

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Version number of package */
#define VERSION "3.2c"

#define GETNAMEINFO_PROTOTYPE getnameinfo (__const struct sockaddr *__restrict sa,   \
socklen_t salen, char *__restrict node, \
socklen_t nodelen, char *__restrict service, \
socklen_t servicelen, int flags)

