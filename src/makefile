include prorab.mk
include prorab-license.mk
include prorab-clang-format.mk
include prorab-install-dbgsrc.mk

$(eval $(call prorab-config, ../config))

this_name := tml

this_soname := $(shell cat $(d)soname.txt)

this_srcs += $(call prorab-src-dir, .)

this_ldlibs += -lpapki -lutki

$(eval $(prorab-build-lib))

this_license_file := ../LICENSE
this_src_dir := $(this_name)
$(eval $(prorab-license))

$(eval $(prorab-clang-format))

$(eval $(prorab-install-dbgsrc))
