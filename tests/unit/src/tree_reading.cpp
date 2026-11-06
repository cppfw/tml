#include <ranges>

#include <fsif/native_file.hpp>
#include <tst/check.hpp>
#include <tst/set.hpp>
#include <utki/debug.hpp>

#include "../../../src/tml/tree.hpp"

namespace {
const tst::set set("tree_reading", [](tst::suite& suite) {
	suite.add("read", []() {
		fsif::native_file fi("res/tree_reading_data/test.tml");

		auto roots = tml::read(fi);

		auto i = roots.begin();

		tst::check(i->value == "ttt", SL);
		tst::check(i->children.size() == 0, SL);

		++i;
		tst::check(i != roots.end(), SL);
		tst::check(
			i->value == "test string",
			[&](auto& o) {
				o << i->value.string;
			},
			SL
		);
		tst::check(i->children.size() == 0, SL);

		++i;
		tst::check(i != roots.end(), SL);
		tst::check(
			i->value == "anot/her string",
			[&](auto& o) {
				o << i->value.string;
			},
			SL
		);
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
		tst::check(
			i->value == "string_interrupted//tro",
			[&](auto& o) {
				o << i->value.string;
			},
			SL
		);
		tst::check(i->children.size() == 0, SL);

		++i;
		tst::check(i != roots.end(), SL);
		tst::check(
			i->value == "_by_comment",
			[&](auto& o) {
				o << i->value.string;
			},
			SL
		);
		tst::check(i->children.size() == 0, SL);

		++i;
		tst::check(i != roots.end(), SL);
		tst::check(
			i->value == "string_broken/*tro",
			[&](auto& o) {
				o << i->value.string;
			},
			SL
		);
		tst::check(i->children.size() == 0, SL);

		++i;
		tst::check(i != roots.end(), SL);
		tst::check(
			i->value == "*/_by_comment",
			[&](auto& o) {
				o << i->value.string;
			},
			SL
		);
		tst::check(i->children.size() == 0, SL);

		++i;
		tst::check(i != roots.end(), SL);
		tst::check(i->value == "", SL);
		tst::check(i->value.length() == 0, SL);
		tst::check(
			i->children.size() == 0,
			[&i](auto& o) {
				// if the check has failed, then the children list is not empty
				utki::assert(!i->children.empty(), SL);
				o << i->children.front().value.string;
			},
			SL
		);

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
		tst::check(
			i->children.size() == 5,
			[&](auto& o) {
				o << i->children.size();
			},
			SL
		);
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
			auto i = std::ranges::find_if(
				roots, //
				[&str](const auto& t) {
					return t == str;
				}
			);
			tst::check(i != roots.end(), SL);
			tst::check(i->value == str, SL);
		}
		{
			std::string str("unqu/otedString");
			const auto& const_roots = roots;
			auto i = std::ranges::find_if(
				const_roots, //
				[&str](const auto& t) {
					return t == str;
				}
			);
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
} // namespace
