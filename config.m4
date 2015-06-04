dnl $Id$
dnl config.m4 for extension xlsxwriter

PHP_ARG_WITH(xlsxwriter, for xlsxwriter support,
Make sure that the comment is aligned:
[  --with-xlsxwriter   enable xlsxwriter support])

PHP_ARG_WITH(libxlsxwriter, C include dir for libxl,
[  --with-libxlsxwriter[=DIR] Include path for the C headers and libraries of libxlsxwriter])

if test "$PHP_XLSXWRITER" != "no"; then
  AC_MSG_CHECKING([for libxlsxwriter includes])
  if test -r $PHP_LIBXLSXWRITER/include/xlsxwriter.h; then
    XLSX_WRITER_INCLUDE_DIR=$PHP_LIBXLSXWRITER/include
  fi

  if test -z "$XLSX_WRITER_INCLUDE_DIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Please reinstall the libxlsxwriter distribution])
  else
    AC_MSG_RESULT(found in $XLSX_WRITER_INCLUDE_DIR)
  fi

  SEARCH_FOR="libxlsxwriter.so"

  AC_MSG_CHECKING([for libxlsxwriter libraries])
  if test -r $PHP_LIBXLSXWRITER/lib/$SEARCH_FOR; then
    XLSX_WRITER_LIB_DIR=$PHP_LIBXLSXWRITER/lib
  fi
  if test -r $PHP_LIBXLSXWRITER/lib64/$SEARCH_FOR; then
    XLSX_WRITER_LIB_DIR=$PHP_LIBXLSXWRITER/lib64
  fi

  if test -z "XLSX_WRITER_LIB_DIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Please reinstall the libxlsxwriter distribution])
  else
    AC_MSG_RESULT(found in $XLSX_WRITER_LIB_DIR)
  fi

  PHP_CHECK_LIBRARY(xlsxwriter, worksheet_write_number,
  [
    PHP_ADD_INCLUDE($XLSX_WRITER_INCLUDE_DIR)
    PHP_ADD_LIBRARY_WITH_PATH(xlsxwriter, $XLSX_WRITER_LIB_DIR, XLSXWRITER_SHARED_LIBADD)
    AC_DEFINE(HAVE_XLSXWRITERLIB, 1, [ ])
  ], [
    AC_MSG_ERROR([symbolink not found])
  ], [
    -L$XLSX_WRITER_LIB_DIR
  ])

  PHP_SUBST(XLSXWRITER_SHARED_LIBADD)
  PHP_NEW_EXTENSION(xlsxwriter, xlsxwriter.c, $ext_shared)
fi



