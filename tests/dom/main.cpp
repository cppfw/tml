#include "../../src/stob/dom.hpp"

#include <ting/debug.hpp>
#include <ting/fs/FSFile.hpp>



namespace TestBasicParsing{

void Run(){
	ting::fs::FSFile fi("test.stob");
	
	std::unique_ptr<stob::Node> root = stob::Node::New();
	root->SetChildren(stob::Load(fi));
	ASSERT_ALWAYS(root)
	ASSERT_ALWAYS(root->Next() == 0)
	
	stob::Node* n = root->Child();
	ASSERT_ALWAYS(n)
	ASSERT_ALWAYS(*n == "ttt")
	ASSERT_ALWAYS(!n->Child())
	
	n = n->Next();
	ASSERT_ALWAYS(n)
	ASSERT_INFO_ALWAYS(*n == "test string", n->Value())
	ASSERT_ALWAYS(!n->Child())
	
	n = n->Next();
	ASSERT_ALWAYS(n)
	ASSERT_INFO_ALWAYS(*n == "anot/her string", n->Value())
	ASSERT_ALWAYS(!n->Child())
	
	n = n->Next();
	ASSERT_ALWAYS(n)
	ASSERT_ALWAYS(*n == "unqu/otedString")
	ASSERT_ALWAYS(!n->Child())
	
	n = n->Next();
	ASSERT_ALWAYS(n)
	ASSERT_ALWAYS(*n == "string with empty children list")
	ASSERT_ALWAYS(!n->Child())
	
	n = n->Next();
	ASSERT_ALWAYS(n)
	ASSERT_ALWAYS(*n == "unquoted_string_with_empty_children_list")
	ASSERT_ALWAYS(!n->Child())
	
	n = n->Next();
	ASSERT_ALWAYS(n)
	ASSERT_INFO_ALWAYS(*n == "string_interrupted_by_comment", n->Value())
	ASSERT_ALWAYS(!n->Child())
	
	n = n->Next();
	ASSERT_ALWAYS(n)
	ASSERT_INFO_ALWAYS(*n == "string_broken_by_comment", n->Value())
	ASSERT_ALWAYS(!n->Child())

	n = n->Next();
	ASSERT_ALWAYS(n)
	ASSERT_ALWAYS(*n == "")
	ASSERT_ALWAYS(n->Value() == 0)
	ASSERT_INFO_ALWAYS(!n->Child(), n->Child()->Value())
	
	n = n->Next();
	ASSERT_ALWAYS(n)
	ASSERT_ALWAYS(*n == "")
	ASSERT_ALWAYS(n->Value() == 0)
	ASSERT_ALWAYS(n->Child())
	{
		stob::Node * n1 = n->Child();
		ASSERT_ALWAYS(n1)
		ASSERT_ALWAYS(*n1 == "")
		ASSERT_ALWAYS(n1->Value() == 0)
		ASSERT_ALWAYS(!n1->Child())
		
		n1 = n1->Next();
		ASSERT_ALWAYS(n1)
		ASSERT_ALWAYS(*n1 == "")
		ASSERT_ALWAYS(n1->Value() == 0)
		ASSERT_ALWAYS(!n1->Child())
		
		ASSERT_ALWAYS(!n1->Next())
	}
	
	n = n->Next();
	ASSERT_ALWAYS(n)
	ASSERT_ALWAYS(*n == "")
	{
		ASSERT_INFO_ALWAYS(n->count() == 5, n->count())
		
		ASSERT_ALWAYS(n->child(3))
		ASSERT_ALWAYS(n->child(3)->operator==("ccc"))
		
		ASSERT_ALWAYS(n->child(0))
		ASSERT_ALWAYS(n->child(0)->operator ==("child string"))
		
		ASSERT_ALWAYS(!n->child(5))
		ASSERT_ALWAYS(!n->child(54))
		
		stob::Node * n1 = n->Child();
		ASSERT_ALWAYS(n1)
		ASSERT_ALWAYS(*n1 == "child string")
		
		n1 = n1->Next();
		ASSERT_ALWAYS(n1)
		ASSERT_ALWAYS(*n1 == "aaa")
		ASSERT_ALWAYS(!n1->Child())
		
		n1 = n1->Next();
		ASSERT_ALWAYS(n1)
		ASSERT_ALWAYS(*n1 == "bbb")
		ASSERT_ALWAYS(!n1->Child())
		
		n1 = n1->Next();
		ASSERT_ALWAYS(n1)
		ASSERT_ALWAYS(*n1 == "ccc")
		{
			stob::Node* n2 = n1->Child();
			ASSERT_ALWAYS(n2)
			ASSERT_ALWAYS(*n2 == "ddd")
			ASSERT_ALWAYS(!n2->Next())
		}
		
		n1 = n1->Next();
		ASSERT_ALWAYS(n1)
		ASSERT_ALWAYS(*n1 == "")
		ASSERT_ALWAYS(!n1->Child())
		
		ASSERT_ALWAYS(!n1->Next())
	}
	
	n = n->Next();
	ASSERT_ALWAYS(n)
	ASSERT_ALWAYS(*n == "Tab\t Backslash\\ Slash/ Doubleslash// Newline\n Carriagereturn\r Doublequotes\" {}{}{}{}")
	ASSERT_ALWAYS(!n->Child())
	
	n = n->Next();
	ASSERT_ALWAYS(n)
	ASSERT_ALWAYS(*n == "quoted string with trailing slash /")
	ASSERT_ALWAYS(!n->Child())
	
	n = n->Next();
	ASSERT_ALWAYS(n)
	ASSERT_ALWAYS(*n == "fff ggg")
	ASSERT_ALWAYS(!n->Child())
	
	n = n->Next();
	ASSERT_ALWAYS(n)
	ASSERT_ALWAYS(*n == "tralala tro lo lo\ntre lele")
	ASSERT_ALWAYS(!n->Child())
	
	n = n->Next();
	ASSERT_ALWAYS(n)
	ASSERT_ALWAYS(*n == "-1213.33")
	ASSERT_ALWAYS(!n->Child())
	
	n = n->Next();
	ASSERT_ALWAYS(n)
	ASSERT_ALWAYS(*n == "UnquotedStringAtTheVeryEndOfTheFile")
	ASSERT_ALWAYS(!n->Child())
	
	ASSERT_ALWAYS(!n->Next())
	
	
	{
		stob::Node* ch = root->Child("test string").node();
		ASSERT_ALWAYS(ch)
		ASSERT_ALWAYS(*ch == "test string")
	}
	{
		const stob::Node* constRoot = root.operator->();
		const stob::Node* ch = constRoot->Child("unqu/otedString").node();
		ASSERT_ALWAYS(ch)
		ASSERT_ALWAYS(*ch == "unqu/otedString")
	}
}
}//~namespace



