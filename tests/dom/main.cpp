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
}//~namespace



namespace TestWriting{
void Run(){
	ting::Ptr<stob::Node> root = stob::Node::New();
	
	{
		root->SetChildren(stob::Node::New());
		stob::Node* n = root->Children();
		ASSERT_ALWAYS(n)
		n->SetValue("test string");
		
		n->InsertNext(stob::Node::New());
		n = n->Next();
		ASSERT_ALWAYS(n)
		n->SetValue("Unquoted_String");
		
		n->InsertNext(stob::Node::New());
		n = n->Next();
		ASSERT_ALWAYS(n)
		n->SetValue("Escapes: \n \r \t \\ \" {}");

		n->InsertNext(stob::Node::New());
		n = n->Next();
		ASSERT_ALWAYS(n)
		n->SetValue("Quoted{String");
		{
			n->SetChildren(stob::Node::New());
			stob::Node* n1 = n->Children();
			ASSERT_ALWAYS(n1)
			//n1 has no value (empty string)
			
			n1->InsertNext(stob::Node::New());
			n1 = n1->Next();
			ASSERT_ALWAYS(n1)
			n1->SetValue("Child2");
			
			n1->InsertNext(stob::Node::New());
			n1 = n1->Next();
			ASSERT_ALWAYS(n1)
			n1->SetValue("Child_third");
			{
				n1->SetChildren(stob::Node::New());
				stob::Node* n2 = n1->Children();
				ASSERT_ALWAYS(n2)
				n2->SetValue("only one child");
			}
			
			n1->InsertNext(stob::Node::New());
			n1 = n1->Next();
			ASSERT_ALWAYS(n1)
			n1->SetValue("Child fourth");
			{
				n1->SetChildren(stob::Node::New());
				stob::Node* n2 = n1->Children();
				ASSERT_ALWAYS(n2)
				n2->SetValue("subchild1");
				
				n2->InsertNext(stob::Node::New());
				n2 = n2->Next();
				ASSERT_ALWAYS(n2)
				n2->SetValue("subchild 2");
			}
			
			n1->InsertNext(stob::Node::New());
			n1 = n1->Next();
			ASSERT_ALWAYS(n1)
			n1->SetValue("");
			{
				n1->SetChildren(stob::Node::New());
				stob::Node* n2 = n1->Children();
				ASSERT_ALWAYS(n2)
				n2->SetValue("-3213.43");
			}
			
			n1->InsertNext(stob::Node::New());
			n1 = n1->Next();
			ASSERT_ALWAYS(n1)
			n1->SetS32(315);
			
			n1->InsertNext(stob::Node::New());
			n1 = n1->Next();
			ASSERT_ALWAYS(n1)
			n1->SetS32(-426);
			
			n1->InsertNext(stob::Node::New());
			n1 = n1->Next();
			ASSERT_ALWAYS(n1)
			n1->SetU32(4000100315);
			
			n1->InsertNext(stob::Node::New());
			n1 = n1->Next();
			ASSERT_ALWAYS(n1)
			n1->SetU32(-426);
			
			n1->InsertNext(stob::Node::New());
			n1 = n1->Next();
			ASSERT_ALWAYS(n1)
			n1->SetU64(1234567890123LL);
			
			n1->InsertNext(stob::Node::New());
			n1 = n1->Next();
			ASSERT_ALWAYS(n1)
			n1->SetS64(-1234567890123LL);
			
			n1->InsertNext(stob::Node::New());
			n1 = n1->Next();
			ASSERT_ALWAYS(n1)
			n1->SetFloat(315.34f);
			
			n1->InsertNext(stob::Node::New());
			n1 = n1->Next();
			ASSERT_ALWAYS(n1)
			n1->SetDouble(-315.3);
			
			n1->InsertNext(stob::Node::New());
			n1 = n1->Next();
			ASSERT_ALWAYS(n1)
			n1->SetBool(true);
			
			n1->InsertNext(stob::Node::New());
			n1 = n1->Next();
			ASSERT_ALWAYS(n1)
			n1->SetBool(false);
		}
		
		n->InsertNext(stob::Node::New());
		n = n->Next();
		ASSERT_ALWAYS(n)
		n->SetValue("Another}QuotedString");

		n->InsertNext(stob::Node::New());
		n = n->Next();
		ASSERT_ALWAYS(n)
		n->SetValue("Last{String}InTheFile");
	}
	
	ting::fs::FSFile file("out.stob");
	
	root->Write(file, true);
}
}//~namespace



int main(int argc, char** argv){

	TestBasicParsing::Run();
	TestWriting::Run();
	
	return 0;
}
