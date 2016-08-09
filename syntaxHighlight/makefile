include prorab.mk

define this_rules
install::
	$(prorab_echo)install -d $(DESTDIR)$(PREFIX)/share/gtksourceview-3.0/language-specs
	$(prorab_echo)install $(prorab_this_dir)gtkSourceView/* $(DESTDIR)$(PREFIX)/share/gtksourceview-3.0/language-specs
	$(prorab_echo)install -d $(DESTDIR)$(PREFIX)/share/kde4/apps/katepart/syntax
	$(prorab_echo)install $(prorab_this_dir)katepart/* $(DESTDIR)$(PREFIX)/share/kde4/apps/katepart/syntax
endef
$(eval $(this_rules))
