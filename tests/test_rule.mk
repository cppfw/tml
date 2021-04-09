ifeq ($(os),windows)
    this_test_cmd := (cd $(d); cp ../../src/out/$(c)/libtreeml.dll out/$(c); $$^)
else
    this_test_cmd := (cd $(d); LD_LIBRARY_PATH=../../src/out/$(c) $$^)
endif

define this_rules
test:: $(prorab_this_name)
$(.RECIPEPREFIX)@myci-running-test.sh $$(notdir $$(abspath $$(dir $$^)))...
$(.RECIPEPREFIX)$(a)$(this_test_cmd)
$(.RECIPEPREFIX)@myci-passed.sh
endef
$(eval $(this_rules))
