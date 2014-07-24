include prorab.mk

this_name := tests

this_cflags := $(ecf)
this_cflags += -Wall
this_cflags += -Wno-format #no warnings about format
this_cflags += -DDEBUG
this_cflags += -fstrict-aliasing #strict aliasing!!!
this_cflags += -std=c++11

this_srcs := main.cpp

this_ldlibs := -lstob -lting
this_ldflags := -L$(prorab_this_dir)../../src

$(eval $(prorab-build-app))


define this_rules
test:: $(prorab_this_name)
	@echo running $$^...
	@(cd $(prorab_this_dir); LD_LIBRARY_PATH=../../src $$^)
endef
$(eval $(this_rules))

#add dependency on libstob
$(prorab_this_name): $(abspath $(prorab_this_dir)../../src/libstob$(prorab_lib_extension))

$(eval $(call prorab-include,$(prorab_this_dir)../../src/makefile))
