#include <tst/check.hpp>
#include <tst/set.hpp>

#include "../../../src/tml/tree.hpp"

namespace {
const tst::set set("snippets", [](auto& suite) {
	suite.template add<std::string>(
		"snippet__reading_fails",
		{
			R"(
                    def{
                        v{bla bla}
                    }
                    if{{}}}then{Hello}else{World}
                )",
		},
		[](const auto& p) {
			bool thrown = false;
			try {
				auto r = tml::read(p);
				tst::check(false, SL);
			} catch (std::invalid_argument const&) {
				thrown = true;
			} catch (...) {
				tst::check(false, SL);
			}
			tst::check(thrown, SL);
		}
	);
});
} // namespace
