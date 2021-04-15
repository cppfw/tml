include prorab.mk

define this_rules
install::
	$(prorab_echo)install -d $(DESTDIR)$(PREFIX)/share/gtksourceview-3.0/language-specs
	$(prorab_echo)install -m 644 $(d)gtkSourceView/* $(DESTDIR)$(PREFIX)/share/gtksourceview-3.0/language-specs
	$(prorab_echo)install -d $(DESTDIR)$(PREFIX)/share/kde4/apps/katepart/syntax
	$(prorab_echo)install -m 644 $(d)katepart/* $(DESTDIR)$(PREFIX)/share/kde4/apps/katepart/syntax
	$(prorab_echo)install -d $(DESTDIR)$(PREFIX)/share/katepart5/syntax
	$(prorab_echo)install -m 644 $(d)katepart/* $(DESTDIR)$(PREFIX)/share/katepart5/syntax
endef

ifeq ($(prorab_os), linux)
    $(eval $(this_rules))
endif
