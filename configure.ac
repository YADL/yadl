# Set CFLAGS
AC_PREREQ(2.57)

AC_INIT([yadl], [0.1], [josferna@redhat.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_GNU_SOURCE
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
AC_CONFIG_HEADERS(src/config.h)
AC_PROG_CC
CFLAGS="$CFLAGS -Wall -Werror"

# Check if CC is linked to CLANG
AC_MSG_CHECKING([if compiling with clang])
AC_COMPILE_IFELSE(
[AC_LANG_PROGRAM([], [[
#ifndef __clang__
       not clang
#endif
]])],
[CLANG=yes], [CLANG=no])
AC_MSG_RESULT([$CLANG])
AM_CONDITIONAL(CLANG, test "$CLANG" = yes)
AM_CONDITIONAL(GCC, test "$GCC" = yes)   # let the Makefile know if we're gcc

AC_PROG_LIBTOOL
AC_SUBST(LIBTOOL_DEPS)
# Check whether some low-level functions/files are available
AC_HEADER_STDC

# Check libraries
AC_CHECK_LIB([crypto], [MD5], , AC_MSG_ERROR([OpenSSL crypto library is required to build]))
PKG_CHECK_MODULES([UNITTEST], [cmockery2], , AC_MSG_ERROR([cmockery2 library is required to build]))

# If pkg-config
AC_CHECK_PROG(PKG_CONFIG, pkg-config, yes)
AM_CONDITIONAL([HAVE_PKG_CONFIG], [test "x$PKG_CONFIG" != "x"])
AS_IF([test "x$PKG_CONFIG" != "x"], [
    AC_CONFIG_FILES([yadl.pc])
])

# Check headers

AC_SUBST(CFLAGS)
AC_PREFIX_DEFAULT(/usr)

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
