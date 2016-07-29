srcpodir = $(top_srcdir)/po


# Copied from intltool's Makefile
# This will help this Makefile to inherit LINGUAS values from po.

ALL_LINGUAS = @ALL_LINGUAS@
PO_LINGUAS=$(shell if test -r $(srcpodir)/LINGUAS; then grep -v "^\#" $(srcpodir)/LINGUAS; else echo "$(ALL_LINGUAS)"; fi)
USER_LINGUAS=$(shell if test -n "$(LINGUAS)"; then LLINGUAS="$(LINGUAS)"; ALINGUAS="$(ALL_LINGUAS)"; for lang in $$LLINGUAS; do if test -n "`grep \^$$lang$$ $(srcdir)/LINGUAS 2>/dev/null`" -o -n "`echo $$ALINGUAS|tr ' ' '\n'|grep \^$$lang$$`"; then printf "$$lang "; fi; done; fi)
USE_LINGUAS=$(shell if test -n "$(USER_LINGUAS)" -o -n "$(LINGUAS)"; then LLINGUAS="$(USER_LINGUAS)"; else if test -n "$(PO_LINGUAS)"; then LLINGUAS="$(PO_LINGUAS)"; else LLINGUAS="$(ALL_LINGUAS)"; fi; fi; for lang in $$LLINGUAS; do printf "$$lang "; done)


# Man files.
INSTALL_MANFILES=$(USE_LINGUAS:=/touchscreen-autodisabler.7)

MANFILES= \
        touchscreen-autodisabler.7 \
        $(PO_LINGUAS:=/touchscreen-autodisabler.7)

dist_man7_MANS = touchscreen-autodisabler.7




EXTRA_DIST = \
        $(MANFILES) \
        $(MANFILES:=.markdown)





# Custom Build Rules
####################

if HAS_RONN

SUFFIXES= .7.markdown .7

all: $(INSTALL_MANFILES)

.7.markdown.7:
	$(AM_V_GEN) $(RONN) $< \
	    --manual=$(PACKAGE_NAME) \
	    --organization=$(VERSION) \
	    --roff > /dev/zero

endif


install-data-local: $(INSTALL_MANFILES)
	for L in $(USE_LINGUAS); \
	do $(INSTALL) -D $(srcdir)/$$L/touchscreen-autodisabler.7 \
	                 $(mandir)/$$L/man7/touchscreen-autodisabler.7;\
        done

uninstall-local:
	-for L in $(USE_LINGUAS); \
	 do rm -f $(mandir)/$$L/man7/touchscreen-autodisabler.7; \
	 done
