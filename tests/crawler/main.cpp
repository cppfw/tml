#include "../../src/puu/tree.hpp"

#include <utki/debug.hpp>
#include <papki/FSFile.hpp>

namespace{
struct predicate_str{
	std::string s;

	predicate_str(const std::string& s) : s(s){}

	bool operator()(const utki::tree<puu::leaf>& l){
		return l.value == s;
	}
};
}

void test_puu_crawling(){
	papki::FSFile fi("test.puu");

	auto roots = puu::read(fi);

	// non-const crawler
	{
		auto& b6_1_1 = puu::crawler(roots).to("b5").next().up().to_if(predicate_str("b6_1")).up().to("b6_1_1").get().value;

		ASSERT_ALWAYS(b6_1_1 == "b6_1_1")
	}

	// const crawler from non-const roots
	{
		auto& b6_1_1 = puu::const_crawler(roots).to("b5").next().up().to_if(predicate_str("b6_1")).up().to("b6_1_1").get().value;

		ASSERT_ALWAYS(b6_1_1 == "b6_1_1")
	}

	// const crawler from const roots
	{
		const auto& const_roots = roots;
		auto& b6_1_1 = puu::const_crawler(const_roots).to("b5").next().up().to_if(predicate_str("b6_1")).up().to("b6_1_1").get().value;

		ASSERT_ALWAYS(b6_1_1 == "b6_1_1")
	}

	// test 'to' fail
	{
		bool thrown = false;
		try{
			puu::crawler(roots).to("b-1");
			ASSERT_ALWAYS(false)
		}catch(utki::not_found& e){
			thrown = true;
		}
		ASSERT_ALWAYS(thrown)
	}

	// test 'to_if' fail
	{
		bool thrown = false;
		try{
			puu::crawler(roots).to_if(predicate_str("b6-1"));
			ASSERT_ALWAYS(false)
		}catch(utki::not_found& e){
			thrown = true;
		}
		ASSERT_ALWAYS(thrown)
	}

	// test 'up' fail
	{
		bool thrown = false;
		try{
			puu::crawler(roots).to("b6").up().to("b6_1").up().to("b6_1_1").up();
			ASSERT_ALWAYS(false)
		}catch(utki::not_found& e){
			thrown = true;
		}
		ASSERT_ALWAYS(thrown)
	}

	// test 'next' fail
	{
		bool thrown = false;
		try{
			puu::crawler(roots).to("b6").up().to("b6_1").next();
			ASSERT_ALWAYS(false)
		}catch(utki::not_found& e){
			thrown = true;
		}
		ASSERT_ALWAYS(thrown)
	}
}

int main(int argc, char** argv){

	test_puu_crawling();

	return 0;
}
