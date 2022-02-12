#include <tst/set.hpp>
#include <tst/check.hpp>

#include "../../../src/treeml/tree.hpp"

namespace{
tst::set set("snippets", [](auto& suite){
    suite.template add<std::string>(
            "snippet_is_reading_fails",
            {
                R"(
                    def{
                        v{bla bla}
                    }
                    if{{}}}then{Hello}else{World}
                )",
            },
            [](const auto& p){
                try{
                    auto r = tml::read(p);
                    tst::check(false, SL);
                }catch(std::invalid_argument& e){
                }catch(...){
                    tst::check(false, SL);
                }
            }
        );
});
}
