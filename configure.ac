dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.

AC_INIT(touchscreen-autodisabler, 0.9.8)

AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.11])

AM_SILENT_RULES([yes])

AC_PROG_CC
AC_PATH_XTRA

dnl sleep - for delaying autostarting of 
AC_PATH_PROG([SLEEP], [sleep])
AC_SUBST([SLEEP])

dnl ***************************************************************************
dnl Internationalization
dnl ***************************************************************************
GETTEXT_PACKAGE=PACKAGE
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [GETTEXT Package name])
AM_GNU_GETTEXT([external])


dnl GSettings //////////////////////////////////////////////////////////////////

GLIB_GSETTINGS



dnl ronn ///////////////////////////////////////////////////////////////////////

AC_CHECK_TOOL([RONN], [ronn], [:])
AM_CONDITIONAL([HAS_RONN], [test s$RONN != s":" ])


PKG_CHECK_MODULES([TOUCHSCREEN_AUTODISABLER],
                  [x11 xi glib-2.0 gobject-2.0 gio-2.0])

AC_OUTPUT([
Makefile
po/Makefile.in
src/Makefile
man/Makefile
])
