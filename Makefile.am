## Process this file with automake to produce Makefile.in

SUBDIRS = po src man

# Directory setup.
##################

servicedir=$(prefix)/lib/systemd/user/

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

service_DATA= \
	touchscreen-autodisabler.service


# Misc Files setup
##################

EXTRA_DIST = \
		touchscreen-autodisabler.service.in \
        config.rpath m4 ChangeLog


# File Generation
#################
touchscreen-autodisabler.service: touchscreen-autodisabler.service.in
	$(SED) \
	    -e "s,[@]bindir[@],$(bindir),g" \
	    -e "s,[@]SLEEP[@],$(SLEEP),g" \
	    < $< > $@



CLEANFILES= \
    $(service_DATA)

# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(docdir)

ACLOCAL_AMFLAGS = --install -I m4
