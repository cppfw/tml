include prorab.mk

debug := false


this_name := stob

this_soname := $(shell cat $(prorab_this_dir)soname.txt)


this_srcs += stob/parser.cpp
this_srcs += stob/dom.cpp


this_cxxflags += -Wall #enable all warnings
this_cxxflags += -Wno-comment #no warnings on nested comments
this_cxxflags += -funsigned-char #the 'char' type is unsigned
this_cxxflags += -Wnon-virtual-dtor #warn if base class has non-virtual destructor
this_cxxflags += -fno-operator-names #do not treat 'and', 'bitand','bitor','compl','not','or','xor' as keywords
this_cxxflags += -Werror #treat warnings as errors
this_cxxflags += -Wfatal-errors # stop on first error encountered
this_cxxflags += -fstrict-aliasing #strict aliasing (in order to comply with the c++ standard more strictly)
this_cxxflags += -g #include debugging symbols

this_cxxflags += -std=c++11



ifeq ($(debug), true)
    this_cxxflags += -DDEBUG
else
    this_cxxflags += -O3
                #-P -E #-Wreorder
endif

this_ldlibs += -lunikod -lpapki -lstdc++

ifeq ($(prorab_os), windows)
    this_cxxflags += -Wno-format #due to bugs in some g++ versions it warns about format when not needed, so disable format warnings
else ifeq ($(prorab_os), macosx)
    this_cxxflags += -stdlib=libc++ #this is needed to be able to use c++11 std lib
    this_ldlibs += -lc++
else
    this_ldlibs +=  -lpthread
    this_cxxflags += -fPIC # Since we are building shared library, we need Position-Independend Code
endif

$(eval $(prorab-build-lib))
