#include "../../src/treeml/tree.hpp"

#include <utki/debug.hpp>
#include <papki/fs_file.hpp>

namespace{
struct predicate_str{
	std::string s;

	predicate_str(const std::string& s) : s(s){}

	bool operator()(const utki::tree<treeml::leaf>& l){
		return l.value == s;
	}
};
}

void test_treeml_crawling(){
	papki::fs_file fi("test.tml");

	auto roots = treeml::read(fi);

	// non-const crawler
	{
		auto& b6_1_1 = treeml::crawler(roots).to("b5").next().in().to_if(predicate_str("b6_1")).in().to("b6_1_1").get().value;

		ASSERT_ALWAYS(b6_1_1 == "b6_1_1")
	}

	// const crawler from non-const roots
	{
		auto& b6_1_1 = treeml::const_crawler(roots).to("b5").next().in().to_if(predicate_str("b6_1")).in().to("b6_1_1").get().value;

		ASSERT_ALWAYS(b6_1_1 == "b6_1_1")
	}

	// const crawler from const roots
	{
		const auto& const_roots = roots;
		auto& b6_1_1 = treeml::const_crawler(const_roots).to("b5").next().in().to_if(predicate_str("b6_1")).in().to("b6_1_1").get().value;

		ASSERT_ALWAYS(b6_1_1 == "b6_1_1")
	}

	// test 'to' fail
	{
		bool thrown = false;
		try{
			treeml::crawler(roots).to("b-1");
			ASSERT_ALWAYS(false)
		}catch(std::runtime_error& e){
			thrown = true;
		}
		ASSERT_ALWAYS(thrown)
	}

	// test 'to_if' fail
	{
		bool thrown = false;
		try{
			treeml::crawler(roots).to_if(predicate_str("b6-1"));
			ASSERT_ALWAYS(false)
		}catch(std::runtime_error& e){
			thrown = true;
		}
		ASSERT_ALWAYS(thrown)
	}

	// test 'in' fail
	{
		bool thrown = false;
		try{
			treeml::crawler(roots).to("b6").in().to("b6_1").in().to("b6_1_1").in();
			ASSERT_ALWAYS(false)
		}catch(std::logic_error& e){
			thrown = true;
		}
		ASSERT_ALWAYS(thrown)
	}

	// test 'next' fail
	{
		bool thrown = false;
		try{
			treeml::crawler(roots).to("b6").in().to("b6_1").next();
			ASSERT_ALWAYS(false)
		}catch(std::logic_error& e){
			thrown = true;
		}
		ASSERT_ALWAYS(thrown)
	}
}

int main(int argc, char** argv){

	test_treeml_crawling();

	return 0;
}
