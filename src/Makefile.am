BUILT_SOURCES= \
        tad_enumtypes.c \
        tad_enumtypes.h \
        wsid.Tad.gresources.c


bin_PROGRAMS = touchscreen-autodisabler

gsettings_SCHEMAS = wsid.Tad.gschema.xml




touchscreen_autodisabler_SOURCES = \
	main.c \
	tad_app.c \
	tad_util.c \
        tad_statemachine.c \
	tad_device.c \
        tad_enumtypes.c \
	wsid.Tad.gresources.c

touchscreen_autodisabler_CFLAGS= \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir). \
	$(XI_CFLAGS). \
	$(XI_CFLAGS). \
	$(XI_CFLAGS)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	$(TOUCHSCREEN_AUTODISABLER_CFLAGS)

touchscreen_autodisabler_LDADD = \
        $(TOUCHSCREEN_AUTODISABLER_LIBS) \
	-lX11 -lXext \
	$(X_LIBS) \
	$(XI_LIBS) \
	@LIBINTL@



noinst_HEADERS = \
	tad_def.h \
	tad_device.h \
	tad_util.h \
	tad_statemachine.h \
	tad_app.h \
	tad_enumtypes.h



EXTRA_DIST = \
        $(gsettings_SCHEMAS) \
        tad_enumtypes.c.template \
        tad_enumtypes.h.template \
        wsid.Tad.dbus.xml \
        wsid.Tad.gresources.xml


CLEANFILES= \
	tad_enumtypes.c \
	tad_enumtypes.h \
	wsid.Tad.gresources.c

@GSETTINGS_RULES@




tad_enumtypes.c: tad_enumtypes.c.template tad_def.h
	$(AM_V_GEN) $(GLIB_MKENUMS) --template tad_enumtypes.c.template tad_def.h > tad_enumtypes.c.tmp \
	&& mv tad_enumtypes.c.tmp tad_enumtypes.c

tad_enumtypes.h: tad_enumtypes.h.template tad_def.h
	$(AM_V_GEN) $(GLIB_MKENUMS) --template tad_enumtypes.h.template tad_def.h > tad_enumtypes.h.tmp \
	&& mv tad_enumtypes.h.tmp tad_enumtypes.h

wsid.Tad.gresources.c: wsid.Tad.gresources.xml wsid.Tad.dbus.xml
	$(AM_V_GEN) $(GLIB_COMPILE_RESOURCES) --generate-source wsid.Tad.gresources.xml
