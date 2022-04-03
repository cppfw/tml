include prorab.mk

ifeq ($(os),linux)

$(eval $(call prorab-config, ../config))

this_name := print_tml

this_srcs := $(call prorab-src-dir, src)
this_srcs += out/parser.cpp

$(eval $(prorab-build-app))

# The rule for generating bisonc++ parser code.
# Note, that before invoking bisonc++ we remove the old generated files, because
# bisonc++ otherwise exists with error.
define this_rules

# main.cpp depends on bisonc++ generated header file which is generated along with
# parser.cpp, so add dependency on that
$(d)src/main.cpp: $(d)out/parser.cpp

$(d)out/parser.cpp: $(d)src/parser.y
$(.RECIPEPREFIX)@echo "bisonc++: generate code from $$<"
$(.RECIPEPREFIX)$(a)rm -f $(d)out/parser.hpp $(d)out/parser.cxx
$(.RECIPEPREFIX)$(a)bisonc++ --target-directory=$(d)out $$<
$(.RECIPEPREFIX)$(a)echo "#include \"../src/parser.cxx\"" > $(d)out/parser.cxx
$(.RECIPEPREFIX)$(a)echo "#pragma once\n#include \"../src/parser.hpp\"" > $(d)out/parser.hpp

endef
$(eval $(this_rules))

endif # linux
