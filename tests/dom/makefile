platform := linux

operating_system := $(shell uname -o)
ifeq ($(operating_system), Msys)
    platform := win32
endif

binary_name := tests

ifeq ($(platform),win32)
    binary_name := $(binary_name).exe
endif


ifeq ($(platform),win32)
    libs := ../../src/libstob.a -lting -L/usr/lib
    includes := -I"../../../ting/src"
endif
ifeq ($(platform),darwin)
    libs := 
    includes :=
endif
ifeq ($(platform),linux)
    libs := -lting ../../src/libstob.so
    includes :=
endif

#external compiler flags (to be passed to make from command line)
ecf :=

#compiler flags
cf := $(ecf)
cf += -Wall
cf += -Wno-format #no warnings about format
cf += -DDEBUG
cf += -fstrict-aliasing #strict aliasing!!!

default: $(binary_name)

test: $(binary_name)
	@echo running $^...
	@LD_LIBRARY_PATH=../../src ./$^

$(binary_name): main.cpp
	@echo Building $@...
	@g++ -o $@ $(cf) $^ $(includes) $(libs)



clean:
	@rm -rf $(binary_name)

include ../../targets.mk
