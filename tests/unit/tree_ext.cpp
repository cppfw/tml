#include <tst/set.hpp>
#include <tst/check.hpp>

#include "../../src/treeml/tree_ext.hpp"

namespace{
tst::set set0("tree_ext", [](auto& suite){
    suite.add("compare_leaf_ext_to_string", [](){
        std::string str = "hello world!";
        treeml::leaf_ext l{std::string(str)};
        tst::check(l == str.c_str(), SL);
        tst::check(l == str, SL);
    });

    suite.add("compare_tree_ext_to_string", [](){
        std::string str = "hello world!";
        treeml::tree_ext t{std::string(str)};
        tst::check(t == str.c_str(), SL);
        tst::check(t == str, SL);
    });

    suite.add("find_tree_from_forest_by_string", [](){
        std::string str = "hello world!";
        treeml::forest_ext f{
            {"hi"},
            {"how are you?"},
            {str},
            {"I'm fine"},
        };

        auto i = std::find(f.begin(), f.end(), str.c_str());
        tst::check(i != f.end(), SL);
    });
});
}
