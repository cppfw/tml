#include "../../src/puu/tree.hpp"

#include <utki/debug.hpp>
#include <papki/FSFile.hpp>



void test_puu_basic_parsing(){
	papki::FSFile fi("test.puu");

	auto roots = puu::read(fi);

	auto i = roots.begin();

	ASSERT_ALWAYS(i->leaf() == "ttt");
	ASSERT_ALWAYS(i->size() == 0);

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_INFO_ALWAYS(i->leaf() == "test string", i->leaf())
	ASSERT_ALWAYS(i->size() == 0)

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_INFO_ALWAYS(i->leaf() == "anot/her string", i->leaf())
	ASSERT_ALWAYS(i->size() == 0)

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_ALWAYS(i->leaf() == "unqu/otedString")
	ASSERT_ALWAYS(i->size() == 0)

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_ALWAYS(i->leaf() == "one\ntwo three\tfour")
	ASSERT_ALWAYS(i->size() == 0)

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_ALWAYS(i->leaf() == "string with empty children list")
	ASSERT_ALWAYS(i->size() == 0)

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_ALWAYS(i->leaf() == "unquoted_string_with_empty_children_list")
	ASSERT_ALWAYS(i->size() == 0)

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_INFO_ALWAYS(i->leaf() == "string_interrupted", i->leaf())
	ASSERT_ALWAYS(i->size() == 0)

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_INFO_ALWAYS(i->leaf() == "_by_comment", i->leaf())
	ASSERT_ALWAYS(i->size() == 0)

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_INFO_ALWAYS(i->leaf() == "string_broken", i->leaf())
	ASSERT_ALWAYS(i->size() == 0)

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_INFO_ALWAYS(i->leaf() == "_by_comment", i->leaf())
	ASSERT_ALWAYS(i->size() == 0)

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_ALWAYS(i->leaf() == "")
	ASSERT_ALWAYS(i->leaf().length() == 0)
	ASSERT_INFO_ALWAYS(i->size() == 0, i->branches().front().leaf())

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_ALWAYS(i->leaf() == "")
	ASSERT_ALWAYS(i->leaf().length() == 0)
	ASSERT_ALWAYS(i->size() == 2)
	{
		auto& b = i->branches();
		auto ci = b.begin();
		ASSERT_ALWAYS(ci != b.end())
		ASSERT_ALWAYS(ci->leaf() == "")
		ASSERT_ALWAYS(ci->leaf().length() == 0)
		ASSERT_ALWAYS(ci->size() == 0)

		++ci;
		ASSERT_ALWAYS(ci != b.end())
		ASSERT_ALWAYS(ci->leaf() == "")
		ASSERT_ALWAYS(ci->leaf().length() == 0)
		ASSERT_ALWAYS(ci->size() == 0)

		++ci;
		ASSERT_ALWAYS(ci == b.end())
	}

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_ALWAYS(i->leaf() == "")
	ASSERT_INFO_ALWAYS(i->size() == 5, i->size())
	{
		auto& b = i->branches();

		ASSERT_ALWAYS(b[0].leaf() == "child string")
		ASSERT_ALWAYS(b[3].leaf() == "ccc")

		auto i = b.begin();
		ASSERT_ALWAYS(i != b.end())
		ASSERT_ALWAYS(i->leaf() == "child string")

		++i;
		ASSERT_ALWAYS(i != b.end())
		ASSERT_ALWAYS(i->leaf() == "aaa")
		ASSERT_ALWAYS(i->size() == 0)

		++i;
		ASSERT_ALWAYS(i != b.end())
		ASSERT_ALWAYS(i->leaf() == "bbb")
		ASSERT_ALWAYS(i->size() == 0)

		++i;
		ASSERT_ALWAYS(i != b.end())
		ASSERT_ALWAYS(i->leaf() == "ccc")
		{
			auto& b = i->branches();
			ASSERT_ALWAYS(b.size() == 1)

			ASSERT_ALWAYS(b[0].leaf() == "ddd")
		}

		++i;
		ASSERT_ALWAYS(i != b.end())
		ASSERT_ALWAYS(i->leaf() == "")
		ASSERT_ALWAYS(i->size() == 0)

		++i;
		ASSERT_ALWAYS(i == b.end())
	}

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_ALWAYS(i->leaf() == "Tab\t Backslash\\ Slash/ Doubleslash// Newline\n Carriagereturn\r Doublequotes\" {}{}{}{}")
	ASSERT_ALWAYS(i->size() == 0)

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_ALWAYS(i->leaf() == "quoted string with trailing slash /")
	ASSERT_ALWAYS(i->size() == 0)

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_ALWAYS(i->leaf() == "fff ggg")
	ASSERT_ALWAYS(i->size() == 0)

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_ALWAYS(i->leaf() == "tralala tro lo lo\ntre lele")
	ASSERT_ALWAYS(i->size() == 0)

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_ALWAYS(i->leaf() == "-1213.33")
	ASSERT_ALWAYS(i->size() == 0)

	++i;
	ASSERT_ALWAYS(i != roots.end())
	ASSERT_ALWAYS(i->leaf() == "UnquotedStringAtTheVeryEndOfTheFile")
	ASSERT_ALWAYS(i->size() == 0)

	++i;
	ASSERT_ALWAYS(i == roots.end())


	{
		std::string str("test string");
		auto i = std::find(roots.begin(), roots.end(), str);
		ASSERT_ALWAYS(i != roots.end())
		ASSERT_ALWAYS(i->leaf() == str)
	}
	{
		std::string str("unqu/otedString");
		const auto& const_roots = roots;
		auto i = std::find(const_roots.begin(), const_roots.end(), str);
		ASSERT_ALWAYS(i != const_roots.end())
		ASSERT_ALWAYS(i->leaf() == str)
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
