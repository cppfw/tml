#include "../../src/stob/dom.hpp"

#include <ting/debug.hpp>
#include <ting/fs/FSFile.hpp>



namespace TestBasicParsing{

void Run(){
	ting::fs::FSFile fi("test.stob");
	
	ting::Ptr<stob::Node> root = stob::Load(fi);
	ASSERT_ALWAYS(root.IsValid())
	ASSERT_ALWAYS(root->Next() == 0)
	
	stob::Node* n = root->Children();
	ASSERT_ALWAYS(n)
	ASSERT_ALWAYS(n->Value() == "test string")
	ASSERT_ALWAYS(!n->Children())
	
	n = n->Next();
	ASSERT_ALWAYS(n)
	ASSERT_ALWAYS(n->Value() == "another string")
	ASSERT_ALWAYS(!n->Children())
	
	n = n->Next();
	ASSERT_ALWAYS(n)
	ASSERT_ALWAYS(n->Value() == "unquotedString")
	ASSERT_ALWAYS(!n->Children())
	
	n = n->Next();
	ASSERT_ALWAYS(n)
	ASSERT_ALWAYS(n->Value() == "string with empty children list")
	ASSERT_ALWAYS(!n->Children())
	
	n = n->Next();
	ASSERT_ALWAYS(n)
	ASSERT_ALWAYS(n->Value() == "unquoted_string_with_empty_children_list")
	ASSERT_ALWAYS(!n->Children())
	
	n = n->Next();
	ASSERT_ALWAYS(n)
	ASSERT_ALWAYS(n->Value() == "")
	{
		stob::Node * n1 = n->Children();
		ASSERT_ALWAYS(n1)
		ASSERT_ALWAYS(n1->Value() == "child string")
		
		n1 = n1->Next();
		ASSERT_ALWAYS(n1)
		ASSERT_ALWAYS(n1->Value() == "aaa")
		ASSERT_ALWAYS(!n1->Children())
		
		n1 = n1->Next();
		ASSERT_ALWAYS(n1)
		ASSERT_ALWAYS(n1->Value() == "bbb")
		ASSERT_ALWAYS(!n1->Children())
		
		n1 = n1->Next();
		ASSERT_ALWAYS(n1)
		ASSERT_ALWAYS(n1->Value() == "ccc")
		{
			stob::Node* n2 = n1->Children();
			ASSERT_ALWAYS(n2)
			ASSERT_ALWAYS(n2->Value() == "ddd")
			ASSERT_ALWAYS(!n2->Next())
		}
		
		ASSERT_ALWAYS(!n1->Next())
	}
	
	n = n->Next();
	ASSERT_ALWAYS(n)
	ASSERT_ALWAYS(n->Value() == "Tab\t Backslash\\ Slash/ Doubleslash// Newline\n Carriagereturn\r Doublequotes\" {}{}{}{}")
	ASSERT_ALWAYS(!n->Children())
	
	n = n->Next();
	ASSERT_ALWAYS(n)
	ASSERT_ALWAYS(n->Value() == "quoted string with trailing slash /")
	ASSERT_ALWAYS(!n->Children())
	
	n = n->Next();
	ASSERT_ALWAYS(n)
	ASSERT_ALWAYS(n->Value() == "fff ggg")
	ASSERT_ALWAYS(!n->Children())
	
	n = n->Next();
	ASSERT_ALWAYS(n)
	ASSERT_ALWAYS(n->Value() == "tralala tro lo lo\ntre lele")
	ASSERT_ALWAYS(!n->Children())
	
	n = n->Next();
	ASSERT_ALWAYS(n)
	ASSERT_ALWAYS(n->Value() == "-1213.33")
	ASSERT_ALWAYS(!n->Children())
	
	n = n->Next();
	ASSERT_ALWAYS(n)
	ASSERT_ALWAYS(n->Value() == "UnquotedStringAtTheVeryEndOfTheFile")
	ASSERT_ALWAYS(!n->Children())
	
	ASSERT_ALWAYS(!n->Next())
}

}




int main(int argc, char** argv){

	TestBasicParsing::Run();
	
	
	return 0;
}
