#include "../../src/stob/parser.hpp"

#include <string>
#include <deque>

#include <ting/debug.hpp>
#include <ting/fs/FSFile.hpp>
#include <ting/util.hpp>



namespace TestBasicParsing{

enum E_Action{
	CHILDREN_START,
	CHILDREN_END,
	STRING
};

class Listener : public stob::ParseListener{
	void OnChildrenParseFinished() OVERRIDE{
		ASSERT_ALWAYS(this->actions.size() > 0)
		ASSERT_INFO_ALWAYS(this->actions.front().first == CHILDREN_END, "first = " << this->actions.front().first << " second = " << this->actions.front().second)
		this->actions.pop_front();
	}
	
	void OnChildrenParseStarted() OVERRIDE{
		ASSERT_ALWAYS(this->actions.size() > 0)
		ASSERT_INFO_ALWAYS(this->actions.front().first == CHILDREN_START, "first = " << this->actions.front().first << " second = " << this->actions.front().second)
		this->actions.pop_front();
	}
	
	void OnStringParsed(const ting::Buffer<const char>& str) OVERRIDE{
		ASSERT_ALWAYS(this->actions.size() > 0)
		ASSERT_INFO_ALWAYS(this->actions.front().first == STRING, "first = " << this->actions.front().first << " second = " << this->actions.front().second)
		ASSERT_INFO_ALWAYS(this->actions.front().second == std::string(str.Begin(), str.Size()), "first = " << this->actions.front().first << " second = " << this->actions.front().second << " str = " << std::string(str.Begin(), str.Size()))
		this->actions.pop_front();
	}
	
public:
	~Listener()throw(){}
	
	std::deque<std::pair<E_Action, std::string> > actions;
};

void Run(){
	Listener l;
	
	typedef std::pair<E_Action, std::string> T_Pair;
	l.actions.push_back(T_Pair(STRING, "test string"));
	l.actions.push_back(T_Pair(STRING, "another string"));
	l.actions.push_back(T_Pair(STRING, "unquotedString"));
	
	l.actions.push_back(T_Pair(STRING, "string with empty children list"));
	l.actions.push_back(T_Pair(CHILDREN_START, ""));
	l.actions.push_back(T_Pair(CHILDREN_END, ""));
	
	l.actions.push_back(T_Pair(STRING, "unquoted_string_with_empty_children_list"));
	l.actions.push_back(T_Pair(CHILDREN_START, ""));
	l.actions.push_back(T_Pair(CHILDREN_END, ""));
	
	
	l.actions.push_back(T_Pair(STRING, ""));
	l.actions.push_back(T_Pair(CHILDREN_START, ""));
	
	l.actions.push_back(T_Pair(STRING, "child string"));
	
	l.actions.push_back(T_Pair(STRING, "aaa"));
	l.actions.push_back(T_Pair(CHILDREN_START, ""));
	l.actions.push_back(T_Pair(CHILDREN_END, ""));
	
	l.actions.push_back(T_Pair(STRING, "bbb"));
	
	l.actions.push_back(T_Pair(STRING, "ccc"));
	l.actions.push_back(T_Pair(CHILDREN_START, ""));
	l.actions.push_back(T_Pair(STRING, "ddd"));
	l.actions.push_back(T_Pair(CHILDREN_END, ""));
	
	l.actions.push_back(T_Pair(CHILDREN_END, ""));
	
	l.actions.push_back(T_Pair(STRING, "Tab\t Backslash\\ Slash/ Doubleslash// Newline\n Carriagereturn\r Doublequotes\" {}{}{}{}"));
	
	l.actions.push_back(T_Pair(STRING, "quoted string with trailing slash /"));
	
	l.actions.push_back(T_Pair(STRING, "fff ggg"));
	
	l.actions.push_back(T_Pair(STRING, "tralala tro lo lo\ntre lele"));
	
	l.actions.push_back(T_Pair(STRING, "-1213.33"));
	
	l.actions.push_back(T_Pair(STRING, "UnquotedStringAtTheVeryEndOfTheFile"));
	
	ting::fs::FSFile fi("test.stob");
	
	stob::Parse(fi, l);
	
	ASSERT_ALWAYS(l.actions.size() == 0)
}

}




int main(int argc, char** argv){

	TestBasicParsing::Run();
	
	
	return 0;
}
