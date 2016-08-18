#include "../../src/stob/dom.hpp"

#include <utki/debug.hpp>
#include <papki/FSFile.hpp>



namespace TestBasicParsing{

void Run(){
	papki::FSFile fi("test.stob");
	
	auto root = utki::makeUnique<stob::Node>();
	root->setChildren(stob::load(fi));
	ASSERT_ALWAYS(root)
	ASSERT_ALWAYS(root->next() == 0)
	
	auto n = root->child();
	ASSERT_ALWAYS(n)
	ASSERT_ALWAYS(*n == "ttt")
	ASSERT_ALWAYS(!n->child())
	
	n = n->next();
	ASSERT_ALWAYS(n)
	ASSERT_INFO_ALWAYS(*n == "test string", n->value())
	ASSERT_ALWAYS(!n->child())
	
	n = n->next();
	ASSERT_ALWAYS(n)
	ASSERT_INFO_ALWAYS(*n == "anot/her string", n->value())
	ASSERT_ALWAYS(!n->child())
	
	n = n->next();
	ASSERT_ALWAYS(n)
	ASSERT_ALWAYS(*n == "unqu/otedString")
	ASSERT_ALWAYS(!n->child())
	
	n = n->next();
	ASSERT_ALWAYS(n)
	ASSERT_ALWAYS(*n == "one\ntwo three\tfour")
	ASSERT_ALWAYS(!n->child())
	
	n = n->next();
	ASSERT_ALWAYS(n)
	ASSERT_ALWAYS(*n == "string with empty children list")
	ASSERT_ALWAYS(!n->child())
	
	n = n->next();
	ASSERT_ALWAYS(n)
	ASSERT_ALWAYS(*n == "unquoted_string_with_empty_children_list")
	ASSERT_ALWAYS(!n->child())
	
	n = n->next();
	ASSERT_ALWAYS(n)
	ASSERT_INFO_ALWAYS(*n == "string_interrupted", n->value())
	ASSERT_ALWAYS(!n->child())
	
	n = n->next();
	ASSERT_ALWAYS(n)
	ASSERT_INFO_ALWAYS(*n == "_by_comment", n->value())
	ASSERT_ALWAYS(!n->child())
	
	n = n->next();
	ASSERT_ALWAYS(n)
	ASSERT_INFO_ALWAYS(*n == "string_broken", n->value())
	ASSERT_ALWAYS(!n->child())
	
	n = n->next();
	ASSERT_ALWAYS(n)
	ASSERT_INFO_ALWAYS(*n == "_by_comment", n->value())
	ASSERT_ALWAYS(!n->child())

	n = n->next();
	ASSERT_ALWAYS(n)
	ASSERT_ALWAYS(*n == "")
	ASSERT_ALWAYS(n->value() == 0)
	ASSERT_INFO_ALWAYS(!n->child(), n->child()->value())
	
	n = n->next();
	ASSERT_ALWAYS(n)
	ASSERT_ALWAYS(*n == "")
	ASSERT_ALWAYS(n->value() == 0)
	ASSERT_ALWAYS(n->child())
	{
		stob::Node * n1 = n->child();
		ASSERT_ALWAYS(n1)
		ASSERT_ALWAYS(*n1 == "")
		ASSERT_ALWAYS(n1->value() == 0)
		ASSERT_ALWAYS(!n1->child())
		
		n1 = n1->next();
		ASSERT_ALWAYS(n1)
		ASSERT_ALWAYS(*n1 == "")
		ASSERT_ALWAYS(n1->value() == 0)
		ASSERT_ALWAYS(!n1->child())
		
		ASSERT_ALWAYS(!n1->next())
	}
	
	n = n->next();
	ASSERT_ALWAYS(n)
	ASSERT_ALWAYS(*n == "")
	{
//		ASSERT_INFO_ALWAYS(n->count() == 5, n->count())
		
		ASSERT_ALWAYS(n->child(3).node())
		ASSERT_ALWAYS(n->child(3).prev() == n->child(2).node())
		ASSERT_ALWAYS(n->child(3).node()->operator==("ccc"))
		
		ASSERT_ALWAYS(n->child(size_t(0)).node())
		ASSERT_ALWAYS(n->child(size_t(0)).prev() == nullptr)
		ASSERT_ALWAYS(n->child(size_t(0)).node()->operator ==("child string"))
		
		ASSERT_ALWAYS(!n->child(5).node())
		ASSERT_ALWAYS(!n->child(54).node())
		
		stob::Node * n1 = n->child();
		ASSERT_ALWAYS(n1)
		ASSERT_ALWAYS(*n1 == "child string")
		
		n1 = n1->next();
		ASSERT_ALWAYS(n1)
		ASSERT_ALWAYS(*n1 == "aaa")
		ASSERT_ALWAYS(!n1->child())
		
		n1 = n1->next();
		ASSERT_ALWAYS(n1)
		ASSERT_ALWAYS(*n1 == "bbb")
		ASSERT_ALWAYS(!n1->child())
		
		n1 = n1->next();
		ASSERT_ALWAYS(n1)
		ASSERT_ALWAYS(*n1 == "ccc")
		{
			stob::Node* n2 = n1->child();
			ASSERT_ALWAYS(n2)
			ASSERT_ALWAYS(*n2 == "ddd")
			ASSERT_ALWAYS(!n2->next())
		}
		
		n1 = n1->next();
		ASSERT_ALWAYS(n1)
		ASSERT_ALWAYS(*n1 == "")
		ASSERT_ALWAYS(!n1->child())
		
		ASSERT_ALWAYS(!n1->next())
	}
	
	n = n->next();
	ASSERT_ALWAYS(n)
	ASSERT_ALWAYS(*n == "Tab\t Backslash\\ Slash/ Doubleslash// Newline\n Carriagereturn\r Doublequotes\" {}{}{}{}")
	ASSERT_ALWAYS(!n->child())
	
	n = n->next();
	ASSERT_ALWAYS(n)
	ASSERT_ALWAYS(*n == "quoted string with trailing slash /")
	ASSERT_ALWAYS(!n->child())
	
	n = n->next();
	ASSERT_ALWAYS(n)
	ASSERT_ALWAYS(*n == "fff ggg")
	ASSERT_ALWAYS(!n->child())
	
	n = n->next();
	ASSERT_ALWAYS(n)
	ASSERT_ALWAYS(*n == "tralala tro lo lo\ntre lele")
	ASSERT_ALWAYS(!n->child())
	
	n = n->next();
	ASSERT_ALWAYS(n)
	ASSERT_ALWAYS(*n == "-1213.33")
	ASSERT_ALWAYS(!n->child())
	
	n = n->next();
	ASSERT_ALWAYS(n)
	ASSERT_ALWAYS(*n == "UnquotedStringAtTheVeryEndOfTheFile")
	ASSERT_ALWAYS(!n->child())
	
	ASSERT_ALWAYS(!n->next())
	
	
	{
		stob::Node* ch = root->child("test string").node();
		ASSERT_ALWAYS(ch)
		ASSERT_ALWAYS(*ch == "test string")
	}
	{
		const stob::Node* constRoot = root.operator->();
		const stob::Node* ch = constRoot->child("unqu/otedString").node();
		ASSERT_ALWAYS(ch)
		ASSERT_ALWAYS(*ch == "unqu/otedString")
	}
	
	{
		auto cloned = root->cloneChain();
		ASSERT_ALWAYS(cloned)
		ASSERT_ALWAYS(cloned->count() != 0)
	}
}
}//~namespace



