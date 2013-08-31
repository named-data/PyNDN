#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([PyNDN], [0.2], [takeda@takeda.tk])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([build-aux])
AC_CONFIG_SRCDIR([csrc/pyndn.c])

AM_INIT_AUTOMAKE([foreign 1.11 silent-rules parallel-tests color-tests -Wall -Werror])
AM_SILENT_RULES([yes])

# Checks for programs.
AC_PROG_CC

# Required by automake 1.12, not recognized by automake 1.11
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])

LT_INIT

# Checks for libraries.
AX_CHECK_NDN([], AC_MSG_ERROR([NDNx is required to compile PyNDN]))

AM_PATH_PYTHON([2.7], [
	dnl Make sure files will be installed in right location (yes, even on Mac)
	if test "x${prefix}" = "xNONE"; then
		pythondir=`$PYTHON -c "import sys; import distutils.sysconfig as sc; \
			path = sc.get_python_lib(0, 0, '${ac_default_prefix}'); \
			print(path if path in sys.path else sc.get_python_lib(0, 0))"`
		pyexecdir=`$PYTHON -c "import sys; import distutils.sysconfig as sc; \
			path = sc.get_python_lib(1, 0, '${ac_default_prefix}'); \
			print(path if path in sys.path else sc.get_python_lib(1, 0))"`
	else
		pythondir=`$PYTHON -c "import distutils.sysconfig as sc; \
			print(sc.get_python_lib(0, 0, '${prefix}'))"`
		pyexecdir=`$PYTHON -c "import distutils.sysconfig as sc; \
			print(sc.get_python_lib(1, 0, '${prefix}'))"`
	fi
])
AC_PATH_PROGS([PYTHON_CONFIG],
	[python$PYTHON_VERSION-config python-config],
	[no],
	[`dirname $PYTHON`])
AS_IF([test "$PYTHON_CONFIG" = no],
	[AC_MSG_ERROR([cannot find python-config for $PYTHON.])])

# Checks for header files.
AC_MSG_CHECKING([python include flags])
PYTHON_INCLUDES=`$PYTHON_CONFIG --includes`
AC_MSG_RESULT([$PYTHON_INCLUDES])
AC_SUBST(PYTHON_INCLUDES)

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
	Makefile
	csrc/Makefile
	csrc/namecrypto/Makefile
	python/Makefile
	tests/Makefile
	])
AC_CONFIG_FILES([tests/run], [chmod +x tests/run])
AC_OUTPUT
