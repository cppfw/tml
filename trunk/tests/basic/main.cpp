#include "../../src/stob.hpp"

#include <string>

#include <ting/debug.hpp>
#include <ting/fs/FSFile.hpp>



namespace TestSimpleQuotedStringParsing{

class Listener : public stob::ParseListener{
	//override
	void OnChildrenParseFinished(){
		ASSERT_ALWAYS(false)
	}
	
	//override
	void OnChildrenParseStarted(){
		ASSERT_ALWAYS(false)
	}
	
	//override
	void OnStringParsed(const char* s, ting::u32 size){
		ASSERT_INFO_ALWAYS(std::string(s, size) == "test string", "str = " << std::string(s, size))
		this->called = true;
	}
	
public:
	ting::Inited<bool, false> called;
};

void Run(){
	Listener l;
	
	ting::fs::FSFile fi("test1.stob");
	
	stob::Parse(fi, l);
	
	ASSERT_ALWAYS(l.called)
}

}




int main(int argc, char** argv){

	TestSimpleQuotedStringParsing::Run();
	
	
	return 0;
}
