include prorab.mk

this_name := cat_puu

this_cxxflags += -Wall
this_cxxflags += -Wno-format #no warnings about format
this_cxxflags += -Wno-format-security #no warnings about format
this_cxxflags += -DDEBUG
this_cxxflags += -fstrict-aliasing #strict aliasing!!!
this_cxxflags += -std=c++11
this_cxxflags += -g #debugging symbols
this_cxxflags += -O3

this_srcs += main.cpp

this_ldflags += -L$(d)../../src

ifeq ($(os), macosx)
    this_cxxflags += -stdlib=libc++ #this is needed to be able to use c++11 std lib
    this_ldlibs += -lc++
else ifeq ($(os),windows)
endif

this_ldlibs += -lpuu -lpapki -lclargs -lstdc++

this_no_install := true

$(eval $(prorab-build-app))

ifeq ($(os),windows)
    # to avoid /C converted to C:\ need to escape it as //C
    this_test_cmd = cmd //C 'set PATH=$(d)../../src;%PATH% && $$^ $1'
else ifeq ($(os),macosx)
    this_test_cmd = DYLD_LIBRARY_PATH=$(d)../../src $$^ $1
else ifeq ($(os),linux)
    this_test_cmd = LD_LIBRARY_PATH=$(d)../../src $$^ $1
else
    $(error "Unknown OS")
endif

define this_rules
test:: $(prorab_this_name)
$(.RECIPEPREFIX)@myci-running-test.sh $(notdir $(abspath $(d)))...
$(.RECIPEPREFIX)$(Q)for f in $(shell ls $(d)samples/*.puu); do \
                    $(call this_test_cmd, $$$$f -f) > $(d)a.txt; \
                    $(call this_test_cmd, $$$$f) > $(d)b.txt; \
                    ls -l $(d)a.txt $(d)b.txt $$$$f.*formatted; \
                    md5sum $(d)a.txt $(d)b.txt $$$$f.*formatted; \
                    echo a.txt = ; cat $(d)a.txt ; xxd $(d)a.txt ; \
                    $(call this_test_cmd, $$$$f -f) | cmp --print-bytes $$$$f.formatted && \
                    $(call this_test_cmd, $$$$f) | cmp --print-bytes $$$$f.unformatted; \
                done
$(.RECIPEPREFIX)@myci-passed.sh
endef
$(eval $(this_rules))

#add dependency on libpuu
$(prorab_this_name): $(abspath $(d)../../src/libpuu$(soext))

$(eval $(call prorab-try-include,$(d)../../src/makefile))
