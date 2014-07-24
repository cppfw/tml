include prorab.mk

debug := false


this_name := stob

this_so_name := 0


this_srcs := stob/parser.cpp stob/dom.cpp


this_cflags := -Wall #enable all warnings
this_cflags += -Wno-comment #no warnings on nested comments
this_cflags += -funsigned-char #the 'char' type is unsigned
this_cflags += -Wnon-virtual-dtor #warn if base class has non-virtual destructor
this_cflags += -fno-operator-names #do not treat 'and', 'bitand','bitor','compl','not','or','xor' as keywords
this_cflags += -Werror #treat warnings as errors
this_cflags += -Wfatal-errors # stop on first error encountered
this_cflags += -fstrict-aliasing #strict aliasing (in order to comply with the c++ standard more strictly)
this_cflags += -g #include debugging symbols
                #-P -E #-Wreorder
                #-O3 -funroll-loops -fomit-frame-pointer
this_cflags += -std=c++11

ifeq ($(debug), true)
    this_cflags += -DDEBUG
endif

ifeq ($(prorab_os), windows)
    this_cflags += -Wno-format #due to bugs in some g++ versions it warns about format when not needed, so disable format warnings
else
    this_cflags += -fPIC # Since we are building shared library, we need Position-Independend Code
endif

ifeq ($(prorab_os), linux)
    this_ldlibs := -lting -lpthread
else ifeq ($(prorab_os), windows)
    this_ldlibs := -lting
    this_ldflags := -L/usr/lib
else ifeq ($(prorab_os), macosx)
    this_ldlibs := -lting
endif

$(eval $(prorab-build-lib))
