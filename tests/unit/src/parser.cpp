#include <tst/set.hpp>
#include <tst/check.hpp>

#include <deque>

#include <papki/fs_file.hpp>

#include "../../../src/treeml/parser.hpp"

namespace{
enum action_enum{
	children_start,
	children_end,
	string_item
};
}

namespace{
class test_listener : public tml::listener{
	void on_children_parse_finished(tml::location)override{
		// TRACE(<< "}" << std::endl)
		tst::check(this->actions.size() > 0, SL);
		tst::check(this->actions.front().first == children_end, SL) << "first = " << this->actions.front().first << " second = " << this->actions.front().second;
		this->actions.pop_front();
	}

	void on_children_parse_started(tml::location)override{
		// TRACE(<< "{" << std::endl)
		tst::check(this->actions.size() > 0, SL);
		tst::check(this->actions.front().first == children_start, SL) << "first = " << this->actions.front().first << " second = " << this->actions.front().second;
		this->actions.pop_front();
	}

	void on_string_parsed(std::string_view s, const tml::extra_info& info)override{
		std::string str(s);
		// utki::log([&](auto&o){
		// 	o << "str = " << str;
		// 	o << " line = " << info.location.line << '\n';
		// });
		tst::check(this->actions.size() > 0, SL);
		tst::check(this->actions.front().first == string_item, [&](auto&o){
				o << "first = " << this->actions.front().first;
				o << " second = " << this->actions.front().second;
				o << " line = " << info.location.line;
			}, SL);
		tst::check(this->actions.front().second == str, [&](auto&o){
				o << "first = " << this->actions.front().first;
				o << " second = " << this->actions.front().second;
				o << " str = " << str;
			}, SL);
		this->actions.pop_front();
	}

public:
	test_listener() = default;

	test_listener(const test_listener&) = default;
	test_listener& operator=(const test_listener&) = default;

	test_listener(test_listener&&) noexcept = default;
	test_listener& operator=(test_listener&&) = default;

	~test_listener() override = default;

	std::deque<std::pair<action_enum, std::string> > actions;
};
}

namespace{
const tst::set set("parser", [](auto& suite){
	suite.add("parse", [](){
		test_listener l;

		l.actions.emplace_back(string_item, "test string");
		l.actions.emplace_back(string_item, "another string");
		l.actions.emplace_back(string_item, "unquotedString");

		l.actions.emplace_back(string_item, "string with empty children list");
		l.actions.emplace_back(children_start, "");
		l.actions.emplace_back(children_end, "");

		l.actions.emplace_back(string_item, "unquoted_string_with_empty_children_list");
		l.actions.emplace_back(children_start, "");
		l.actions.emplace_back(children_end, "");

		l.actions.emplace_back(string_item, "");
		l.actions.emplace_back(children_start, "");
		l.actions.emplace_back(children_end, "");

		l.actions.emplace_back(string_item, "");
		l.actions.emplace_back(children_start, "");

		l.actions.emplace_back(string_item, "");
		l.actions.emplace_back(children_start, "");
		l.actions.emplace_back(children_end, "");

		l.actions.emplace_back(string_item, "");
		l.actions.emplace_back(children_start, "");
		l.actions.emplace_back(children_end, "");

		l.actions.emplace_back(children_end, "");

		l.actions.emplace_back(string_item, "");
		l.actions.emplace_back(children_start, "");

		l.actions.emplace_back(string_item, "child string");

		l.actions.emplace_back(string_item, "aaa");
		l.actions.emplace_back(children_start, "");
		l.actions.emplace_back(children_end, "");

		l.actions.emplace_back(string_item, "bbb");

		l.actions.emplace_back(string_item, "ccc");
		l.actions.emplace_back(children_start, "");
		l.actions.emplace_back(string_item, "ddd");
		l.actions.emplace_back(children_end, "");

		l.actions.emplace_back(children_end, "");

		l.actions.emplace_back(string_item, "Tab\t Backslash\\ Slash/ Doubleslash// Newline\n Doublequotes\" {}{}{}{}");

		l.actions.emplace_back(string_item, "quoted string with trailing slash /");

		l.actions.emplace_back(string_item, "fff ggg");

		l.actions.emplace_back(string_item, "tralala tro lo lo\ntre lele");

		l.actions.emplace_back(string_item, "-1213.33");

		l.actions.emplace_back(string_item, "UnquotedStringAtTheVeryEndOfTheFile");

		papki::fs_file fi("parser_data/test.tml");

		tml::parse(fi, l);

		tst::check(l.actions.size() == 0, SL);
	});
});
}
