#include <tst/set.hpp>
#include <tst/check.hpp>

#include "../../../src/treeml/parser.hpp"

namespace{
tst::set set1("parser_flags", [](tst::suite& suite){
	suite.add<std::string_view>(
			"flag_space_should_be_false",
			{
				"hello", // 0
				" hello",
				"hello post",
				"pre\"hello\"",
				"\"pre\"hello",
				"\"\"{}hello", // 5
				" pre {}hello", // space between string and children list does not count
				" pre {\n}hello",
				" pre {child}hello",
				" pre {child }hello",
				"pre{child1 child2}hello post", // 10
				"R\"qwe(raw string)qwe\"hello",
				"R\"qwe(hello)qwe\"",
				"pre {}R\"qwe(hello)qwe\"",
				"pre{hello child2}",
				"pre{ hello child2}", // 15
				"\"\"{hello child2}",
				"\"\"{} \"\"{hello child2}",
				"pre{hello child2}",
				"pre {hello child2}",
				"pre /{hello child2}", // 20
				"\"\"{ hello child2}",
				"\"\"{} \"\"{ hello child2}",
				"pre{ hello child2}",
				"pre { hello child2}",
				"pre /{ hello child2}", // 25
				"pre{}/*bla bla*/hello",
				"pre/*bla bla*/\"hello\"",
				"\"pre\"/*bla bla*/hello",
				"\"pre\"/*bla bla*/\"hello\"",
			},
			[](const auto& p){
				tml::parser parser;

				struct listener : public tml::listener{
					bool string_parsed = false;
					void on_string_parsed(std::string_view str, const tml::extra_info& info)override{
						if(str == "hello"){
							this->string_parsed = true;
							tst::check(!info.flags.get(tml::flag::space), SL);
						}
					}

					void on_children_parse_started(tml::location)override{}
					void on_children_parse_finished(tml::location)override{}
				} l;

				parser.parse_data_chunk(p, l);
				parser.end_of_data(l);

				tst::check(l.string_parsed, SL);
			}
		);
	
	suite.add<std::string_view>(
			"flag_space_should_be_true",
			{
				"pre hello",
				"pre\nhello",
				"pre\thello",
				"pre hello post",
				"pre\nhello post",
				"pre\thello post",
				"pre \"hello\"",
				"pre\n \"hello\"",
				"pre\t\"hello\"",
				"\"pre\" hello",
				"\"pre\"\nhello",
				"\"pre\"\thello",
				"\"\"{} hello",
				"\"\"{}\nhello",
				"\"\"{}\thello",
				"pre{child1 child2} hello post",
				"pre{child1 child2}\nhello post",
				"pre{child1 child2}\thello post",
				"R\"qwe(raw string)qwe\" hello",
				"R\"qwe(raw string)qwe\"\nhello",
				"R\"qwe(raw string)qwe\"\thello",
				"pre R\"qwe(hello)qwe\"",
				"pre\nR\"qwe(hello)qwe\"",
				"pre\tR\"qwe(hello)qwe\"",
				"pre{pre hello child2}",
				"pre{pre\nhello child2}",
				"pre{pre\thello child2}",
				"pre//bla bla\n hello",
				"pre//bla bla\n \"hello\"",
				"pre/*bla bla*/\nhello",
				"pre/*bla bla*/\n\"hello\"",
				"pre\n/*bla bla*/hello",
				"pre\n/*bla bla*/\"hello\"",
				"\"pre\"/*bla bla*/\nhello",
				"\"pre\"/*bla bla*/\n\"hello\"",
				"\"pre\"\n/*bla bla*/hello",
				"\"pre\"\n/*bla bla*/\"hello\"",
				"pre//bla bla\nhello",
				"pre//bla bla\n\"hello\"",
			},
			[](const auto& p){
				tml::parser parser;

				struct listener : public tml::listener{
					bool string_parsed = false;
					void on_string_parsed(std::string_view str, const tml::extra_info& info)override{
						if(str == "hello"){
							this->string_parsed = true;
							tst::check(info.flags.get(tml::flag::space), SL);
						}
					}

					void on_children_parse_started(tml::location)override{}
					void on_children_parse_finished(tml::location)override{}
				} l;

				parser.parse_data_chunk(p, l);
				parser.end_of_data(l);

				tst::check(l.string_parsed, SL);
			}
		);
	
	suite.add<std::string_view>(
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
				tml::parser parser;

				struct listener : public tml::listener{
					bool string_parsed = false;
					void on_string_parsed(std::string_view str, const tml::extra_info& info)override{
						if(str == "hello"){
							this->string_parsed = true;
							tst::check(!info.flags.get(tml::flag::quoted), SL);
						}
					}

					void on_children_parse_started(tml::location)override{}
					void on_children_parse_finished(tml::location)override{}
				} l;

				parser.parse_data_chunk(p, l);
				parser.end_of_data(l);

				tst::check(l.string_parsed, SL);
			}
		);
	
	suite.add<std::string_view>(
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
				tml::parser parser;

				struct listener : public tml::listener{
					bool string_parsed = false;
					void on_string_parsed(std::string_view str, const tml::extra_info& info)override{
						if(str == "hello"){
							this->string_parsed = true;
							tst::check(info.flags.get(tml::flag::quoted), SL);
						}
					}

					void on_children_parse_started(tml::location)override{}
					void on_children_parse_finished(tml::location)override{}
				} l;

				parser.parse_data_chunk(p, l);
				parser.end_of_data(l);

				tst::check(l.string_parsed, SL);
			}
		);

