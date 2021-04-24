#include <tst/set.hpp>
#include <tst/check.hpp>

#include "../../src/treeml/parser.hpp"

namespace{
tst::set set("parser", [](auto& suite){
	suite.template add<const char*>(
			"flag_space_should_be_false",
			{
				"hello",
				"hello post",
				"pre\"hello\"",
				"\"pre\"hello",
				"{}hello",
				" pre {}hello", // space between string and children list does not count
				"pre{child1 child2}hello post",
				"R\"qwe(raw string)qwe\"hello",
				"R\"qwe(hello)qwe\"",
				"pre {}R\"qwe(hello)qwe\"",
				"pre{hello child2}"
			},
			[](const auto& p){
				treeml::parser parser;

				struct listener : public treeml::listener{
					bool string_parsed = false;
					void on_string_parsed(std::string_view str, utki::flags<treeml::flag> flags)override{
						if(str == "hello"){
							string_parsed = true;
							tst::check(!flags.get(treeml::flag::space), SL);
						}
					}

					void on_children_parse_started()override{}
					void on_children_parse_finished()override{}
				} l;

				parser.parse_data_chunk(p, l);
				parser.end_of_data(l);

				tst::check(l.string_parsed, SL);
			}
		);
	
	suite.template add<const char*>(
			"flag_space_should_be_true",
			{
				" hello",
				"\nhello",
				"\thello",
				" hello post",
				"\nhello post",
				"\thello post",
				"pre \"hello\"",
				"pre\n \"hello\"",
				"pre\t\"hello\"",
				"\"pre\" hello",
				"\"pre\"\nhello",
				"\"pre\"\thello",
				"{} hello",
				"{}\nhello",
				"{}\thello",
				"pre{child1 child2} hello post",
				"pre{child1 child2}\nhello post",
				"pre{child1 child2}\thello post",
				"R\"qwe(raw string)qwe\" hello",
				"R\"qwe(raw string)qwe\"\nhello",
				"R\"qwe(raw string)qwe\"\thello",
				"pre R\"qwe(hello)qwe\"",
				"pre\nR\"qwe(hello)qwe\"",
				"pre\tR\"qwe(hello)qwe\"",
				"pre{ hello child2}",
				"pre{\nhello child2}",
				"pre{\thello child2}",
			},
			[](const auto& p){
				treeml::parser parser;

				struct listener : public treeml::listener{
					bool string_parsed = false;
					void on_string_parsed(std::string_view str, utki::flags<treeml::flag> flags)override{
						if(str == "hello"){
							string_parsed = true;
							tst::check(flags.get(treeml::flag::space), SL);
						}
					}

					void on_children_parse_started()override{}
					void on_children_parse_finished()override{}
				} l;

				parser.parse_data_chunk(p, l);
				parser.end_of_data(l);

				tst::check(l.string_parsed, SL);
			}
		);
});
}
