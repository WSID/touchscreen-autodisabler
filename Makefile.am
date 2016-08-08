## Process this file with automake to produce Makefile.in

SUBDIRS = po src man



# Document files
################

dist_doc_DATA = \
	README \
	ABOUT-NLS \
	COPYING \
	AUTHORS \
	ChangeLog \
	INSTALL \
	NEWS


# Service files
###############

dbussessionbusservices_DATA= wsid.Tad.service

if HAVE_SYSTEMD_USER_UNIT
systemduserunit_DATA= touchscreen-autodisabler.service
endif





EXTRA_DIST = \
	touchscreen-autodisabler.service.in \
	wsid.Tad.service.in \
        config.rpath m4


CLEANFILES= \
    $(systemduserunit_DATA) \
    $(dbussessionbusservices_DATA)


ACLOCAL_AMFLAGS = --install -I m4


# Add custom configure directory
################################
DISTCHECK_CONFIGURE_FLAGS = \
	SYSTEMD_USER_UNIT_DIR=$$dc_install_base/lib/systemd/user/ \
	DBUS_SESSION_BUS_SERVICES_DIR=$$dc_install_base/share/dbus-1/




# Target Hooks
##############
uninstall-local:
	-rm -r $(docdir)




# Custom Rules
##############

touchscreen-autodisabler.service: touchscreen-autodisabler.service.in
	$(AM_V_GEN) $(SED) \
	    -e "s,[@]bindir[@],$(bindir),g" \
	    -e "s,[@]SLEEP[@],$(SLEEP),g" \
	    < $< > $@

wsid.Tad.service: wsid.Tad.service.in
	$(AM_V_GEN) $(SED) \
	    -e "s,[@]bindir[@],$(bindir),g" \
	    -e "s,[@]SLEEP[@],$(SLEEP),g" \
	    < $< > $@