		suite.add<std::string_view>(
			"flag_first_on_line_should_be_false",
			{
				"hi hello", // #0
				"hi \"hello\"",
				"hi R\"(hello)\"",
				"hi \"\"\"hello\"\"\"",
				"\nhi hello",
				"\nhi \"hello\"", // #5
				"\nhi R\"(hello)\"",
				"\nhi \"\"\"hello\"\"\"",
				"pre hello",
				"pre \"hello\"",
				"pre R\"(hello)\"", // #10
				"pre \"\"\"hello\"\"\"",
				"pre hello\n",
				"pre \"hello\"\n",
				"\npre\"hello\"",
				"\npre\"\"\"hello\"\"\"", // #15
				"qwe\n{bla bla}hello",
				"qwe\n{bla bla}\"hello\"",
				"qwe\n{bla bla}R\"(hello)\"",
				"qwe\n{bla bla}\"\"\"hello\"\"\"",
				"qwe\n{}hello", // #20
				"qwe\n{}\"hello\"",
				"qwe\n{}R\"(hello)\"",
				"qwe\n{}\"\"\"hello\"\"\"",
				"qwe\n{} hello",
				"qwe\n{} \"hello\"", // #25
				"qwe\n{} R\"(hello)\"",
				"qwe\n{} \"\"\"hello\"\"\"",
				"qwe\n{}\thello",
				"qwe\n{}\t\"hello\"",
				"qwe\n{}\tR\"(hello)\"", // #30
				"qwe\n{}\t\"\"\"hello\"\"\"",
				"hi{}hello\n",
				"hi{}\"hello\"\n",
				"hi{}R\"(hello)\"\n",
				"hi{}\"\"\"hello\"\"\"\n", // #35
				"hi{\n bye hello}",
				"hi{\n bye \"hello\"}",
				"hi{\n bye R\"(hello)\"}",
				"hi{\n bye \"\"\"hello\"\"\"}",
				"hi\n{hello}",
				"hi\n{ hello}",
				"hi\n { hello}",
				"hi\n{\thello}",
				"hi\n\t{hello}",
				"hi\n{\"hello\"}",
			},
			[](const auto& p){
				tml::parser parser;

				struct listener : public tml::listener{
					bool string_parsed = false;
					void on_string_parsed(std::string_view str, const tml::extra_info& info)override{
						if(str == "hello"){
							this->string_parsed = true;
							tst::check(!info.flags.get(tml::flag::first_on_line), SL);
						}
					}

					void on_children_parse_started(tml::location)override{}
					void on_children_parse_finished(tml::location)override{}
				} l;

				parser.parse_data_chunk(p, l);
				parser.end_of_data(l);

				tst::check(l.string_parsed, SL);
			}
		);
	
