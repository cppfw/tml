include prorab.mk
include prorab-test.mk

$(eval $(call prorab-config, ../../config))

this_no_install := true

this_name := tests

this_srcs := $(call prorab-src-dir, src)

this_libtml := $(d)../../src/out/$(c)/libtml$(dot_so)

this_ldlibs += $(this_libtml) -ltst -lutki -lpapki

$(eval $(prorab-build-app))

$(eval $(call prorab-depend, $(prorab_this_name), $(this_libtml)))

this_test_cmd := $(prorab_this_name) --jobs=auto --junit-out=out/$(c)/junit.xml
this_test_deps := $(prorab_this_name)
this_test_ld_path := $(dir $(this_libtml))
$(eval $(prorab-test))

$(eval $(call prorab-include, ../../src/makefile))
