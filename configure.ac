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


dnl DBus ///////////////////////////////////////////////////////////////////////

AC_MSG_CHECKING([dbus session bus service file directory])
PKG_CHECK_VAR([DBUS_SESSION_BUS_SERVICES_DIR], [dbus-1], [session_bus_services_dir],
              [], [AC_MSG_ERROR([DBus session services directory is not available])])

AC_SUBST([dbussessionbusservicesdir], [$DBUS_SESSION_BUS_SERVICES_DIR])

AC_MSG_RESULT([$DBUS_SESSION_BUS_SERVICES_DIR])


dnl Systemd ////////////////////////////////////////////////////////////////////

AC_MSG_CHECKING([systemd user unit directory])
AC_ARG_WITH([systemd-user-unit],
            [AS_HELP_STRING([--with-systemd-user-unit],
                            [Install systemd user service unit])],
            [],
            [with_systemd_user_unit="auto"])



AS_CASE([$with_systemd_user_unit],
        ["no"],
        [],

        ["yes"],
        [PKG_CHECK_VAR([SYSTEMD_USER_UNIT_DIR], [systemd], [systemduserunitdir],
                       [],
                       [AC_MSG_ERROR([Systemd user unit is enabled but cannot find directory for it.])])],

        ["auto"],
        [PKG_CHECK_VAR([SYSTEMD_USER_UNIT_DIR], [systemd], [systemduserunitdir])],

        [AC_MSG_ERROR([words from 3 are allowed: yes, no, auto])])


AM_CONDITIONAL([HAVE_SYSTEMD_USER_UNIT], [test s$SYSTEMD_USER_UNIT_DIR != s""])
AS_IF([test s$SYSTEMD_USER_UNIT_DIR != s""],
      [AC_SUBST([systemduserunitdir], [$SYSTEMD_USER_UNIT_DIR])],
      [SYSTEMD_USER_UNIT_DIR="no"])

AC_MSG_RESULT([$SYSTEMD_USER_UNIT_DIR])

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
