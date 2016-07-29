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

dist_service_DATA= \
	touchscreen-autodisabler.service


# Misc Files setup
##################

EXTRA_DIST = \
        config.rpath m4 ChangeLog




# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(docdir)

ACLOCAL_AMFLAGS = --install -I m4
