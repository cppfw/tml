# Author: Ivan Gagis <igagis@gmail.com>

#==============================================================================
#==============================================================================
#==============================================================================
#                        Project configuration part
#             change these strings to configure project building

#default platform. It may be overriden by specifying platform=xxx in command line when running make
#Known platforms are:
#    linux
#    maemo_linux
#    win32
#    win32cross
include ../platform.mk

debug := false

name := libstob

so_name := 0

ifeq ($(platform), linux)
    name := $(name).so
endif
ifeq ($(platform), macosx)
    name := $(name).so
endif
ifeq ($(platform), windows)
    name := $(name).dll
endif



#Sources
srcs :=
srcs += stob/parser.cpp
srcs += stob/dom.cpp



defines :=

ifeq ($(debug), true)
    defines += -DDEBUG
endif

ifeq ($(platform), maemo_linux)
    defines += -DM_MAEMO
endif



compiler_flags := -Wall #enable all warnings
compiler_flags += -Wno-comment #no warnings on nested comments
compiler_flags += -funsigned-char #the 'char' type is unsigned
compiler_flags += -Wnon-virtual-dtor #warn if base class has non-virtual destructor
compiler_flags += -fno-operator-names #do not treat 'and', 'bitand','bitor','compl','not','or','xor' as keywords
compiler_flags += -Werror #treat warnings as errors
compiler_flags += -Wfatal-errors # stop on first error encountered
compiler_flags += -fstrict-aliasing #strict aliasing (in order to comply with the c++ standard more strictly)
compiler_flags += -g #include debugging symbols
                #-P -E #-Wreorder
                #-O3 -funroll-loops -fomit-frame-pointer
compiler_flags += -std=c++11



ifeq ($(platform), linux)
    compiler_flags += -fPIC # Since we are building shared library, we need Position-Independend Code
endif
ifeq ($(platform), maemo_linux)
    compiler_flags += -fPIC # Since we are building shared library, we need Position-Independend Code
endif
ifeq ($(platform), windows)
    compiler_flags += -Wno-format #due to bugs in some g++ versions it warns about format when not needed, so disable format warnings
endif


linker_flags := -shared

ifeq ($(platform), linux)
    linker_flags += -Wl,-soname,$(name).$(so_name)
endif
ifeq ($(platform), windows)
    linker_flags += -s
endif



#=======================
#  Include directories
#=======================
ifeq ($(platform), maemo_linux)
    include_dirs :=
endif
ifeq ($(platform), linux)
    include_dirs :=
endif
ifeq ($(platform), windows)
    include_dirs := -I/usr/include
endif



#============================
#  Libs and lib directories
#============================
ifeq ($(platform), maemo_linux)
    libs := 
endif
ifeq ($(platform), linux)
    libs := -lting -lpthread
endif
ifeq ($(platform), windows)
    libs := -L/usr/lib -lting
endif
ifeq ($(platform), macosx)
    libs := -lting
endif




#                     __
#                    /  \__/  end of configuration part
#==============================================================================


obj_dir := obj/1/2
compiler := g++


#==============================================================================
#=============================TARGETS==========================================
#==============================================================================

#==========================
#=project default target=
default: create-obj-dir $(name)

create-obj-dir:
	@mkdir -p $(obj_dir)



#find all .cpp files and get an .o file name for it to get dependencies for this target
$(name): $(addprefix $(obj_dir)/,$(patsubst %.cpp,%.o,$(srcs)))
	@echo Linking $@...
ifeq ($(platform), windows)
	@ar cr $(patsubst %.dll,%.a,$@) $^
	@$(compiler) $^ -o "$@" $(libs) $(linker_flags)
else
	@ar cr $(patsubst %.so,%.a,$@) $^
	@$(compiler) $^ -o "$@".$(so_name) $(libs) $(linker_flags)
	@ln -f -s $@.$(so_name) $@
endif


#======================
#=compile .cpp sources=
$(obj_dir)/%.o:%.cpp
	@echo Compiling $<...
	@mkdir -p $(dir $@)
# -MF option specifies dependency output file name
	@$(compiler) -c -MF "$(patsubst %.o,%.d,$@)" -MD -o "$@" $(compiler_flags) $(defines) $(include_dirs) $<
#workaround for gcc 4.2 compiler (it behaves differently than gcc 4.1 for some reason)
#ifeq ($(platform), linux)
#	@echo -n $(obj_dir)/ > $(patsubst %.o,%.d_,$@)
#	@cat $(patsubst %.o,%.d,$@) >> $(patsubst %.o,%.d_,$@)
#	@mv $(patsubst %.o,%.d_,$@) $(patsubst %.o,%.d,$@)
#endif

include $(wildcard $(addsuffix /*.d,$(dir $(addprefix $(obj_dir)/,$(srcs)))))



#==================
#=build all target=
all: clean proj


test:

#==============
#=clean target=
#delete all objects and executables

#it is an abstract target (not a file), declare as .PHONY
.PHONY: clean
clean:
ifeq ($(platform),windows)
	@rm -rf $(name)
	@rm -rf obj
else
	@rm -rf $(name)
	@rm -rf $(patsubst %.so,%.a,$(name))
	@rm -rf $(name).$(so_name)
	@rm -rf $(dir $(obj_dir))
endif

include ../targets.mk
