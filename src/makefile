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

this_src_dir := bnf

this_name := tml-bnf$(this_dbg)

this_srcs += $(call prorab-src-dir, $(this_src_dir))

this_cxxflags += -I .

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
endef
$(eval $(this__rules))

$(eval $(prorab-build-lib))
