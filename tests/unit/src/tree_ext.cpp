#include "../../../src/tml/tree_ext.hpp"

#include <algorithm>

#include <fsif/vector_file.hpp>
#include <tst/check.hpp>
#include <tst/set.hpp>
#include <utki/string.hpp>

namespace {
const tst::set set("tree_ext", [](auto& suite) {
	suite.add("compare_leaf_ext_to_string", []() {
		std::string str = "hello world!";
		tml::leaf_ext l{std::string(str)};
		tst::check(l == str.c_str(), SL);
		tst::check(l == str, SL);
	});

	suite.add("compare_leaf_ext_to_string_view", []() {
		std::string str = "hello world!";
		tml::leaf_ext l{std::string(str)};
		tst::check(l == str.c_str(), SL);
		tst::check(l == std::string_view(str), SL);
	});

	suite.add("compare_tree_ext_to_string", []() {
		std::string str = "hello world!";
		tml::tree_ext t{std::string(str)};
		tst::check(t == str.c_str(), SL);
		tst::check(t == str, SL);
	});

	suite.add("find_tree_from_forest_by_string", []() {
		std::string str = "hello world!";
		tml::forest_ext f{
			{"hi"},
			{"how are you?"},
			{str},
			{"I'm fine"},
		};

		auto i = std::find_if(
			f.begin(), //
			f.end(), //
			[&str](const auto& t) {
				return t == str;
			}
		);
		tst::check(i != f.end(), SL);
	});

	suite.add("write_ext_unquoted_minimal", []() {
		tml::forest_ext f;
		f.emplace_back("hello");
		auto& b = f.emplace_back("world");
		b.value.info.flags.set(tml::flag::space);

		fsif::vector_file fi;
		tml::write(f, fi, tml::formatting::minimal);
		auto data = fi.reset_data();

		tst::check_eq(utki::make_string(data), std::string("hello world"), SL);
	});

	suite.add("write_ext_quoted_minimal", []() {
		tml::forest_ext f;
		auto& a = f.emplace_back("hello world");
		a.value.info.flags.set(tml::flag::quoted);

		fsif::vector_file fi;
		tml::write(f, fi, tml::formatting::minimal);
		auto data = fi.reset_data();

		tst::check_eq(utki::make_string(data), std::string("\"hello world\""), SL);
	});

	suite.add("write_ext_raw_minimal", []() {
		tml::forest_ext f;
		auto& a = f.emplace_back("hello world");
		a.value.info.flags.set(tml::flag::raw);

		fsif::vector_file fi;
		tml::write(f, fi, tml::formatting::minimal);
		auto data = fi.reset_data();

		tst::check_eq(utki::make_string(data), std::string("\"\"\"hello world\"\"\""), SL);
	});

	suite.add("write_ext_curly_braces_empty", []() {
		tml::forest_ext f;
		auto& a = f.emplace_back("hello");
		a.value.info.flags.set(tml::flag::curly_braces);

		fsif::vector_file fi;
		tml::write(f, fi, tml::formatting::minimal);
		auto data = fi.reset_data();

		tst::check_eq(utki::make_string(data), std::string("hello{}"), SL);
	});

	suite.add("write_ext_with_children_minimal", []() {
		tml::forest_ext f;
		auto& a = f.emplace_back("hello");
		a.value.info.flags.set(tml::flag::curly_braces);
		a.children.emplace_back("world");
		a.children.emplace_back("foo");

		fsif::vector_file fi;
		tml::write(f, fi, tml::formatting::minimal);
		auto data = fi.reset_data();

		tst::check_eq(utki::make_string(data), std::string("hello{worldfoo}"), SL);
	});

	suite.add("write_ext_quoted_with_escapes", []() {
		tml::forest_ext f;
		auto& a = f.emplace_back("hello\nworld");
		a.value.info.flags.set(tml::flag::quoted);

		fsif::vector_file fi;
		tml::write(f, fi, tml::formatting::minimal);
		auto data = fi.reset_data();

		tst::check_eq(utki::make_string(data), std::string("\"hello\\nworld\""), SL);
	});

	suite.add("write_ext_normal_formatting", []() {
		tml::forest_ext f;
		auto& a = f.emplace_back("hello");
		a.children.emplace_back("world");

		fsif::vector_file fi;
		tml::write(f, fi, tml::formatting::normal);
		auto data = fi.reset_data();

		tst::check_eq(utki::make_string(data), std::string("hello{world}\n"), SL);
	});

	suite.add("write_ext_to_string", []() {
		tml::forest_ext f;
		f.emplace_back("hello");
		auto& b = f.emplace_back("world");
		b.value.info.flags.set(tml::flag::space);

		auto s = tml::to_string(f);

		tst::check_eq(s, std::string("hello world"), SL);
	});

	suite.add("write_ext_roundtrip_minimal", []() {
		const auto input = std::string("hello \"world\" \"\"\"raw\"\"\" foo{}");

		auto f = tml::read_ext(input);

		fsif::vector_file fi;
		tml::write(f, fi, tml::formatting::minimal);
		auto data = fi.reset_data();
		auto output = utki::make_string(data);

		tst::check_eq(output, input, SL);
	});
});
} // namespace