	suite.add<std::string_view>(
			"flag_first_on_line_should_be_true",
			{
				"hello",
				"R\"(hello)\"",
				"\"\"\"hello\"\"\"",
				" R\"(hello)\"",
				" \"\"\"hello\"\"\"",
				"\thello",
				"\tR\"(hello)\"",
				"\t\"\"\"hello\"\"\"",
				"\nhello",
				"\n\"hello\"",
				"\nR\"(hello)\"",
				"\n\"\"\"hello\"\"\"",
				"\n hello",
				"\n \"hello\"",
				"\n R\"(hello)\"",
				"\n \"\"\"hello\"\"\"",
				" hello post",
				" \"hello\" \"post\"",
				" R\"(hello)\" post",
				" \"\"\"hello\"\"\" post",
				"pre\nhello",
				"pre\n\"hello\"",
				"pre\nR\"(hello)\"",
				"pre\n\"\"\"hello\"\"\"",
				"pre\n hello",
				"pre\n \"hello\"",
				"pre\n R\"(hello)\"",
				"pre\n \"\"\"hello\"\"\"",
				"pre \nhello",
				"pre \n\"hello\"",
				"pre \nR\"(hello)\"",
				"pre \n\"\"\"hello\"\"\"",
				"pre\t\nhello",
				"pre\t\n\"hello\"",
				"pre\t\nR\"(hello)\"",
				"pre\t\n\"\"\"hello\"\"\"",
				"pre\t\n\thello",
				"pre\t\n\t\"hello\"",
				"pre\t\n\tR\"(hello)\"",
				"pre\t\n\t\"\"\"hello\"\"\"",
				"\"pre\"\nhello",
				"\"pre\"\n hello",
				"\"pre\"\n\"hello\"",
				"\"pre\"\n \"hello\"",
				"hi{}\nhello",
				"hi{}\n\"hello\"",
				"hi{}\nR\"(hello)\"",
				"hi{}\n\"\"\"hello\"\"\"",
				"hi{bye}\nhello",
				"hi{bye}\n\"hello\"",
				"hi{bye}\nR\"(hello)\"",
				"hi{bye}\n\"\"\"hello\"\"\"",
				"hi{} \nhello",
				"hi{} \n\"hello\"",
				"hi{} \nR\"(hello)\"",
				"hi{} \n\"\"\"hello\"\"\"",
				"hi{}\t\nhello",
				"hi{}\t\n\"hello\"",
				"hi{}\t\nR\"(hello)\"",
				"hi{}\t\n\"\"\"hello\"\"\"",
				"hi{}\n hello",
				"hi{}\n \"hello\"",
				"hi{}\n R\"(hello)\"",
				"hi{}\n \"\"\"hello\"\"\"",
				"hi{\nhello}",
				"hi{\n\"hello\"}",
				"hi{\nR\"(hello)\"}",
				"hi{\n\"\"\"hello\"\"\"}",
				"hi{\n hello}",
				"hi{\n\thello}",
				"hi{\n \"hello\"}",
				"hi{\n R\"(hello)\"}",
				"hi{\n \"\"\"hello\"\"\"}",
				"hi{bye\nhello}",
				"hi{bye\n\"hello\"}",
				"hi{bye\nR\"(hello)\"}",
				"hi{bye\n\"\"\"hello\"\"\"}",
			},
			[](const auto& p){
				tml::parser parser;

				struct listener : public tml::listener{
					bool string_parsed = false;
					void on_string_parsed(std::string_view str, const tml::extra_info& info)override{
						if(str == "hello"){
							this->string_parsed = true;
							tst::check(info.flags.get(tml::flag::first_on_line), SL);
						}
					}

					void on_children_parse_started(tml::location)override{}
					void on_children_parse_finished(tml::location)override{}
				} l;

				parser.parse_data_chunk(p, l);
				parser.end_of_data(l);

				tst::check(l.string_parsed, SL);
			}
		);

