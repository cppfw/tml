include prorab.mk

this_name := tests

this_cflags += -Wall
this_cflags += -Wno-format #no warnings about format
this_cflags += -DDEBUG
this_cflags += -fstrict-aliasing #strict aliasing!!!
this_cflags += -std=c++11

this_srcs += main.cpp

this_ldlibs += -lstob -lting
this_ldflags += -L$(prorab_this_dir)../../src

ifeq ($(prorab_os),windows)
    this_cflags += -I/usr/include
    this_ldflags += -L/usr/lib
endif

$(eval $(prorab-build-app))


ifeq ($(prorab_os),windows)
    this_test_cmd := $(prorab_echo)(cd $(prorab_this_dir); cp ../../src/libstob.dll .; $$^)
else
    this_test_cmd := $(prorab_echo)(cd $(prorab_this_dir); LD_LIBRARY_PATH=../../src $$^)
endif


define this_rules
test:: $(prorab_this_name)
	@echo running $$^...
	$(this_test_cmd)
endef
$(eval $(this_rules))

#add dependency on libstob
$(prorab_this_name): $(abspath $(prorab_this_dir)../../src/libstob$(prorab_lib_extension))

$(eval $(call prorab-include,$(prorab_this_dir)../../src/makefile))
