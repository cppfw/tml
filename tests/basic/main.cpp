#include "../../src/stob.hpp"

#include <string>
#include <deque>

#include <ting/debug.hpp>
#include <ting/fs/FSFile.hpp>



namespace TestSimpleQuotedStringParsing{

enum E_Action{
	CHILDREN_START,
	CHILDREN_END,
	STRING
};

class Listener : public stob::ParseListener{
	//override
	void OnChildrenParseFinished(){
		ASSERT_ALWAYS(this->actions.size() > 0)
		ASSERT_INFO_ALWAYS(this->actions.front().first == CHILDREN_END, "first = " << this->actions.front().first << " second = " << this->actions.front().second)
		this->actions.pop_front();
	}
	
	//override
	void OnChildrenParseStarted(){
		ASSERT_ALWAYS(this->actions.size() > 0)
		ASSERT_INFO_ALWAYS(this->actions.front().first == CHILDREN_START, "first = " << this->actions.front().first << " second = " << this->actions.front().second)
		this->actions.pop_front();
	}
	
	//override
	void OnStringParsed(const char* s, ting::u32 size){
		ASSERT_ALWAYS(this->actions.size() > 0)
		ASSERT_INFO_ALWAYS(this->actions.front().first == STRING, "first = " << this->actions.front().first << " second = " << this->actions.front().second)
		ASSERT_INFO_ALWAYS(this->actions.front().second == std::string(s, size), "first = " << this->actions.front().first << " second = " << this->actions.front().second << " str = " << std::string(s, size))
		this->actions.pop_front();
	}
	
public:
	
	std::deque<std::pair<E_Action, std::string> > actions;
};

void Run(){
	Listener l;
	
	typedef std::pair<E_Action, std::string> T_Pair;
	l.actions.push_back(T_Pair(STRING, "test string"));
	l.actions.push_back(T_Pair(STRING, "another string"));
	
	ting::fs::FSFile fi("test.stob");
	
	stob::Parse(fi, l);
	
	ASSERT_ALWAYS(l.actions.size() == 0)
}

}




int main(int argc, char** argv){

	TestSimpleQuotedStringParsing::Run();
	
	
	return 0;
}
