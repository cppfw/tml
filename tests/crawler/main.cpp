#include "../../src/puu/tree.hpp"

#include <utki/debug.hpp>
#include <papki/FSFile.hpp>



void test_puu_crawling(){
	papki::FSFile fi("test.puu");

	auto roots = puu::read(fi);

	{
		auto& b6_1_1 = puu::crawler(roots).to("b5").next().up().to("b6_1").up().to("b6_1_1").get().value;

		ASSERT_ALWAYS(b6_1_1 == "b6_1_1")
	}

	// test 'to' fail
	{
		bool thrown = false;
		try{
			puu::crawler(roots).to("b-1");
			ASSERT_ALWAYS(false)
		}catch(puu::not_found_exception& e){
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
		}catch(puu::not_found_exception& e){
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
		}catch(puu::not_found_exception& e){
			thrown = true;
		}
		ASSERT_ALWAYS(thrown)
	}
}

int main(int argc, char** argv){

	test_puu_crawling();

	return 0;
}
