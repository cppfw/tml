#include <tst/set.hpp>
#include <tst/check.hpp>

#include "../../../src/tml/tree_ext.hpp"

namespace{
const tst::set set("to_non_ext", [](auto& suite){
    suite.template add<std::string>(
            "tree_to_non_ext",
            {
                "hello",
                "hello {world!}",
                "hello{world!{how \"are\" you?}}",
            },
            [](auto& p){
                auto in = tml::read_ext(p)[0];
                auto expected = tml::read(p)[0];

                auto out = tml::to_non_ext(in);

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
                auto in = tml::read_ext(p);
                auto expected = tml::read(p);

                auto out = tml::to_non_ext(in);

                tst::check_eq(out, expected, SL);
            }
        );
});
}
