#include <tst/set.hpp>
#include <tst/check.hpp>

#include "../../src/treeml/parser.hpp"

namespace{
tst::set set1("parser_flags", [](auto& suite){
	suite.template add<const char*>(
			"flag_space_should_be_false",
			{
				"hello",
				"hello post",
				"pre\"hello\"",
				"\"pre\"hello",
				"{}hello",
				" pre {}hello", // space between string and children list does not count
				" pre {\n}hello",
				" pre {child}hello",
				" pre {child }hello",
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
					void on_string_parsed(std::string_view str, const text_info& info)override{
						if(str == "hello"){
							this->string_parsed = true;
							tst::check(!info.flags.get(treeml::flag::space), SL);
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
					void on_string_parsed(std::string_view str, const text_info& info)override{
						if(str == "hello"){
							this->string_parsed = true;
							tst::check(info.flags.get(treeml::flag::space), SL);
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
			"flag_quoted_should_be_false",
			{
				"hello",
				" hello",
				"pre hello",
				" hello post",
				" hello \"post\"",
				"\"pre\"hello",
				" \"pre\"hello",
				"\"pre\" hello",
				"\"pre\"\thello",
				"\"pre\"\nhello",
				"R\"qwe(hello)qwe\"",
				" R\"qwe(hello)qwe\"post",
			},
			[](const auto& p){
				treeml::parser parser;

				struct listener : public treeml::listener{
					bool string_parsed = false;
					void on_string_parsed(std::string_view str, const text_info& info)override{
						if(str == "hello"){
							this->string_parsed = true;
							tst::check(!info.flags.get(treeml::flag::quoted), SL);
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
			"flag_quoted_should_be_true",
			{
				"\"hello\"",
				" \"hello\"",
				"pre\"hello\"",
				"pre \"hello\"",
				"pre\n\"hello\"",
				"pre\t\"hello\"",
				"\"pre\"\"hello\"",
				"\"pre\" \"hello\"",
				"\"pre\"\n\"hello\"",
				"\"pre\"\t\"hello\"",
				" R\"qwe(pre)qwe\"\"hello\"",
				" R\"qwe(pre)qwe\" \"hello\"",
				" R\"qwe(pre)qwe\"\n\"hello\"",
				" R\"qwe(pre)qwe\"\t\"hello\"",
			},
			[](const auto& p){
				treeml::parser parser;

				struct listener : public treeml::listener{
					bool string_parsed = false;
					void on_string_parsed(std::string_view str, const text_info& info)override{
						if(str == "hello"){
							this->string_parsed = true;
							tst::check(info.flags.get(treeml::flag::quoted), SL);
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
			"flag_cpp_raw_should_be_false",
			{
				"\"hello\"",
				"hello",
				" \"hello\"",
				" hello",
				"pre\"hello\"",
				"pre hello",
				"pre hello ",
				"pre \"hello\"",
				"pre \"hello\" ",
				"pre\n\"hello\"",
				"pre\t\"hello\"",
				"\"pre\"\"hello\"",
				"\"pre\" \"hello\"",
				"\"pre\"\n\"hello\"",
				"\"pre\"\t\"hello\"",
				" R\"qwe(pre)qwe\"\"hello\"",
				" R\"qwe(pre)qwe\" \"hello\"",
				" R\"qwe(pre)qwe\"\n\"hello\"",
				" R\"qwe(pre)qwe\"\t\"hello\"",
				"R\"hello\"",
				"R\"hello\" ",
				" R\"hello\" ",
				"pre R\"hello\" ",
			},
			[](const auto& p){
				treeml::parser parser;

				struct listener : public treeml::listener{
					bool string_parsed = false;
					void on_string_parsed(std::string_view str, const text_info& info)override{
						if(str == "hello"){
							this->string_parsed = true;
							tst::check(!info.flags.get(treeml::flag::raw_cpp), SL);
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
			"flag_cpp_raw_should_be_true",
			{
				"R\"qwe(hello)qwe\"",
				" R\"qwe(hello)qwe\"",
				"R\"qwe(hello)qwe\" ",
				" R\"qwe(hello)qwe\" ",
				"pre R\"qwe(hello)qwe\" ",
				"pre R\"qwe(hello)qwe\" post",
				"pre\nR\"qwe(hello)qwe\"\npost",
				"pre\tR\"qwe(hello)qwe\"\tpost",
			},
			[](const auto& p){
				treeml::parser parser;

				struct listener : public treeml::listener{
					bool string_parsed = false;
					void on_string_parsed(std::string_view str, const text_info& info)override{
						if(str == "hello"){
							this->string_parsed = true;
							tst::check(info.flags.get(treeml::flag::raw_cpp), SL);
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
			"raw_cpp_and_quoted_flags_should_be_false_for_r",
			{
				"R\"hello\"",
				" R\"hello\"",
				"R\"hello\" ",
				" R\"hello\" ",
				"pre R\"hello\" ",
				"pre R\"hello\" post",
				"pre\nR\"hello\"\npost",
				"pre\tR\"hello\"\tpost",
			},
			[](const auto& p){
				treeml::parser parser;

				struct listener : public treeml::listener{
					bool string_parsed = false;
					void on_string_parsed(std::string_view str, const text_info& info)override{
						if(str == "R"){
							this->string_parsed = true;
							tst::check(!info.flags.get(treeml::flag::quoted), SL);
							tst::check(!info.flags.get(treeml::flag::raw_cpp), SL);
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
			"space_flag_should_be_false_for_r",
			{
				"R\"hello\"",
				"R\"hello\" ",
				"pre{}R\"hello\" ",
				"pre { child}R\"hello\" post",
				"pre { child }R\"hello\" post",
				"pre { }R\"hello\" post",
				"pre{\n}R\"hello\"\npost",
				"pre{\t}R\"hello\"\tpost",
			},
			[](const auto& p){
				treeml::parser parser;

				struct listener : public treeml::listener{
					bool string_parsed = false;
					void on_string_parsed(std::string_view str, const text_info& info)override{
						if(str == "R"){
							this->string_parsed = true;
							tst::check(!info.flags.get(treeml::flag::space), SL);
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
			"space_flag_should_be_true_for_r",
			{
				" R\"hello\"",
				" R\"hello\" ",
				"pre R\"hello\" ",
				"pre R\"hello\" post",
				"pre\nR\"hello\"\npost",
				"pre\tR\"hello\"\tpost",
				"pre{} R\"hello\" post",
				"pre {\n} R\"hello\" post",
				"{} R\"hello\" post",
				"\n{} R\"hello\" post",
			},
			[](const auto& p){
				treeml::parser parser;

				struct listener : public treeml::listener{
					bool string_parsed = false;
					void on_string_parsed(std::string_view str, const text_info& info)override{
						if(str == "R"){
							this->string_parsed = true;
							tst::check(info.flags.get(treeml::flag::space), SL);
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

namespace{
tst::set set2("parser_location", [](auto& suite){
	suite.template add<std::tuple<const char*, size_t, size_t>>(
			"location",
			{
				{"hello", 0, 0},
				{" hello", 0, 1},
				{"\nhello", 1, 0},
				{"\n  hello", 1, 2},
				{"bla bla{child} bla{}\n  hello", 1, 2},
				{"bla bla{child} \"bla\n bla\" bla bla{child}hello", 1, 20},
				{"\"hello\"", 0, 0},
				{" \"hello\"", 0, 1},
				{"\n\"hello\"", 1, 0},
				{"\n  \"hello\"", 1, 2},
				{"bla bla{child} bla{}\n  \"hello\"", 1, 2},
				{"bla bla{child} \"bla\n bla\" bla bla{child}\"hello\" bla bla", 1, 20},
				{"R\"qwe(hello)qwe\"", 0, 0},
				{" R\"qwe(hello)qwe\"", 0, 1},
				{"\nR\"qwe(hello)qwe\"", 1, 0},
				{"\n  R\"qwe(hello)qwe\"", 1, 2},
				{"bla bla{child} bla{}\n  R\"qwe(hello)qwe\"", 1, 2},
				{"bla bla{child} \"bla\n bla\" bla bla{child}R\"qwe(hello)qwe\" bla bla", 1, 20},
				{"bla bla{child} R\"qwe(bla\n bla)qwe\" bla bla{child}R\"qwe(hello)qwe\" bla bla", 1, 24},
			},
			[](const auto& p){
				treeml::parser parser;

				struct listener : public treeml::listener{
					bool string_parsed = false;
					text_info info = {0, 0};
					void on_string_parsed(std::string_view str, const text_info& info)override{
						if(str == "hello"){
							this->string_parsed = true;
							this->info = info;
						}
					}

					void on_children_parse_started()override{}
					void on_children_parse_finished()override{}
				} l;

				parser.parse_data_chunk(std::get<0>(p), l);
				parser.end_of_data(l);

				tst::check(l.string_parsed, SL);
				tst::check_eq(l.info.line, std::get<1>(p), SL);
				tst::check_eq(l.info.line_offset, std::get<2>(p), SL);
			}
		);
});
}