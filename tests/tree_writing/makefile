include prorab.mk

$(eval $(call prorab-config, ../../config))

this_name := cat_treeml

this_srcs += $(call prorab-src-dir, .)

this_ldflags += -L$(d)../../src/out/$(c)
this_ldlibs += -ltreeml -lpapki -lclargs -lutki

this_no_install := true

$(eval $(prorab-build-app))

ifeq ($(os),windows)
    # to avoid /C converted to C:\ need to escape it as //C
    this_test_cmd = (cd $(d) && cmd //C "set PATH=../../src/out/$(c);%PATH% && out\$(c)\cat_treeml$(dot_exe) $1")
else ifeq ($(os),macosx)
    this_test_cmd = (cd $(d) && DYLD_LIBRARY_PATH=../../src/out/$(c) out/$(c)/cat_treeml$(dot_exe) $1)
else ifeq ($(os),linux)
    this_test_cmd = (cd $(d) && LD_LIBRARY_PATH=../../src/out/$(c) out/$(c)/cat_treeml$(exeext) $1)
else
    $(error "Unknown OS")
endif

define this_rules
test:: $(prorab_this_name)
$(.RECIPEPREFIX)@myci-running-test.sh $(notdir $(abspath $(d)))...
$(.RECIPEPREFIX)$(Q)for f in $(shell cd $(d) && ls samples/*.tml); do \
                    $(call this_test_cmd, $$$$f -f) | cmp --print-bytes $(d)$$$$f.formatted && \
                    $(call this_test_cmd, $$$$f) | cmp --print-bytes $(d)$$$$f.unformatted; \
                done
$(.RECIPEPREFIX)@myci-passed.sh
endef
$(eval $(this_rules))

# add dependency on libtreeml
$(prorab_this_name): $(abspath $(d)../../src/out/$(c)/libtreeml$(dot_so))

$(eval $(call prorab-try-include,$(d)../../src/makefile))
