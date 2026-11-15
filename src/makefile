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

# review: the so name should be same as for tml
this_soname := tml-bnf

this_src_dir := bnf

this_name := tml-bnf$(this_dbg)

this_srcs += $(call prorab-src-dir, $(this_src_dir))

# review: why is this needed?
this_cxxflags += -I .

# review the -l tml would lint to tml in system lib location but we just built the tml, need to link directly to the .so from out/$(c)/libtml$(dot_so), check how it is done in other libs
this_ldlibs += -l tml$(this_dbg)
this_ldlibs += -l fsif$(this_dbg)
this_ldlibs += -l utki$(this_dbg)

# bison-generated parser
this_bnf_bin_dir := $(d)out/$(c)/bnf
tml_bnf_cc := $(this_bnf_bin_dir)/tml_bnf.cc
tml_bnf_hh := $(this_bnf_bin_dir)/tml_bnf.hh

this_srcs += $(tml_bnf_cc)
this_cxxflags += -I $(this_bnf_bin_dir)

define this__rules
	$(d)$(tml_bnf_cc): $(d)bnf/tml.y
$(.RECIPEPREFIX)$(a)echo generate $$@
$(.RECIPEPREFIX)$(a)mkdir -p $$(dir $$@)
$(.RECIPEPREFIX)$(a)bison -o $$@ --defines=$(tml_bnf_hh) $$<

	# tml_bnf.hh is a byproduct of the bison command above
	$(tml_bnf_hh): $(tml_bnf_cc)

	$(d)bnf/tml_bnf.cpp: $(tml_bnf_hh)
	$(d)bnf/tml_bnf_grammar.cpp: $(tml_bnf_hh)
	$(d)bnf/tml_bnf_lexer.cpp: $(tml_bnf_hh)
endef
$(eval $(this__rules))

$(eval $(prorab-build-lib))
