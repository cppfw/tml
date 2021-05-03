#include <tst/set.hpp>
#include <tst/check.hpp>

#include "../../src/treeml/tree_ext.hpp"

namespace{
tst::set set0("to_non_ext", [](auto& suite){
    suite.template add<std::string>(
            "tree_to_non_ext",
            {
                "hello",
                "hello {world!}",
                "hello{world!{how \"are\" you?}}",
            },
            [](auto& p){
                auto in = treeml::read_ext(p)[0];
                auto expected = treeml::read(p)[0];

                auto out = treeml::to_non_ext(in);

                tst::check_eq(out, expected, SL);
            }
        );
    
    suite.template add<std::string>(
            "forest_to_non_ext",
            {
                "hello world!",
                "hello{world!{how \"are\" you?}} world!",
                "hello{world!{how \"are\" you?}} world!{bla{one two three} bla bla}",
            },
            [](auto& p){
                auto in = treeml::read_ext(p);
                auto expected = treeml::read(p);

                auto out = treeml::to_non_ext(in);

                tst::check_eq(out, expected, SL);
            }
        );
});
}
