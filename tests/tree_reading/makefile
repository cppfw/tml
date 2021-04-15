include prorab.mk
include prorab-test.mk

$(eval $(call prorab-config, ../../config))

this_name := tests

this_srcs += $(call prorab-src-dir, .)

this_ldflags += -L$(d)../../src/out/$(c)

this_ldlibs += -ltreeml -lpapki -lutki

this_no_install := true

$(eval $(prorab-build-app))

# add dependency on libtreeml
$(prorab_this_name): $(abspath $(d)../../src/out/$(c)/libtreeml$(dot_so))

this_test_cmd := $(prorab_this_name)
this_test_deps := $(prorab_this_name)
this_test_ld_path := ../../src/out/$(c)
$(eval $(prorab-test))

$(eval $(call prorab-try-include,$(d)../../src/makefile))