namespace TestWriting{
void Run(){
	auto root = utki::makeUnique<stob::Node>();
	
	{
		root->setChildren(utki::makeUnique<stob::Node>());
		stob::Node* n = root->child();
		ASSERT_ALWAYS(n)
		n->setValue("test string");
		
		n->insertNext(utki::makeUnique<stob::Node>());
		n = n->next();
		ASSERT_ALWAYS(n)
		n->setValue("Unquoted_String");
		
		n->insertNext(utki::makeUnique<stob::Node>());
		n = n->next();
		ASSERT_ALWAYS(n)
		n->setValue("Escapes: \n \r \t \\ \" {}");

		n->insertNext(utki::makeUnique<stob::Node>());
		n = n->next();
		ASSERT_ALWAYS(n)
		n->setValue("Quoted{String");
		{
			n->setChildren(utki::makeUnique<stob::Node>());
			stob::Node* n1 = n->child();
			ASSERT_ALWAYS(n1)
			//n1 has no value (empty string)
			
			n1->insertNext(utki::makeUnique<stob::Node>());
			n1 = n1->next();
			ASSERT_ALWAYS(n1)
			n1->setValue("Child2");
			
			n1->insertNext(utki::makeUnique<stob::Node>());
			n1 = n1->next();
			ASSERT_ALWAYS(n1)
			n1->setValue("Child_third");
			{
				n1->setChildren(utki::makeUnique<stob::Node>());
				stob::Node* n2 = n1->child();
				ASSERT_ALWAYS(n2)
				n2->setValue("only one child");
			}
			
			n1->insertNext(utki::makeUnique<stob::Node>());
			n1 = n1->next();
			ASSERT_ALWAYS(n1)
			n1->setValue("Child fourth");
			{
				n1->addProperty("Property")->setValue("value");
				
				stob::Node* n2 = n1->child();
				n2->insertNext(utki::makeUnique<stob::Node>());
				n2 = n2->next();
				ASSERT_ALWAYS(n2)
				n2->setValue("subchild1");
				n2->setChildren(utki::makeUnique<stob::Node>());
				
				n2->insertNext(utki::makeUnique<stob::Node>());
				n2 = n2->next();
				ASSERT_ALWAYS(n2)
				n2->setValue("subchild 2");
				
				n1->addProperty("Prop")->setBool("true");
			}
			
			n1->insertNext(utki::makeUnique<stob::Node>());
			n1 = n1->next();
			ASSERT_ALWAYS(n1)
			//n1->setValue("");
			{
				n1->setChildren(utki::makeUnique<stob::Node>());
				stob::Node* n2 = n1->child();
				ASSERT_ALWAYS(n2)
				n2->setValue("-3213.43");
				n2->insertNext(utki::makeUnique<stob::Node>("fsd"));
			}
			
			n1->insertNext(utki::makeUnique<stob::Node>());
			n1 = n1->next();
			ASSERT_ALWAYS(n1)
			n1->setInt32(315);
			
			n1->insertNext(utki::makeUnique<stob::Node>());
			n1 = n1->next();
			ASSERT_ALWAYS(n1)
			n1->setInt32(-426);
			n1->setChildren(utki::makeUnique<stob::Node>());
			n1->child()->setChildren(utki::makeUnique<stob::Node>("trololo"));
			
			n1->insertNext(utki::makeUnique<stob::Node>());
			n1 = n1->next();
			ASSERT_ALWAYS(n1)
			n1->setUint32(4000100315u);
			
			n1->insertNext(utki::makeUnique<stob::Node>());
			n1 = n1->next();
			ASSERT_ALWAYS(n1)
			n1->setUint32(-426);
			
			n1->insertNext(utki::makeUnique<stob::Node>());
			n1 = n1->next();
			ASSERT_ALWAYS(n1)
			n1->setUint64(1234567890123LL);
			
			n1->insertNext(utki::makeUnique<stob::Node>());
			n1 = n1->next();
			ASSERT_ALWAYS(n1)
			n1->setInt64(-1234567890123LL);
			
			n1->insertNext(utki::makeUnique<stob::Node>());
			n1 = n1->next();
			ASSERT_ALWAYS(n1)
			n1->setChildren(utki::makeUnique<stob::Node>());
			n1->child()->setChildren(utki::makeUnique<stob::Node>("trololo"));
			
			n1->insertNext(utki::makeUnique<stob::Node>());
			n1 = n1->next();
			ASSERT_ALWAYS(n1)
			n1->setFloat(315.34f);
			
			n1->insertNext(utki::makeUnique<stob::Node>());
			n1 = n1->next();
			ASSERT_ALWAYS(n1)
			n1->setFloat(0.00006f);
			
			n1->insertNext(utki::makeUnique<stob::Node>());
			n1 = n1->next();
			ASSERT_ALWAYS(n1)
			n1->setDouble(-315.3);
			
			n1->insertNext(utki::makeUnique<stob::Node>());
			n1 = n1->next();
			ASSERT_ALWAYS(n1)
			n1->setDouble(-31523355325.3);
			
			n1->insertNext(utki::makeUnique<stob::Node>());
			n1 = n1->next();
			ASSERT_ALWAYS(n1)
			n1->setLongDouble(-315.33L);
			
			n1->insertNext(utki::makeUnique<stob::Node>());
			n1 = n1->next();
			ASSERT_ALWAYS(n1)
			n1->setBool(true);
			
			n1->insertNext(utki::makeUnique<stob::Node>());
			n1 = n1->next();
			ASSERT_ALWAYS(n1)
			n1->setBool(false);
		}
		
		n->insertNext(utki::makeUnique<stob::Node>());
		n = n->next();
		ASSERT_ALWAYS(n)
		n->setValue("Another}QuotedString");

		n->insertNext(utki::makeUnique<stob::Node>());
		n = n->next();
		ASSERT_ALWAYS(n)
		n->setValue("Last{String}InTheFile");
	}
	
	papki::FSFile fileFormatted("out_formatted.stob");
	papki::FSFile fileNotFormatted("out_not_formatted.stob");
	
	root->child()->writeChain(fileFormatted, true);
	root->child()->writeChain(fileNotFormatted, false);
	
	std::unique_ptr<stob::Node> readFormatted = utki::makeUnique<stob::Node>();
	readFormatted->setChildren(stob::load(fileFormatted));
	ASSERT_ALWAYS(root->operator==(*readFormatted))
	
//	TRACE(<< "formatted read" << std::endl)
	
	std::unique_ptr<stob::Node> readNotFormatted = utki::makeUnique<stob::Node>();
	readNotFormatted->setChildren(stob::load(fileNotFormatted));
	ASSERT_ALWAYS(root->operator==(*readNotFormatted))
	
//	TRACE(<< "not formatted read" << std::endl)
}
}//~namespace



int main(int argc, char** argv){

	TestBasicParsing::Run();
	TestWriting::Run();
	
	return 0;
}