	suite.add<std::string_view>(
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
				tml::parser parser;

				struct listener : public tml::listener{
					bool string_parsed = false;
					void on_string_parsed(std::string_view str, const tml::extra_info& info)override{
						if(str == "hello"){
							this->string_parsed = true;
							tst::check(!info.flags.get(tml::flag::raw), SL);
						}
					}

					void on_children_parse_started(tml::location)override{}
					void on_children_parse_finished(tml::location)override{}
				} l;

				parser.parse_data_chunk(p, l);
				parser.end_of_data(l);

				tst::check(l.string_parsed, SL);
			}
		);
	
	suite.add<std::string_view>(
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
				tml::parser parser;

				struct listener : public tml::listener{
					bool string_parsed = false;
					void on_string_parsed(std::string_view str, const tml::extra_info& info)override{
						if(str == "hello"){
							this->string_parsed = true;
							tst::check(info.flags.get(tml::flag::raw), SL);
							tst::check(!info.flags.get(tml::flag::raw_python_style), SL);
						}
					}

					void on_children_parse_started(tml::location)override{}
					void on_children_parse_finished(tml::location)override{}
				} l;

				parser.parse_data_chunk(p, l);
				parser.end_of_data(l);

				tst::check(l.string_parsed, SL);
			}
		);

	// test that failed C++ style raw string sequence works as expected
	suite.add<std::string_view>(
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
				tml::parser parser;

				struct listener : public tml::listener{
					bool string_parsed = false;
					void on_string_parsed(std::string_view str, const tml::extra_info& info)override{
						if(str == "R"){
							this->string_parsed = true;
							tst::check(!info.flags.get(tml::flag::quoted), SL);
							tst::check(!info.flags.get(tml::flag::raw), SL);
						}
					}

					void on_children_parse_started(tml::location)override{}
					void on_children_parse_finished(tml::location)override{}
				} l;

				parser.parse_data_chunk(p, l);
				parser.end_of_data(l);

				tst::check(l.string_parsed, SL);
			}
		);
	
	suite.add<std::string_view>(
			"space_flag_should_be_false_for_r",
			{
				"R\"hello\"",
				" R\"hello\"", // space flag is false because first on line
				"\rR\"hello\"",
				"\nR\"hello\"",
				"\tR\"hello\"",
				"R\"hello\" ",
				"pre{}R\"hello\" ",
				"pre { child}R\"hello\" post",
				"pre { child }R\"hello\" post",
				"pre { }R\"hello\" post",
				"pre{\n}R\"hello\"\npost",
				"pre{\t}R\"hello\"\tpost",
			},
			[](const auto& p){
				tml::parser parser;

				struct listener : public tml::listener{
					bool string_parsed = false;
					void on_string_parsed(std::string_view str, const tml::extra_info& info)override{
						if(str == "R"){
							this->string_parsed = true;
							tst::check(!info.flags.get(tml::flag::space), SL);
						}
					}

					void on_children_parse_started(tml::location)override{}
					void on_children_parse_finished(tml::location)override{}
				} l;

				parser.parse_data_chunk(p, l);
				parser.end_of_data(l);

				tst::check(l.string_parsed, SL);
			}
		);
	
	suite.add<std::string_view>(
			"space_flag_should_be_true_for_cpp_raw_string",
			{
				"pre R\"hello\"",
				"pre R\"hello\" ",
				"pre R\"hello\" post",
				"pre\nR\"hello\"\npost",
				"pre\tR\"hello\"\tpost",
				"pre{} R\"hello\" post",
				"pre {\n} R\"hello\" post",
				"\"\"{} R\"hello\" post",
				"\"\"\n{} R\"hello\" post",
			},
			[](const auto& p){
				tml::parser parser;

				struct listener : public tml::listener{
					bool string_parsed = false;
					void on_string_parsed(std::string_view str, const tml::extra_info& info)override{
						if(str == "R"){
							this->string_parsed = true;
							tst::check(info.flags.get(tml::flag::space), SL);
						}
					}

					void on_children_parse_started(tml::location)override{}
					void on_children_parse_finished(tml::location)override{}
				} l;

				parser.parse_data_chunk(p, l);
				parser.end_of_data(l);

				tst::check(l.string_parsed, SL);
			}
		);

	suite.add<std::string_view>(
		"curly_braces_flag_should_be_true",
		{
			"hello {}",
			"hello{}",
			"hello\n{}",
			"hello \n{}",
			"hello \n {}",
			"\"hello\"{}",
			"\"hello\" {}",
			" \"hello\"{}",
			" \"hello\" {}",
			"hi \"hello\"{}",
			"hi \"hello\" {}"
			"\"\"\"hello\"\"\"{}",
			"\"\"\"hello\"\"\" {}",
			"R\"(hello)\"{}",
			"R\"(hello)\" {}",
			
		},
		[](const auto& p){
			tml::parser parser;

				struct listener : public tml::listener{
					bool string_parsed = false;
					void on_string_parsed(std::string_view str, const tml::extra_info& info)override{
						if(str == "hello"){
							this->string_parsed = true;
							tst::check(info.flags.get(tml::flag::curly_braces), SL);
						}
					}

					void on_children_parse_started(tml::location)override{}
					void on_children_parse_finished(tml::location)override{}
				} l;

				parser.parse_data_chunk(p, l);
				parser.end_of_data(l);

				tst::check(l.string_parsed, SL);
		}
	);

	suite.add<std::string_view>(
		"curly_braces_flag_should_be_false",
		{
			"hi{hey} hello hi{}",
			"hi{hey}hello",
			"\"hi\"{hey}hello",
			"hi{hello}",
			"hi\n{hello}",
		},
		[](const auto& p){
			tml::parser parser;

				struct listener : public tml::listener{
					bool string_parsed = false;
					void on_string_parsed(std::string_view str, const tml::extra_info& info)override{
						if(str == "hello"){
							this->string_parsed = true;
							tst::check(!info.flags.get(tml::flag::curly_braces), SL);
						}
					}

					void on_children_parse_started(tml::location)override{}
					void on_children_parse_finished(tml::location)override{}
				} l;

				parser.parse_data_chunk(p, l);
				parser.end_of_data(l);

				tst::check(l.string_parsed, SL);
		}
	);
});
}

