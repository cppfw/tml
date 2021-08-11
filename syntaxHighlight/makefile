include prorab.mk

define this_rules
install::
$(.RECIPEPREFIX)$(a)install -d $(DESTDIR)$(PREFIX)/share/gtksourceview-3.0/language-specs
$(.RECIPEPREFIX)$(a)install -m 644 $(d)gtkSourceView/* $(DESTDIR)$(PREFIX)/share/gtksourceview-3.0/language-specs
$(.RECIPEPREFIX)$(a)install -d $(DESTDIR)$(PREFIX)/share/kde4/apps/katepart/syntax
$(.RECIPEPREFIX)$(a)install -m 644 $(d)katepart/* $(DESTDIR)$(PREFIX)/share/kde4/apps/katepart/syntax
$(.RECIPEPREFIX)$(a)install -d $(DESTDIR)$(PREFIX)/share/katepart5/syntax
$(.RECIPEPREFIX)$(a)install -m 644 $(d)katepart/* $(DESTDIR)$(PREFIX)/share/katepart5/syntax
endef

ifeq ($(os), linux)
    $(eval $(this_rules))
endif
