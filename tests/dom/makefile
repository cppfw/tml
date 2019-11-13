include prorab.mk

this_name := tests

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

this_ldlibs += -lpuu -lpapki -lstdc++

this_no_install := true

$(eval $(prorab-build-app))


include $(d)../test_rule.mk

#add dependency on libpuu
$(prorab_this_name): $(abspath $(d)../../src/libpuu$(soext))

$(eval $(call prorab-try-include,$(d)../../src/makefile))
