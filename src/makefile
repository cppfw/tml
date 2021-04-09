include prorab.mk

$(eval $(call prorab-config, ../config))

this_name := treeml

this_soname := $(shell cat $(prorab_this_dir)soname.txt)

this_srcs += $(call prorab-src-dir, .)

this_ldlibs += -lutki -lpapki

$(eval $(prorab-build-lib))
