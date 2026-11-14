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

# review: add $(eval $(prorab-clear-this-vars)) and after that add the tml-bnf library, in bnf_conformance tests add makefile similar to one in unit tests, use this_no_install := true for tml-bnf
