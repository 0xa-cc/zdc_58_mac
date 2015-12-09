dnl
dnl $Id: config.m4,v 1.24.4.4 2005/01/22 01:37:12 sniper Exp $
dnl 

PHP_ARG_WITH(mcrypt_filter, for mcrypt support,
[  --with-mcrypt-filter[=DIR] Include mcrypt_filter support.])

if test "$PHP_MCRYPT_FILTER" != "no"; then
  for i in $PHP_MCRYPT_FILTER /usr/local /usr; do
    test -f $i/include/mcrypt.h && MCRYPT_FILTER_DIR=$i && break
  done

  if test -z "$MCRYPT_FILTER_DIR"; then
    AC_MSG_ERROR(mcrypt.h not found. Please reinstall libmcrypt.)
  fi

  PHP_CHECK_LIBRARY(mcrypt, mcrypt_module_open, 
  [
    PHP_ADD_LIBRARY(ltdl,, MCRYPT_SHARED_LIBADD)
    AC_DEFINE(HAVE_LIBMCRYPT24,1,[ ])

    PHP_CHECK_LIBRARY(mcrypt, mcrypt_generic_deinit, 
    [
      AC_DEFINE(HAVE_MCRYPT_GENERIC_DEINIT,1,[ ])
    ],[],[
      -L$MCRYPT_FILTER_DIR/lib -lltdl
    ])

  ],[
    PHP_CHECK_LIBRARY(mcrypt, mcrypt_module_open,
    [
      AC_DEFINE(HAVE_LIBMCRYPT24,1,[ ])

      PHP_CHECK_LIBRARY(mcrypt, mcrypt_generic_deinit,
      [
        AC_DEFINE(HAVE_MCRYPT_GENERIC_DEINIT,1,[ ])
      ],[],[
        -L$MCRYPT_FILTER_DIR/lib
      ])
    ],[
      AC_MSG_ERROR([Sorry, I was not able to diagnose which libmcrypt version you have installed.])
    ],[
      -L$MCRYPT_FILTER_DIR/lib
    ])
  ],[
    -L$MCRYPT_FILTER_DIR/lib -lltdl
  ])

  PHP_ADD_LIBRARY_WITH_PATH(mcrypt, $MCRYPT_FILTER_DIR/lib, MCRYPT_SHARED_LIBADD)
  PHP_ADD_INCLUDE($MCRYPT_FILTER_DIR/include)

  PHP_NEW_EXTENSION(mcrypt_filter, mcrypt_filter_stub.c mcrypt_filter.c, $ext_shared)
  PHP_SUBST(MCRYPT_FILTER_SHARED_LIBADD)
  AC_DEFINE(HAVE_LIBMCRYPT,1,[ ])
fi
