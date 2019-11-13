#include "../../src/puu/tree.hpp"

#include <utki/debug.hpp>
#include <papki/FSFile.hpp>



void test_puu_basic_parsing(){
	papki::FSFile fi("test.puu");

	auto roots = puu::read(fi);

	auto i = roots.begin();

	ASSERT_ALWAYS(i->get() == "ttt");
	ASSERT_ALWAYS(i->size() == 0);

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_INFO_ALWAYS(i->get() == "test string", i->get())
	ASSERT_ALWAYS(i->size() == 0)

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_INFO_ALWAYS(i->get() == "anot/her string", i->get())
	ASSERT_ALWAYS(i->size() == 0)

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_ALWAYS(i->get() == "unqu/otedString")
	ASSERT_ALWAYS(i->size() == 0)

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_ALWAYS(i->get() == "one\ntwo three\tfour")
	ASSERT_ALWAYS(i->size() == 0)

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_ALWAYS(i->get() == "string with empty children list")
	ASSERT_ALWAYS(i->size() == 0)

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_ALWAYS(i->get() == "unquoted_string_with_empty_children_list")
	ASSERT_ALWAYS(i->size() == 0)

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_INFO_ALWAYS(i->get() == "string_interrupted", i->get())
	ASSERT_ALWAYS(i->size() == 0)

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_INFO_ALWAYS(i->get() == "_by_comment", i->get())
	ASSERT_ALWAYS(i->size() == 0)

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_INFO_ALWAYS(i->get() == "string_broken", i->get())
	ASSERT_ALWAYS(i->size() == 0)

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_INFO_ALWAYS(i->get() == "_by_comment", i->get())
	ASSERT_ALWAYS(i->size() == 0)

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_ALWAYS(i->get() == "")
	ASSERT_ALWAYS(i->get().length() == 0)
	ASSERT_INFO_ALWAYS(i->size() == 0, i->children().front().get())

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_ALWAYS(i->get() == "")
	ASSERT_ALWAYS(i->get().length() == 0)
	ASSERT_ALWAYS(i->size() == 2)
	{
		auto& b = i->children();
		auto ci = b.begin();
		ASSERT_ALWAYS(ci != b.end())
		ASSERT_ALWAYS(ci->get() == "")
		ASSERT_ALWAYS(ci->get().length() == 0)
		ASSERT_ALWAYS(ci->size() == 0)

		++ci;
		ASSERT_ALWAYS(ci != b.end())
		ASSERT_ALWAYS(ci->get() == "")
		ASSERT_ALWAYS(ci->get().length() == 0)
		ASSERT_ALWAYS(ci->size() == 0)

		++ci;
		ASSERT_ALWAYS(ci == b.end())
	}

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_ALWAYS(i->get() == "")
	ASSERT_INFO_ALWAYS(i->size() == 5, i->size())
	{
		auto& b = i->children();

		ASSERT_ALWAYS(b[0].get() == "child string")
		ASSERT_ALWAYS(b[3].get() == "ccc")

		auto i = b.begin();
		ASSERT_ALWAYS(i != b.end())
		ASSERT_ALWAYS(i->get() == "child string")

		++i;
		ASSERT_ALWAYS(i != b.end())
		ASSERT_ALWAYS(i->get() == "aaa")
		ASSERT_ALWAYS(i->size() == 0)

		++i;
		ASSERT_ALWAYS(i != b.end())
		ASSERT_ALWAYS(i->get() == "bbb")
		ASSERT_ALWAYS(i->size() == 0)

		++i;
		ASSERT_ALWAYS(i != b.end())
		ASSERT_ALWAYS(i->get() == "ccc")
		{
			auto& b = i->children();
			ASSERT_ALWAYS(b.size() == 1)

			ASSERT_ALWAYS(b[0].get() == "ddd")
		}

		++i;
		ASSERT_ALWAYS(i != b.end())
		ASSERT_ALWAYS(i->get() == "")
		ASSERT_ALWAYS(i->size() == 0)

		++i;
		ASSERT_ALWAYS(i == b.end())
	}

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_ALWAYS(i->get() == "Tab\t Backslash\\ Slash/ Doubleslash// Newline\n Carriagereturn\r Doublequotes\" {}{}{}{}")
	ASSERT_ALWAYS(i->size() == 0)

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_ALWAYS(i->get() == "quoted string with trailing slash /")
	ASSERT_ALWAYS(i->size() == 0)

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_ALWAYS(i->get() == "fff ggg")
	ASSERT_ALWAYS(i->size() == 0)

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_ALWAYS(i->get() == "tralala tro lo lo\ntre lele")
	ASSERT_ALWAYS(i->size() == 0)

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_ALWAYS(i->get() == "-1213.33")
	ASSERT_ALWAYS(i->size() == 0)

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_ALWAYS(i->get() == "UnquotedStringAtTheVeryEndOfTheFile")
	ASSERT_ALWAYS(i->size() == 0)

	++i;
	ASSERT_ALWAYS(i == roots.end())


	{
		std::string str("test string");
		auto i = std::find(roots.begin(), roots.end(), str);
		ASSERT_ALWAYS(i != roots.end())
		ASSERT_ALWAYS(i->get() == str)
	}
	{
		std::string str("unqu/otedString");
		const auto& const_roots = roots;
		auto i = std::find(const_roots.begin(), const_roots.end(), str);
		ASSERT_ALWAYS(i != const_roots.end())
		ASSERT_ALWAYS(i->get() == str)
	}

	{
		auto cloned = roots;
		ASSERT_ALWAYS(cloned.size() != 0)
		ASSERT_ALWAYS(cloned.size() == roots.size())
	}

}



int main(int argc, char** argv){

	test_puu_basic_parsing();

	return 0;
}
