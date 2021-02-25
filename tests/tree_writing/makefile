include prorab.mk

this_name := cat_treeml

this_cxxflags += -Wall
this_cxxflags += -Wno-format #no warnings about format
this_cxxflags += -Wno-format-security #no warnings about format
this_cxxflags += -DDEBUG
this_cxxflags += -fstrict-aliasing #strict aliasing!!!
this_cxxflags += -std=c++11
this_cxxflags += -g #debugging symbols
this_cxxflags += -O3

this_srcs += main.cpp

this_ldflags += -L$(d)../../src/build

ifeq ($(os), macosx)
    this_cxxflags += -stdlib=libc++ #this is needed to be able to use c++11 std lib
    this_ldlibs += -lc++
else ifeq ($(os),windows)
endif

this_ldlibs += -ltreeml -lpapki -lclargs -lstdc++

this_no_install := true

$(eval $(prorab-build-app))

ifeq ($(os),windows)
    # to avoid /C converted to C:\ need to escape it as //C
    this_test_cmd = (cd $(d) && cmd //C "set PATH=../../src/build;%PATH% && cat_treeml$(exeext) $1")
else ifeq ($(os),macosx)
    this_test_cmd = (cd $(d) && DYLD_LIBRARY_PATH=../../src/build ./cat_treeml$(exeext) $1)
else ifeq ($(os),linux)
    this_test_cmd = (cd $(d) && LD_LIBRARY_PATH=../../src/build ./cat_treeml$(exeext) $1)
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

#add dependency on libtreeml
$(prorab_this_name): $(abspath $(d)../../src/build/libtreeml$(soext))

$(eval $(call prorab-try-include,$(d)../../src/makefile))
