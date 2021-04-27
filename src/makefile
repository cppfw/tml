include prorab.mk

$(eval $(call prorab-config, ../config))

this_name := treeml

this_soname := $(shell cat $(d)soname.txt)

this_srcs += $(call prorab-src-dir, .)

this_ldlibs += -lpapki -lutki

$(eval $(prorab-build-lib))
