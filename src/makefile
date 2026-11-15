include prorab.mk
include prorab-license.mk
include prorab-clang-format.mk
include prorab-install-dbgsrc.mk

$(eval $(call prorab-config, ../config))

this_src_dir := tml

this_name := $(this_src_dir)$(this_dbg)

this_soname := $(shell cat $(d)soname.txt)

this_srcs += $(call prorab-src-dir, $(this_src_dir))

this_ldlibs += -l fsif$(this_dbg)
this_ldlibs += -l utki$(this_dbg)

$(eval $(prorab-build-lib))

this_license_file := ../LICENSE
$(eval $(prorab-license))

$(eval $(prorab-clang-format))

$(eval $(prorab-install-dbgsrc))

# =====================================
# === tml-bnf (bison-based parser) ===

$(eval $(prorab-clear-this-vars))

$(eval $(call prorab-config, ../config))

this_no_install := true

this_soname := $(shell cat $(d)soname.txt)

this_src_dir := bnf

this_name := tml-bnf$(this_dbg)

this_srcs += $(call prorab-src-dir, $(this_src_dir))

# so that #include <tml/tree.hpp> in the bison-generated file resolves
# to the local source (same as CMake's PUBLIC_INCLUDE_DIRECTORIES src/)
this_cxxflags += -I .

this_ldlibs += $(d)out/$(c)/libtml$(this_dbg)$(dot_so)
this_ldlibs += -l fsif$(this_dbg)
this_ldlibs += -l utki$(this_dbg)

# bison-generated parser
this__bnf_bin_dir := out/$(c)/bnf
this__bnf_cc := $(this__bnf_bin_dir)/tml_bnf.cpp
this__bnf_hh := $(this__bnf_bin_dir)/tml_bnf.hh

this_srcs += $(this__bnf_cc)
this_cxxflags += -I bnf
this_cxxflags += -I $(this__bnf_bin_dir)

define this__rules
	$(d)$(this__bnf_cc): $(d)bnf/tml.y
$(.RECIPEPREFIX)$(a)echo generate $$@
$(.RECIPEPREFIX)$(a)mkdir -p $$(dir $$@)
$(.RECIPEPREFIX)$(a)bison -o $$@ --defines=$(d)$(this__bnf_hh) $$<

	# tml_bnf.hh is a byproduct of the bison command above
	$(d)$(this__bnf_hh): $(d)$(this__bnf_cc)

	$(d)bnf/tml_bnf.cpp: $(d)$(this__bnf_hh)
	$(d)bnf/tml_bnf_grammar.cpp: $(d)$(this__bnf_hh)
	$(d)bnf/tml_bnf_lexer.cpp: $(d)$(this__bnf_hh)
endef
$(eval $(this__rules))

$(eval $(prorab-build-lib))
