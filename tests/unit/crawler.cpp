#include <tst/set.hpp>
#include <tst/check.hpp>

#include "../../src/treeml/tree.hpp"

namespace{
const auto sample = R"qwertyuiop(
b1 b2
b3{ b3_1 b3_2 }

b4 { b4_1 b4_2 b4_3 b4_4 }

b5{ b5_1}

b6{ b6_1 {b6_1_1 b6_1_2} }

b7

b8 {b8_1 b8_2{b8_2_1} }
)qwertyuiop";

const auto const_roots = treeml::read(sample);
auto roots = treeml::read(sample);
}

namespace{
struct predicate_str{
	std::string s;

	predicate_str(const std::string& s) : s(s){}

	bool operator()(const utki::tree<treeml::leaf>& l){
		return l.value == s;
	}
};
}

namespace{
tst::set set0("crawler", [](auto& suite){
	suite.add("non_const_crawler", [](){
		auto& b6_1_1 = treeml::crawler(roots).to("b5").next().in().to_if(predicate_str("b6_1")).in().to("b6_1_1").get().value;
		tst::check_eq(b6_1_1, treeml::leaf("b6_1_1"), SL);
	});

	suite.add("const_crawler_from_non_const_roots", [](){
		auto& b6_1_1 = treeml::const_crawler(roots).to("b5").next().in().to_if(predicate_str("b6_1")).in().to("b6_1_1").get().value;
		tst::check_eq(b6_1_1, treeml::leaf("b6_1_1"), SL);
	});

	suite.add("const_crawler_from_const_roots", [](){
		auto& b6_1_1 = treeml::const_crawler(const_roots).to("b5").next().in().to_if(predicate_str("b6_1")).in().to("b6_1_1").get().value;
		tst::check_eq(b6_1_1, treeml::leaf("b6_1_1"), SL);
	});

	suite.add("function_to_should_fail", [](){
		bool thrown = false;
		try{
			treeml::crawler(roots).to("b-1");
			tst::check(false, SL);
		}catch(std::runtime_error& e){
			thrown = true;
		}
		tst::check(thrown, SL);
	});

	suite.add("function_to_if_should_fail", [](){
		bool thrown = false;
		try{
			treeml::crawler(roots).to_if(predicate_str("b6-1"));
			tst::check(false, SL);
		}catch(std::runtime_error& e){
			thrown = true;
		}
		tst::check(thrown, SL);
	});

	suite.add("function_in_should_fail", [](){
		bool thrown = false;
		try{
			treeml::crawler(roots).to("b6").in().to("b6_1").in().to("b6_1_1").in();
			tst::check(false, SL);
		}catch(std::logic_error& e){
			thrown = true;
		}
		tst::check(thrown, SL);
	});

	suite.add("function_next_should_fail", [](){
		bool thrown = false;
		try{
			treeml::crawler(roots).to("b6").in().to("b6_1").next();
			tst::check(false, SL);
		}catch(std::logic_error& e){
			thrown = true;
		}
		tst::check(thrown, SL);
	});
});
}
