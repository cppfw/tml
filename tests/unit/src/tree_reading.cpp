#include <tst/set.hpp>
#include <tst/check.hpp>

#include <papki/fs_file.hpp>

#include <treeml/tree.hpp>

namespace{
tst::set set("tree_reading", [](tst::suite& suite){
	suite.add("read", [](){
		papki::fs_file fi("tree_reading_data/test.tml");

		auto roots = tml::read(fi);

		auto i = roots.begin();

		tst::check(i->value == "ttt", SL);
		tst::check(i->children.size() == 0, SL);

		++i;
		tst::check(i != roots.end(), SL);
		tst::check(i->value == "test string", SL) << i->value.to_string();
		tst::check(i->children.size() == 0, SL);

		++i;
		tst::check(i != roots.end(), SL);
		tst::check(i->value == "anot/her string", SL) << i->value.to_string();
		tst::check(i->children.size() == 0, SL);

		++i;
		tst::check(i != roots.end(), SL);
		tst::check(i->value == "unqu/otedString", SL);
		tst::check(i->children.size() == 0, SL);

		++i;
		tst::check(i != roots.end(), SL);
		tst::check(i->value == "one\ntwo three\tfour", SL);
		tst::check(i->children.size() == 0, SL);

		++i;
		tst::check(i != roots.end(), SL);
		tst::check(i->value == "string with empty children list", SL);
		tst::check(i->children.size() == 0, SL);

		++i;
		tst::check(i != roots.end(), SL);
		tst::check(i->value == "unquoted_string_with_empty_children_list", SL);
		tst::check(i->children.size() == 0, SL);

		++i;
		tst::check(i != roots.end(), SL);
		tst::check(i->value == "string_interrupted//tro", SL) << i->value.to_string();
		tst::check(i->children.size() == 0, SL);

		++i;
		tst::check(i != roots.end(), SL);
		tst::check(i->value == "_by_comment", SL) << i->value.to_string();
		tst::check(i->children.size() == 0, SL);

		++i;
		tst::check(i != roots.end(), SL);
		tst::check(i->value == "string_broken/*tro", SL) << i->value.to_string();
		tst::check(i->children.size() == 0, SL);

		++i;
		tst::check(i != roots.end(), SL);
		tst::check(i->value == "*/_by_comment", SL) << i->value.to_string();
		tst::check(i->children.size() == 0, SL);

		++i;
		tst::check(i != roots.end(), SL);
		tst::check(i->value == "", SL);
		tst::check(i->value.length() == 0, SL);
		tst::check(i->children.size() == 0, SL) << i->children.front().value.to_string();

		++i;
		tst::check(i != roots.end(), SL);
		tst::check(i->value == "", SL);
		tst::check(i->value.length() == 0, SL);
		tst::check(i->children.size() == 2, SL);
		{
			auto& b = i->children;
			auto ci = b.begin();
			tst::check(ci != b.end(), SL);
			tst::check(ci->value == "", SL);
			tst::check(ci->value.length() == 0, SL);
			tst::check(ci->children.size() == 0, SL);

			++ci;
			tst::check(ci != b.end(), SL);
			tst::check(ci->value == "", SL);
			tst::check(ci->value.length() == 0, SL);
			tst::check(ci->children.size() == 0, SL);

			++ci;
			tst::check(ci == b.end(), SL);
		}

		++i;
		tst::check(i != roots.end(), SL);
		tst::check(i->value == "", SL);
		tst::check(i->children.size() == 5, SL) << i->children.size();
		{
			auto& b = i->children;

			tst::check(b[0].value == "child string", SL);
			tst::check(b[3].value == "ccc", SL);

			auto i = b.begin();
			tst::check(i != b.end(), SL);
			tst::check(i->value == "child string", SL);

			++i;
			tst::check(i != b.end(), SL);
			tst::check(i->value == "aaa", SL);
			tst::check(i->children.size() == 0, SL);

			++i;
			tst::check(i != b.end(), SL);
			tst::check(i->value == "bbb", SL);
			tst::check(i->children.size() == 0, SL);

			++i;
			tst::check(i != b.end(), SL);
			tst::check(i->value == "ccc", SL);
			{
				auto& b = i->children;
				tst::check(b.size() == 1, SL);

				tst::check(b[0].value == "ddd", SL);
			}

			++i;
			tst::check(i != b.end(), SL);
			tst::check(i->value == "", SL);
			tst::check(i->children.size() == 0, SL);

			++i;
			tst::check(i == b.end(), SL);
		}

		++i;
		tst::check(i != roots.end(), SL);
		tst::check(i->value == "Tab\t Backslash\\ Slash/ Doubleslash// Newline\n Doublequotes\" {}{}{}{}", SL);
		tst::check(i->children.size() == 0, SL);

		++i;
		tst::check(i != roots.end(), SL);
		tst::check(i->value == "quoted string with trailing slash /", SL);
		tst::check(i->children.size() == 0, SL);

		++i;
		tst::check(i != roots.end(), SL);
		tst::check(i->value == "fff ggg", SL);
		tst::check(i->children.size() == 0, SL);

		++i;
		tst::check(i != roots.end(), SL);
		tst::check(i->value == "tralala tro lo lo\ntre lele", SL);
		tst::check(i->children.size() == 0, SL);

		++i;
		tst::check(i != roots.end(), SL);
		tst::check(i->value == "-1213.33", SL);
		tst::check(i->children.size() == 0, SL);

		++i;
		tst::check(i != roots.end(), SL);
		tst::check(i->value == "UnquotedStringAtTheVeryEndOfTheFile", SL);
		tst::check(i->children.size() == 0, SL);

		++i;
		tst::check(i == roots.end(), SL);

		{
			std::string str("test string");
			auto i = std::find(roots.begin(), roots.end(), str);
			tst::check(i != roots.end(), SL);
			tst::check(i->value == str, SL);
		}
		{
			std::string str("unqu/otedString");
			const auto& const_roots = roots;
			auto i = std::find(const_roots.begin(), const_roots.end(), str);
			tst::check(i != const_roots.end(), SL);
			tst::check(i->value == str, SL);
		}

		{
			auto cloned = roots;
			tst::check(cloned.size() != 0, SL);
			tst::check(cloned.size() == roots.size(), SL);
		}
	});
});
}
