include prorab.mk

$(eval $(call prorab-config, ../../config))

this_name := tests

this_srcs += $(call prorab-src-dir, .)

this_ldflags += -L$(d)../../src/out/$(c)

this_ldlibs += -ltreeml -lpapki -lutki

this_no_install := true

$(eval $(prorab-build-app))

include $(d)../test_rule.mk

# add dependency on libtreeml
$(prorab_this_name): $(abspath $(d)../../src/out/$(c)/libtreeml$(dot_so))

$(eval $(call prorab-try-include,$(d)../../src/makefile))
