include prorab.mk
include prorab-test.mk

$(eval $(call prorab-config, ../../config))

this_name := unit_tests

this_srcs := $(call prorab-src-dir,.)

this_libtreeml := $(d)../../src/out/$(c)/libtreeml$(dot_so)

this_ldlibs += $(this_libtreeml) -ltst

$(eval $(prorab-build-app))

$(eval $(call prorab-depend, $(prorab_this_name), $(this_libtreeml)))

this_test_cmd := $(prorab_this_name)
this_test_deps := $(prorab_this_name)
this_test_ld_path := $(dir $(this_libtreeml))
$(eval $(prorab-test))

$(eval $(call prorab-include, ../../src/makefile))
