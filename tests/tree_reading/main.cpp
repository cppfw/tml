#include "../../src/puu/tree.hpp"

#include <utki/debug.hpp>
#include <papki/FSFile.hpp>



void test_puu_basic_parsing(){
	papki::FSFile fi("test.puu");

	auto roots = puu::read(fi);

	auto i = roots.begin();

	ASSERT_ALWAYS(i->value == "ttt");
	ASSERT_ALWAYS(i->children.size() == 0);

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_INFO_ALWAYS(i->value == "test string", i->value.to_string())
	ASSERT_ALWAYS(i->children.size() == 0)

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_INFO_ALWAYS(i->value == "anot/her string", i->value.to_string())
	ASSERT_ALWAYS(i->children.size() == 0)

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_ALWAYS(i->value == "unqu/otedString")
	ASSERT_ALWAYS(i->children.size() == 0)

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_ALWAYS(i->value == "one\ntwo three\tfour")
	ASSERT_ALWAYS(i->children.size() == 0)

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_ALWAYS(i->value == "string with empty children list")
	ASSERT_ALWAYS(i->children.size() == 0)

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_ALWAYS(i->value == "unquoted_string_with_empty_children_list")
	ASSERT_ALWAYS(i->children.size() == 0)

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_INFO_ALWAYS(i->value == "string_interrupted", i->value.to_string())
	ASSERT_ALWAYS(i->children.size() == 0)

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_INFO_ALWAYS(i->value == "_by_comment", i->value.to_string())
	ASSERT_ALWAYS(i->children.size() == 0)

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_INFO_ALWAYS(i->value == "string_broken", i->value.to_string())
	ASSERT_ALWAYS(i->children.size() == 0)

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_INFO_ALWAYS(i->value == "_by_comment", i->value.to_string())
	ASSERT_ALWAYS(i->children.size() == 0)

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_ALWAYS(i->value == "")
	ASSERT_ALWAYS(i->value.length() == 0)
	ASSERT_INFO_ALWAYS(i->children.size() == 0, i->children.front().value.to_string())

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_ALWAYS(i->value == "")
	ASSERT_ALWAYS(i->value.length() == 0)
	ASSERT_ALWAYS(i->children.size() == 2)
	{
		auto& b = i->children;
		auto ci = b.begin();
		ASSERT_ALWAYS(ci != b.end())
		ASSERT_ALWAYS(ci->value == "")
		ASSERT_ALWAYS(ci->value.length() == 0)
		ASSERT_ALWAYS(ci->children.size() == 0)

		++ci;
		ASSERT_ALWAYS(ci != b.end())
		ASSERT_ALWAYS(ci->value == "")
		ASSERT_ALWAYS(ci->value.length() == 0)
		ASSERT_ALWAYS(ci->children.size() == 0)

		++ci;
		ASSERT_ALWAYS(ci == b.end())
	}

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_ALWAYS(i->value == "")
	ASSERT_INFO_ALWAYS(i->children.size() == 5, i->children.size())
	{
		auto& b = i->children;

		ASSERT_ALWAYS(b[0].value == "child string")
		ASSERT_ALWAYS(b[3].value == "ccc")

		auto i = b.begin();
		ASSERT_ALWAYS(i != b.end())
		ASSERT_ALWAYS(i->value == "child string")

		++i;
		ASSERT_ALWAYS(i != b.end())
		ASSERT_ALWAYS(i->value == "aaa")
		ASSERT_ALWAYS(i->children.size() == 0)

		++i;
		ASSERT_ALWAYS(i != b.end())
		ASSERT_ALWAYS(i->value == "bbb")
		ASSERT_ALWAYS(i->children.size() == 0)

		++i;
		ASSERT_ALWAYS(i != b.end())
		ASSERT_ALWAYS(i->value == "ccc")
		{
			auto& b = i->children;
			ASSERT_ALWAYS(b.size() == 1)

			ASSERT_ALWAYS(b[0].value == "ddd")
		}

		++i;
		ASSERT_ALWAYS(i != b.end())
		ASSERT_ALWAYS(i->value == "")
		ASSERT_ALWAYS(i->children.size() == 0)

		++i;
		ASSERT_ALWAYS(i == b.end())
	}

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_ALWAYS(i->value == "Tab\t Backslash\\ Slash/ Doubleslash// Newline\n Carriagereturn\r Doublequotes\" {}{}{}{}")
	ASSERT_ALWAYS(i->children.size() == 0)

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_ALWAYS(i->value == "quoted string with trailing slash /")
	ASSERT_ALWAYS(i->children.size() == 0)

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_ALWAYS(i->value == "fff ggg")
	ASSERT_ALWAYS(i->children.size() == 0)

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_ALWAYS(i->value == "tralala tro lo lo\ntre lele")
	ASSERT_ALWAYS(i->children.size() == 0)

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_ALWAYS(i->value == "-1213.33")
	ASSERT_ALWAYS(i->children.size() == 0)

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_ALWAYS(i->value == "UnquotedStringAtTheVeryEndOfTheFile")
	ASSERT_ALWAYS(i->children.size() == 0)

	++i;
	ASSERT_ALWAYS(i == roots.end())


	{
		std::string str("test string");
		auto i = std::find(roots.begin(), roots.end(), str);
		ASSERT_ALWAYS(i != roots.end())
		ASSERT_ALWAYS(i->value == str)
	}
	{
		std::string str("unqu/otedString");
		const auto& const_roots = roots;
		auto i = std::find(const_roots.begin(), const_roots.end(), str);
		ASSERT_ALWAYS(i != const_roots.end())
		ASSERT_ALWAYS(i->value == str)
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
