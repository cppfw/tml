#include <tst/set.hpp>
#include <tst/check.hpp>

#include <deque>

#include <papki/fs_file.hpp>

#include <treeml/parser.hpp>

namespace{
enum E_Action{
	CHILDREN_START,
	CHILDREN_END,
	STRING
};
}

namespace{
class Listener : public tml::listener{
	void on_children_parse_finished(tml::location)override{
		// TRACE(<< "}" << std::endl)
		tst::check(this->actions.size() > 0, SL);
		tst::check(this->actions.front().first == CHILDREN_END, SL) << "first = " << this->actions.front().first << " second = " << this->actions.front().second;
		this->actions.pop_front();
	}

	void on_children_parse_started(tml::location)override{
		// TRACE(<< "{" << std::endl)
		tst::check(this->actions.size() > 0, SL);
		tst::check(this->actions.front().first == CHILDREN_START, SL) << "first = " << this->actions.front().first << " second = " << this->actions.front().second;
		this->actions.pop_front();
	}

	void on_string_parsed(std::string_view s, const tml::extra_info& info)override{
		std::string str(s);
		// utki::log([&](auto&o){
		// 	o << "str = " << str;
		// 	o << " line = " << info.location.line << '\n';
		// });
		tst::check(this->actions.size() > 0, SL);
		tst::check(this->actions.front().first == STRING, [&](auto&o){
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
	~Listener()noexcept{}

	std::deque<std::pair<E_Action, std::string> > actions;
};
}

namespace{
tst::set set("parser", [](auto& suite){
	suite.add("parse", [](){
		Listener l;

		typedef std::pair<E_Action, std::string> T_Pair;
		l.actions.push_back(T_Pair(STRING, "test string"));
		l.actions.push_back(T_Pair(STRING, "another string"));
		l.actions.push_back(T_Pair(STRING, "unquotedString"));

		l.actions.push_back(T_Pair(STRING, "string with empty children list"));
		l.actions.push_back(T_Pair(CHILDREN_START, ""));
		l.actions.push_back(T_Pair(CHILDREN_END, ""));

		l.actions.push_back(T_Pair(STRING, "unquoted_string_with_empty_children_list"));
		l.actions.push_back(T_Pair(CHILDREN_START, ""));
		l.actions.push_back(T_Pair(CHILDREN_END, ""));

		l.actions.push_back(T_Pair(STRING, ""));
		l.actions.push_back(T_Pair(CHILDREN_START, ""));
		l.actions.push_back(T_Pair(CHILDREN_END, ""));

		l.actions.push_back(T_Pair(STRING, ""));
		l.actions.push_back(T_Pair(CHILDREN_START, ""));

		l.actions.push_back(T_Pair(STRING, ""));
		l.actions.push_back(T_Pair(CHILDREN_START, ""));
		l.actions.push_back(T_Pair(CHILDREN_END, ""));

		l.actions.push_back(T_Pair(STRING, ""));
		l.actions.push_back(T_Pair(CHILDREN_START, ""));
		l.actions.push_back(T_Pair(CHILDREN_END, ""));

		l.actions.push_back(T_Pair(CHILDREN_END, ""));

		l.actions.push_back(T_Pair(STRING, ""));
		l.actions.push_back(T_Pair(CHILDREN_START, ""));

		l.actions.push_back(T_Pair(STRING, "child string"));

		l.actions.push_back(T_Pair(STRING, "aaa"));
		l.actions.push_back(T_Pair(CHILDREN_START, ""));
		l.actions.push_back(T_Pair(CHILDREN_END, ""));

		l.actions.push_back(T_Pair(STRING, "bbb"));

		l.actions.push_back(T_Pair(STRING, "ccc"));
		l.actions.push_back(T_Pair(CHILDREN_START, ""));
		l.actions.push_back(T_Pair(STRING, "ddd"));
		l.actions.push_back(T_Pair(CHILDREN_END, ""));

		l.actions.push_back(T_Pair(CHILDREN_END, ""));

		l.actions.push_back(T_Pair(STRING, "Tab\t Backslash\\ Slash/ Doubleslash// Newline\n Doublequotes\" {}{}{}{}"));

		l.actions.push_back(T_Pair(STRING, "quoted string with trailing slash /"));

		l.actions.push_back(T_Pair(STRING, "fff ggg"));

		l.actions.push_back(T_Pair(STRING, "tralala tro lo lo\ntre lele"));

		l.actions.push_back(T_Pair(STRING, "-1213.33"));

		l.actions.push_back(T_Pair(STRING, "UnquotedStringAtTheVeryEndOfTheFile"));

		papki::fs_file fi("parser_data/test.tml");

		tml::parse(fi, l);

		tst::check(l.actions.size() == 0, SL);
	});
});
}
