include prorab.mk
include prorab-test.mk

$(eval $(call prorab-config, ../../config))

this_name := cat_treeml

this_srcs += $(call prorab-src-dir, .)

this_ldflags += -L$(d)../../src/out/$(c)
this_ldlibs += -ltreeml -lpapki -lclargs -lutki

this_no_install := true

$(eval $(prorab-build-app))

# add dependency on libtreeml
$(prorab_this_name): $(abspath $(d)../../src/out/$(c)/libtreeml$(dot_so))

this_test_cmd := for f in $(shell cd $(d) && ls samples/*.tml); do \
                    $(prorab_this_name) $$$$f -f | cmp --print-bytes $(d)$$$$f.formatted && \
                    $(prorab_this_name) $$$$f | cmp --print-bytes $(d)$$$$f.unformatted; \
                done
this_test_deps := $(prorab_this_name)
this_test_ld_path := ../../src/out/$(c)
$(eval $(prorab-test))

$(eval $(call prorab-try-include,$(d)../../src/makefile))
