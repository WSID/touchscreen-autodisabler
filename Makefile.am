## Process this file with automake to produce Makefile.in

SUBDIRS = po src man



# Target setup
##############

dist_doc_DATA = \
	README \
	ABOUT-NLS \
	COPYING \
	AUTHORS \
	ChangeLog \
	INSTALL \
	NEWS


dbussessionbusservices_DATA= \
	wsid.Tad.service

if HAVE_SYSTEMD_USER_UNIT
systemduserunit_DATA= \
	touchscreen-autodisabler.service

endif

# Misc Files setup
##################

EXTRA_DIST = \
	touchscreen-autodisabler.service.in \
	wsid.Tad.service.in \
        config.rpath m4 ChangeLog


# File Generation
#################
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


CLEANFILES= \
    $(systemduserunit_DATA) \
    $(dbussessionbusservices_DATA)

# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(docdir)

ACLOCAL_AMFLAGS = --install -I m4


DISTCHECK_CONFIGURE_FLAGS = \
	--with-systemd-user-unit-dir=$$dc_install_base/$(systemduserunitdir)
