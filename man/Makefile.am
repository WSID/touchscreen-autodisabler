srcpodir = $(top_srcdir)/po


dist_ko_man7_MANS = ko/touchscreen-autodisabler.7
dist_man7_MANS = touchscreen-autodisabler.7

EXTRA_DIST = \
        $(dist_ko_man7_MANS:=.markdown) \
        $(dist_man7_MANS:=.markdown)





# Custom Build Rules
####################

if HAS_RONN

SUFFIXES= .7.markdown .7

.7.markdown.7:
	$(AM_V_GEN) $(RONN) $< \
	    --manual=$(PACKAGE_NAME) \
	    --organization=$(VERSION) \
	    --roff > /dev/zero

endif
