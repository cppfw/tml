#include "../../src/puu/dom.hpp"

#include <utki/debug.hpp>
#include <papki/FSFile.hpp>



namespace TestBasicParsing{

void Run(){
	papki::FSFile fi("test.puu");

	auto root = utki::makeUnique<puu::node>();
	root->set_children(puu::load(fi));
	ASSERT_ALWAYS(root)
	ASSERT_ALWAYS(root->next() == 0)

	auto n = root->child();
	ASSERT_ALWAYS(n)
	ASSERT_ALWAYS(*n == "ttt")
	ASSERT_ALWAYS(!n->child())

	n = n->next();
	ASSERT_ALWAYS(n)
	ASSERT_INFO_ALWAYS(*n == "test string", n->get_value())
	ASSERT_ALWAYS(!n->child())

	n = n->next();
	ASSERT_ALWAYS(n)
	ASSERT_INFO_ALWAYS(*n == "anot/her string", n->get_value())
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
	ASSERT_INFO_ALWAYS(*n == "string_interrupted", n->get_value())
	ASSERT_ALWAYS(!n->child())

	n = n->next();
	ASSERT_ALWAYS(n)
	ASSERT_INFO_ALWAYS(*n == "_by_comment", n->get_value())
	ASSERT_ALWAYS(!n->child())

	n = n->next();
	ASSERT_ALWAYS(n)
	ASSERT_INFO_ALWAYS(*n == "string_broken", n->get_value())
	ASSERT_ALWAYS(!n->child())

	n = n->next();
	ASSERT_ALWAYS(n)
	ASSERT_INFO_ALWAYS(*n == "_by_comment", n->get_value())
	ASSERT_ALWAYS(!n->child())

	n = n->next();
	ASSERT_ALWAYS(n)
	ASSERT_ALWAYS(*n == "")
	ASSERT_ALWAYS(n->get_value() == 0)
	ASSERT_INFO_ALWAYS(!n->child(), n->child()->get_value())

	n = n->next();
	ASSERT_ALWAYS(n)
	ASSERT_ALWAYS(*n == "")
	ASSERT_ALWAYS(n->get_value() == 0)
	ASSERT_ALWAYS(n->child())
	{
		puu::node * n1 = n->child();
		ASSERT_ALWAYS(n1)
		ASSERT_ALWAYS(*n1 == "")
		ASSERT_ALWAYS(n1->get_value() == 0)
		ASSERT_ALWAYS(!n1->child())

		n1 = n1->next();
		ASSERT_ALWAYS(n1)
		ASSERT_ALWAYS(*n1 == "")
		ASSERT_ALWAYS(n1->get_value() == 0)
		ASSERT_ALWAYS(!n1->child())

		ASSERT_ALWAYS(!n1->next())
	}

	n = n->next();
	ASSERT_ALWAYS(n)
	ASSERT_ALWAYS(*n == "")
	{
//		ASSERT_INFO_ALWAYS(n->count() == 5, n->count())

		ASSERT_ALWAYS(n->child(3).get_node())
		ASSERT_ALWAYS(n->child(3).prev() == n->child(2).get_node())
		ASSERT_ALWAYS(n->child(3).get_node()->operator==("ccc"))

		ASSERT_ALWAYS(n->child(size_t(0)).get_node())
		ASSERT_ALWAYS(n->child(size_t(0)).prev() == nullptr)
		ASSERT_ALWAYS(n->child(size_t(0)).get_node()->operator ==("child string"))

		ASSERT_ALWAYS(!n->child(5).get_node())
		ASSERT_ALWAYS(!n->child(54).get_node())

		puu::node * n1 = n->child();
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
			puu::node* n2 = n1->child();
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
		puu::node* ch = root->child("test string").get_node();
		ASSERT_ALWAYS(ch)
		ASSERT_ALWAYS(*ch == "test string")
	}
	{
		const puu::node* constRoot = root.operator->();
		const puu::node* ch = constRoot->child("unqu/otedString").get_node();
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
	auto root = utki::makeUnique<puu::node>();

	{
		root->set_children(utki::makeUnique<puu::node>());
		puu::node* n = root->child();
		ASSERT_ALWAYS(n)
		n->set_value("test string");

		n->insertNext(utki::makeUnique<puu::node>());
		n = n->next();
		ASSERT_ALWAYS(n)
		n->set_value("Unquoted_String");

		n->insertNext(utki::makeUnique<puu::node>());
		n = n->next();
		ASSERT_ALWAYS(n)
		n->set_value("Escapes: \n \r \t \\ \" {}");

		n->insertNext(utki::makeUnique<puu::node>());
		n = n->next();
		ASSERT_ALWAYS(n)
		n->set_value("Quoted{String");
		{
			n->set_children(utki::makeUnique<puu::node>());
			puu::node* n1 = n->child();
			ASSERT_ALWAYS(n1)
			//n1 has no value (empty string)

			n1->insertNext(utki::makeUnique<puu::node>());
			n1 = n1->next();
			ASSERT_ALWAYS(n1)
			n1->set_value("Child2");

			n1->insertNext(utki::makeUnique<puu::node>());
			n1 = n1->next();
			ASSERT_ALWAYS(n1)
			n1->set_value("Child_third");
			{
				n1->set_children(utki::makeUnique<puu::node>());
				puu::node* n2 = n1->child();
				ASSERT_ALWAYS(n2)
				n2->set_value("only one child");
			}

			n1->insertNext(utki::makeUnique<puu::node>());
			n1 = n1->next();
			ASSERT_ALWAYS(n1)
			n1->set_value("Child fourth");
			{
				n1->addProperty("Property")->set_value("value");

				puu::node* n2 = n1->child();
				n2->insertNext(utki::makeUnique<puu::node>());
				n2 = n2->next();
				ASSERT_ALWAYS(n2)
				n2->set_value("subchild1");
				n2->set_children(utki::makeUnique<puu::node>());

				n2->insertNext(utki::makeUnique<puu::node>());
				n2 = n2->next();
				ASSERT_ALWAYS(n2)
				n2->set_value("subchild 2");

				n1->addProperty("Prop")->set_bool("true");
			}

			n1->insertNext(utki::makeUnique<puu::node>());
			n1 = n1->next();
			ASSERT_ALWAYS(n1)
			//n1->set_value("");
			{
				n1->set_children(utki::makeUnique<puu::node>());
				puu::node* n2 = n1->child();
				ASSERT_ALWAYS(n2)
				n2->set_value("-3213.43");
				n2->insertNext(utki::makeUnique<puu::node>("fsd"));
			}

			n1->insertNext(utki::makeUnique<puu::node>());
			n1 = n1->next();
			ASSERT_ALWAYS(n1)
			n1->set_int32(315);

			n1->insertNext(utki::makeUnique<puu::node>());
			n1 = n1->next();
			ASSERT_ALWAYS(n1)
			n1->set_int32(-426);
			n1->set_children(utki::makeUnique<puu::node>());
			n1->child()->set_children(utki::makeUnique<puu::node>("trololo"));

			n1->insertNext(utki::makeUnique<puu::node>());
			n1 = n1->next();
			ASSERT_ALWAYS(n1)
			n1->setUint32(4000100315u);

			n1->insertNext(utki::makeUnique<puu::node>());
			n1 = n1->next();
			ASSERT_ALWAYS(n1)
			n1->setUint32(-426);

			n1->insertNext(utki::makeUnique<puu::node>());
			n1 = n1->next();
			ASSERT_ALWAYS(n1)
			n1->set_uint64(1234567890123LL);

			n1->insertNext(utki::makeUnique<puu::node>());
			n1 = n1->next();
			ASSERT_ALWAYS(n1)
			n1->set_int64(-1234567890123LL);

			n1->insertNext(utki::makeUnique<puu::node>());
			n1 = n1->next();
			ASSERT_ALWAYS(n1)
			n1->set_children(utki::makeUnique<puu::node>());
			n1->child()->set_children(utki::makeUnique<puu::node>("trololo"));

			n1->insertNext(utki::makeUnique<puu::node>());
			n1 = n1->next();
			ASSERT_ALWAYS(n1)
			n1->set_float(315.34f);

			n1->insertNext(utki::makeUnique<puu::node>());
			n1 = n1->next();
			ASSERT_ALWAYS(n1)
			n1->set_float(0.00006f);

			n1->insertNext(utki::makeUnique<puu::node>());
			n1 = n1->next();
			ASSERT_ALWAYS(n1)
			n1->set_double(-315.3);

			n1->insertNext(utki::makeUnique<puu::node>());
			n1 = n1->next();
			ASSERT_ALWAYS(n1)
			n1->set_double(-31523355325.3);

			n1->insertNext(utki::makeUnique<puu::node>());
			n1 = n1->next();
			ASSERT_ALWAYS(n1)
			n1->set_long_double(-315.33L);

			n1->insertNext(utki::makeUnique<puu::node>());
			n1 = n1->next();
			ASSERT_ALWAYS(n1)
			n1->set_bool(true);

			n1->insertNext(utki::makeUnique<puu::node>());
			n1 = n1->next();
			ASSERT_ALWAYS(n1)
			n1->set_bool(false);
		}

		n->insertNext(utki::makeUnique<puu::node>());
		n = n->next();
		ASSERT_ALWAYS(n)
		n->set_value("Another}QuotedString");

		n->insertNext(utki::makeUnique<puu::node>());
		n = n->next();
		ASSERT_ALWAYS(n)
		n->set_value("Last{String}InTheFile");
	}

	papki::FSFile fileFormatted("out_formatted.puu");
	papki::FSFile fileNotFormatted("out_not_formatted.puu");

	root->child()->writeChain(fileFormatted, true);
	root->child()->writeChain(fileNotFormatted, false);

	std::unique_ptr<puu::node> readFormatted = utki::makeUnique<puu::node>();
	readFormatted->set_children(puu::load(fileFormatted));
	ASSERT_ALWAYS(root->operator==(*readFormatted))

//	TRACE(<< "formatted read" << std::endl)

	std::unique_ptr<puu::node> readNotFormatted = utki::makeUnique<puu::node>();
	readNotFormatted->set_children(puu::load(fileNotFormatted));
	ASSERT_ALWAYS(root->operator==(*readNotFormatted))

//	TRACE(<< "not formatted read" << std::endl)
}
}//~namespace


namespace TestnodeManipulation{
void run(){
	//test cloneChildren with no children
	{
		auto n = utki::makeUnique<puu::node>("test");
		ASSERT_ALWAYS(n)
		ASSERT_ALWAYS(!n->child())
		ASSERT_ALWAYS(*n == "test")

		ASSERT_ALWAYS(!n->cloneChildren())
	}

	//test cloneChildren with many children
	{
		auto n = puu::parse(R"qwertyuiop(
				a{
					b{
						c
					}
					b1{
						c1{
							d1
						}
					}
					b2{
						c2{
							d2
						}
					}
				}
			)qwertyuiop");

		ASSERT_ALWAYS(n)
		ASSERT_ALWAYS(n->child())
		ASSERT_ALWAYS(n->count() == 3)

		auto c = n->cloneChildren();
		ASSERT_ALWAYS(c)
		ASSERT_ALWAYS(c->count_chain() == 3)

		ASSERT_ALWAYS(c->count() == 1)
		ASSERT_ALWAYS(*c == "b")
		ASSERT_ALWAYS(*c->child() == "c")
		auto cn = c->next();
		ASSERT_ALWAYS(cn)
		ASSERT_ALWAYS(*cn == "b1")
		ASSERT_ALWAYS(cn->count() == 1)
		ASSERT_ALWAYS(*cn->child() == "c1")
		ASSERT_ALWAYS(cn->child()->count() == 1)
		ASSERT_ALWAYS(*cn->child()->child() == "d1")
		cn = cn->next();
		ASSERT_ALWAYS(cn)
		ASSERT_ALWAYS(*cn == "b2")
		ASSERT_ALWAYS(cn->count() == 1)
		ASSERT_ALWAYS(*cn->child() == "c2")
		ASSERT_ALWAYS(cn->child()->count() == 1)
		ASSERT_ALWAYS(*cn->child()->child() == "d2")

		ASSERT_ALWAYS(!cn->next())
	}

	//test replace(std::unique_ptr<node>)
	{
		auto chain = puu::parse(R"qwertyuiop(
				b{
					c
				}
				b1{
					c1{
						d1
					}
				}
				b2{
					c2{
						d2
					}
				}
			)qwertyuiop");

		auto r = puu::parse(R"qwertyuiop(
				a{
					b{
						c
					}
					b1{
						c1{
							d1
						}
					}
					b2{
						c2{
							d2
						}
					}
				}
			)qwertyuiop");

		r->child()->next()->replace(std::move(chain));

		auto n = r->child();
		ASSERT_ALWAYS(n)
		ASSERT_ALWAYS(*n == "b")
		ASSERT_ALWAYS(*n->child() == "c")
		ASSERT_ALWAYS(!n->child()->child())
		ASSERT_ALWAYS(!n->child()->next())
		n = n->next();
		ASSERT_ALWAYS(n)
		ASSERT_ALWAYS(*n == "b")
		ASSERT_ALWAYS(*n->child() == "c")
		ASSERT_ALWAYS(!n->child()->child())
		ASSERT_ALWAYS(!n->child()->next())
		n = n->next();
		ASSERT_ALWAYS(n)
		ASSERT_ALWAYS(*n == "b1")
		ASSERT_ALWAYS(*n->child() == "c1")
		ASSERT_ALWAYS(!n->child()->next())
		ASSERT_ALWAYS(*n->child()->child() == "d1")
		ASSERT_ALWAYS(!n->child()->child()->child())
		ASSERT_ALWAYS(!n->child()->next())
		n = n->next();
		ASSERT_ALWAYS(n)
		ASSERT_ALWAYS(*n == "b2")
		ASSERT_ALWAYS(*n->child() == "c2")
		ASSERT_ALWAYS(!n->child()->next())
		ASSERT_ALWAYS(*n->child()->child() == "d2")
		ASSERT_ALWAYS(!n->child()->child()->child())
		ASSERT_ALWAYS(!n->child()->next())
		n = n->next();
		ASSERT_ALWAYS(n)
		ASSERT_ALWAYS(*n == "b2")
		ASSERT_ALWAYS(*n->child() == "c2")
		ASSERT_ALWAYS(!n->child()->next())
		ASSERT_ALWAYS(*n->child()->child() == "d2")
		ASSERT_ALWAYS(!n->child()->child()->child())
		ASSERT_ALWAYS(!n->child()->next())
		ASSERT_ALWAYS(!n->next())
	}

	//test replace(node&)
	{
		auto chain = puu::parse(R"qwertyuiop(
				b{
					c
				}
				b1{
					c1{
						d1
					}
				}
				b2{
					c2{
						d2
					}
				}
			)qwertyuiop");

		auto r = puu::parse(R"qwertyuiop(
				a{
					b{
						c
					}
					b1{
						c1{
							d1
						}
					}
					b2{
						c2{
							d2
						}
					}
				}
			)qwertyuiop");

		r->child()->next()->replace(*chain);
		ASSERT_ALWAYS(chain)

		auto n = r->child();
		ASSERT_ALWAYS(n)
		ASSERT_ALWAYS(*n == "b")
		ASSERT_ALWAYS(*n->child() == "c")
		ASSERT_ALWAYS(!n->child()->child())
		ASSERT_ALWAYS(!n->child()->next())
		n = n->next();
		ASSERT_ALWAYS(n)
		ASSERT_ALWAYS(*n == "b")
		ASSERT_ALWAYS(*n->child() == "c")
		ASSERT_ALWAYS(!n->child()->child())
		ASSERT_ALWAYS(!n->child()->next())
		n = n->next();
		ASSERT_ALWAYS(n)
		ASSERT_ALWAYS(*n == "b1")
		ASSERT_ALWAYS(*n->child() == "c1")
		ASSERT_ALWAYS(!n->child()->next())
		ASSERT_ALWAYS(*n->child()->child() == "d1")
		ASSERT_ALWAYS(!n->child()->child()->child())
		ASSERT_ALWAYS(!n->child()->next())
		n = n->next();
		ASSERT_ALWAYS(n)
		ASSERT_ALWAYS(*n == "b2")
		ASSERT_ALWAYS(*n->child() == "c2")
		ASSERT_ALWAYS(!n->child()->next())
		ASSERT_ALWAYS(*n->child()->child() == "d2")
		ASSERT_ALWAYS(!n->child()->child()->child())
		ASSERT_ALWAYS(!n->child()->next())
		n = n->next();
		ASSERT_ALWAYS(n)
		ASSERT_ALWAYS(*n == "b2")
		ASSERT_ALWAYS(*n->child() == "c2")
		ASSERT_ALWAYS(!n->child()->next())
		ASSERT_ALWAYS(*n->child()->child() == "d2")
		ASSERT_ALWAYS(!n->child()->child()->child())
		ASSERT_ALWAYS(!n->child()->next())
		ASSERT_ALWAYS(!n->next())
	}
}
}


int main(int argc, char** argv){

	TestBasicParsing::Run();
	TestWriting::Run();
	TestnodeManipulation::run();

	return 0;
}