namespace TestWriting{
void Run(){
	std::unique_ptr<stob::Node> root = stob::Node::New();
	
	{
		root->SetChildren(stob::Node::New());
		stob::Node* n = root->Child();
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
			stob::Node* n1 = n->Child();
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
				stob::Node* n2 = n1->Child();
				ASSERT_ALWAYS(n2)
				n2->SetValue("only one child");
			}
			
			n1->InsertNext(stob::Node::New());
			n1 = n1->Next();
			ASSERT_ALWAYS(n1)
			n1->SetValue("Child fourth");
			{
				n1->AddProperty("Property")->SetValue("value");
				
				stob::Node* n2 = n1->Child();
				n2->InsertNext(stob::Node::New());
				n2 = n2->Next();
				ASSERT_ALWAYS(n2)
				n2->SetValue("subchild1");
				n2->SetChildren(stob::Node::New());
				
				n2->InsertNext(stob::Node::New());
				n2 = n2->Next();
				ASSERT_ALWAYS(n2)
				n2->SetValue("subchild 2");
				
				n1->AddProperty("Prop")->SetBool("true");
			}
			
			n1->InsertNext(stob::Node::New());
			n1 = n1->Next();
			ASSERT_ALWAYS(n1)
			//n1->SetValue("");
			{
				n1->SetChildren(stob::Node::New());
				stob::Node* n2 = n1->Child();
				ASSERT_ALWAYS(n2)
				n2->SetValue("-3213.43");
				n2->InsertNext(stob::Node::New("fsd"));
			}
			
			n1->InsertNext(stob::Node::New());
			n1 = n1->Next();
			ASSERT_ALWAYS(n1)
			n1->SetS32(315);
			
			n1->InsertNext(stob::Node::New());
			n1 = n1->Next();
			ASSERT_ALWAYS(n1)
			n1->SetS32(-426);
			n1->SetChildren(stob::Node::New());
			n1->Child()->SetChildren(stob::Node::New("trololo"));
			
			n1->InsertNext(stob::Node::New());
			n1 = n1->Next();
			ASSERT_ALWAYS(n1)
			n1->SetU32(4000100315u);
			
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
			n1->SetChildren(stob::Node::New());
			n1->Child()->SetChildren(stob::Node::New("trololo"));
			
			n1->InsertNext(stob::Node::New());
			n1 = n1->Next();
			ASSERT_ALWAYS(n1)
			n1->SetFloat(315.34f);
			
			n1->InsertNext(stob::Node::New());
			n1 = n1->Next();
			ASSERT_ALWAYS(n1)
			n1->SetFloat(0.00006f);
			
			n1->InsertNext(stob::Node::New());
			n1 = n1->Next();
			ASSERT_ALWAYS(n1)
			n1->SetDouble(-315.3);
			
			n1->InsertNext(stob::Node::New());
			n1 = n1->Next();
			ASSERT_ALWAYS(n1)
			n1->SetDouble(-31523355325.3);
			
			n1->InsertNext(stob::Node::New());
			n1 = n1->Next();
			ASSERT_ALWAYS(n1)
			n1->SetLongDouble(-315.33L);
			
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
	
	ting::fs::FSFile fileFormatted("out_formatted.stob");
	ting::fs::FSFile fileNotFormatted("out_not_formatted.stob");
	
	root->Child()->WriteChain(fileFormatted, true);
	root->Child()->WriteChain(fileNotFormatted, false);
	
	std::unique_ptr<stob::Node> readFormatted = stob::Node::New();
	readFormatted->SetChildren(stob::Load(fileFormatted));
	ASSERT_ALWAYS(root->operator==(*readFormatted))
	
//	TRACE(<< "formatted read" << std::endl)
	
	std::unique_ptr<stob::Node> readNotFormatted = stob::Node::New();
	readNotFormatted->SetChildren(stob::Load(fileNotFormatted));
	ASSERT_ALWAYS(root->operator==(*readNotFormatted))
	
//	TRACE(<< "not formatted read" << std::endl)
}
}//~namespace



int main(int argc, char** argv){

	TestBasicParsing::Run();
	TestWriting::Run();
	
	return 0;
}