namespace{
tst::set set2("parser_location", [](tst::suite& suite){
	suite.add<std::tuple<std::string_view, size_t, size_t>>(
			"location",
			{
				{"hello", 1, 1},
				{" hello", 1, 2},
				{"\nhello", 2, 1},
				{"\n  hello", 2, 3},
				{"bla bla{child} bla{}\n  hello", 2, 3},
				{"bla bla{child} \"bla\n bla\" bla bla{child}hello", 2, 21},
				{"\"hello\"", 1, 1},
				{" \"hello\"", 1, 2},
				{"\n\"hello\"", 2, 1},
				{"\n  \"hello\"", 2, 3},
				{"\n   pre\"hello\"", 2, 7},
				{"\n   pre \"hello\"", 2, 8},
				{"bla bla{child} bla{}\n  \"hello\"", 2, 3},
				{"bla bla{child} \"bla\n bla\" bla bla{child}\"hello\" bla bla", 2, 21},
				{"R\"qwe(hello)qwe\"", 1, 1},
				{" R\"qwe(hello)qwe\"", 1, 2},
				{"\nR\"qwe(hello)qwe\"", 2, 1},
				{"\n  R\"qwe(hello)qwe\"", 2, 3},
				{"bla bla{child} bla{}\n  R\"qwe(hello)qwe\"", 2, 3},
				{"bla bla{child} \"bla\n bla\" bla bla{child}R\"qwe(hello)qwe\" bla bla", 2, 21},
				{"bla bla{child} R\"qwe(bla\n bla)qwe\" bla bla{child}R\"qwe(hello)qwe\" bla bla", 2, 25},
				{" R\"hello\"", 1, 3},
			},
			[](const auto& p){
				tml::parser parser;

				struct listener : public tml::listener{
					bool string_parsed = false;
					tml::extra_info info = {{0, 0}};
					void on_string_parsed(std::string_view str, const tml::extra_info& info)override{
						if(str == "hello"){
							this->string_parsed = true;
							this->info = info;
						}
					}

					void on_children_parse_started(tml::location)override{}
					void on_children_parse_finished(tml::location)override{}
				} l;

				parser.parse_data_chunk(std::get<0>(p), l);
				parser.end_of_data(l);

				tst::check(l.string_parsed, SL);
				tst::check_eq(l.info.location.line, std::get<1>(p), SL);
				tst::check_eq(l.info.location.offset, std::get<2>(p), SL);
			}
		);
});
}