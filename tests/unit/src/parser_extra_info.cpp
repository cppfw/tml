#include <tst/check.hpp>
#include <tst/set.hpp>

#include "../../../src/tml/parser.hpp"

namespace {
const tst::set set("parser_flags", [](tst::suite& suite) {
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
			"\"\"\"(raw string)\"\"\"hello",
			"\"\"\"(hello)\"\"\"",
			"pre {}\"\"\"(hello)\"\"\"",
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
			"pre{}hello",
			"pre{}\"hello\"",
			"\"pre\"{}hello",
			"\"pre\"{}\"hello\"",
		},
		[](const auto& p) {
			tml::parser parser;

			struct listener : public tml::listener {
				bool string_parsed = false;
				void on_string_parsed(std::string_view str, const tml::extra_info& info) override
				{
					if (str == "hello") {
						this->string_parsed = true;
						tst::check(!info.flags.get(tml::flag::space), SL);
					}
				}

				void on_children_parse_started(tml::location) override {}
				void on_children_parse_finished(tml::location) override {}
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
			"\"\"\"(raw string)\"\"\" hello",
			"\"\"\"(raw string)\"\"\"\nhello",
			"\"\"\"(raw string)\"\"\"\thello",
			"pre \"\"\"(hello)\"\"\"",
			"pre\n\"\"\"(hello)\"\"\"",
			"pre\t\"\"\"(hello)\"\"\"",
			"pre{pre hello child2}",
			"pre{pre\nhello child2}",
			"pre{pre\thello child2}",
			"pre//bla bla\n hello",
			"pre//bla bla\n \"hello\"",
			"pre/*bla bla*/\nhello",
			"pre/*bla bla*/\n\"hello\"",
			"pre\n#comment\nhello",
			"pre\n#comment\n\"hello\"",
			"\"pre\"/*bla bla*/\nhello",
			"\"pre\"/*bla bla*/\n\"hello\"",
			"\"pre\"\n#comment\nhello",
			"\"pre\"\n#comment\n\"hello\"",
			"pre//bla bla\nhello",
			"pre//bla bla\n\"hello\"",
		},
		[](const auto& p) {
			tml::parser parser;

			struct listener : public tml::listener {
				bool string_parsed = false;
				void on_string_parsed(std::string_view str, const tml::extra_info& info) override
				{
					if (str == "hello") {
						this->string_parsed = true;
						tst::check(info.flags.get(tml::flag::space), SL);
					}
				}

				void on_children_parse_started(tml::location) override {}
				void on_children_parse_finished(tml::location) override {}
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
			"\"\"\"(hello)\"\"\"",
			" \"\"\"(hello)\"\"\"post",
		},
		[](const auto& p) {
			tml::parser parser;

			struct listener : public tml::listener {
				bool string_parsed = false;
				void on_string_parsed(std::string_view str, const tml::extra_info& info) override
				{
					if (str == "hello") {
						this->string_parsed = true;
						tst::check(!info.flags.get(tml::flag::quoted), SL);
					}
				}

				void on_children_parse_started(tml::location) override {}
				void on_children_parse_finished(tml::location) override {}
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
		[](const auto& p) {
			tml::parser parser;

			struct listener : public tml::listener {
				bool string_parsed = false;
				void on_string_parsed(std::string_view str, const tml::extra_info& info) override
				{
					if (str == "hello") {
						this->string_parsed = true;
						tst::check(info.flags.get(tml::flag::quoted), SL);
					}
				}

				void on_children_parse_started(tml::location) override {}
				void on_children_parse_finished(tml::location) override {}
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
			"hi \"\"\"(hello)\"\"\"",
			"hi \"\"\"(hello)\"\"\"",
			"\nhi hello",
			"\nhi \"hello\"", // #5
			"\nhi \"\"\"(hello)\"\"\"",
			"\nhi \"\"\"(hello)\"\"\"",
			"pre hello",
			"pre \"hello\"",
			"pre \"\"\"(hello)\"\"\"", // #10
			"pre \"\"\"(hello)\"\"\"",
			"pre hello\n",
			"pre \"hello\"\n",
			"\npre\"hello\"",
			"\npre\"\"\"(hello)\"\"\"", // #15
			"qwe\n{bla bla}hello",
			"qwe\n{bla bla}\"hello\"",
			"qwe\n{bla bla}\"\"\"(hello)\"\"\"",
			"qwe\n{bla bla}\"\"\"(hello)\"\"\"",
			"qwe\n{}hello", // #20
			"qwe\n{}\"hello\"",
			"qwe\n{}\"\"\"(hello)\"\"\"",
			"qwe\n{}\"\"\"(hello)\"\"\"",
			"qwe\n{} hello",
			"qwe\n{} \"hello\"", // #25
			"qwe\n{} \"\"\"(hello)\"\"\"",
			"qwe\n{} \"\"\"(hello)\"\"\"",
			"qwe\n{}\thello",
			"qwe\n{}\t\"hello\"",
			"qwe\n{}\t\"\"\"(hello)\"\"\"", // #30
			"qwe\n{}\t\"\"\"(hello)\"\"\"",
			"hi{}hello\n",
			"hi{}\"hello\"\n",
			"hi{}\"\"\"(hello)\"\"\"\n",
			"hi{}\"\"\"(hello)\"\"\"\n", // #35
			"hi{\n bye hello}",
			"hi{\n bye \"hello\"}",
			"hi{\n bye \"\"\"(hello)\"\"\"}",
			"hi{\n bye \"\"\"(hello)\"\"\"}",
			"hi\n{hello}",
			"hi\n{ hello}",
			"hi\n { hello}",
			"hi\n{\thello}",
			"hi\n\t{hello}",
			"hi\n{\"hello\"}",
		},
		[](const auto& p) {
			tml::parser parser;

			struct listener : public tml::listener {
				bool string_parsed = false;
				void on_string_parsed(std::string_view str, const tml::extra_info& info) override
				{
					if (str == "hello") {
						this->string_parsed = true;
						tst::check(!info.flags.get(tml::flag::first_on_line), SL);
					}
				}

				void on_children_parse_started(tml::location) override {}
				void on_children_parse_finished(tml::location) override {}
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
			"\"\"\"(hello)\"\"\"",
			"\"\"\"(hello)\"\"\"",
			" \"\"\"(hello)\"\"\"",
			" \"\"\"(hello)\"\"\"",
			"\thello",
			"\t\"\"\"(hello)\"\"\"",
			"\t\"\"\"(hello)\"\"\"",
			"\nhello",
			"\n\"hello\"",
			"\n\"\"\"(hello)\"\"\"",
			"\n\"\"\"(hello)\"\"\"",
			"\n hello",
			"\n \"hello\"",
			"\n \"\"\"(hello)\"\"\"",
			"\n \"\"\"(hello)\"\"\"",
			" hello post",
			" \"hello\" \"post\"",
			" \"\"\"(hello)\"\"\" post",
			" \"\"\"(hello)\"\"\" post",
			"pre\nhello",
			"pre\n\"hello\"",
			"pre\n\"\"\"(hello)\"\"\"",
			"pre\n\"\"\"(hello)\"\"\"",
			"pre\n hello",
			"pre\n \"hello\"",
			"pre\n \"\"\"(hello)\"\"\"",
			"pre\n \"\"\"(hello)\"\"\"",
			"pre \nhello",
			"pre \n\"hello\"",
			"pre \n\"\"\"(hello)\"\"\"",
			"pre \n\"\"\"(hello)\"\"\"",
			"pre\t\nhello",
			"pre\t\n\"hello\"",
			"pre\t\n\"\"\"(hello)\"\"\"",
			"pre\t\n\"\"\"(hello)\"\"\"",
			"pre\t\n\thello",
			"pre\t\n\t\"hello\"",
			"pre\t\n\t\"\"\"(hello)\"\"\"",
			"pre\t\n\t\"\"\"(hello)\"\"\"",
			"\"pre\"\nhello",
			"\"pre\"\n hello",
			"\"pre\"\n\"hello\"",
			"\"pre\"\n \"hello\"",
			"hi{}\nhello",
			"hi{}\n\"hello\"",
			"hi{}\n\"\"\"(hello)\"\"\"",
			"hi{}\n\"\"\"(hello)\"\"\"",
			"hi{bye}\nhello",
			"hi{bye}\n\"hello\"",
			"hi{bye}\n\"\"\"(hello)\"\"\"",
			"hi{bye}\n\"\"\"(hello)\"\"\"",
			"hi{} \nhello",
			"hi{} \n\"hello\"",
			"hi{} \n\"\"\"(hello)\"\"\"",
			"hi{} \n\"\"\"(hello)\"\"\"",
			"hi{}\t\nhello",
			"hi{}\t\n\"hello\"",
			"hi{}\t\n\"\"\"(hello)\"\"\"",
			"hi{}\t\n\"\"\"(hello)\"\"\"",
			"hi{}\n hello",
			"hi{}\n \"hello\"",
			"hi{}\n \"\"\"(hello)\"\"\"",
			"hi{}\n \"\"\"(hello)\"\"\"",
			"hi{\nhello}",
			"hi{\n\"hello\"}",
			"hi{\n\"\"\"(hello)\"\"\"}",
			"hi{\n\"\"\"(hello)\"\"\"}",
			"hi{\n hello}",
			"hi{\n\thello}",
			"hi{\n \"hello\"}",
			"hi{\n \"\"\"(hello)\"\"\"}",
			"hi{\n \"\"\"(hello)\"\"\"}",
			"hi{bye\nhello}",
			"hi{bye\n\"hello\"}",
			"hi{bye\n\"\"\"(hello)\"\"\"}",
			"hi{bye\n\"\"\"(hello)\"\"\"}",
		},
		[](const auto& p) {
			tml::parser parser;

			struct listener : public tml::listener {
				bool string_parsed = false;
				void on_string_parsed(std::string_view str, const tml::extra_info& info) override
				{
					if (str == "hello") {
						this->string_parsed = true;
						tst::check(info.flags.get(tml::flag::first_on_line), SL);
					}
				}

				void on_children_parse_started(tml::location) override {}
				void on_children_parse_finished(tml::location) override {}
			} l;

			parser.parse_data_chunk(p, l);
			parser.end_of_data(l);

			tst::check(l.string_parsed, SL);
		}
	);

	suite.add<std::string_view>(
		"flag_raw_should_be_false",
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
		[](const auto& p) {
			tml::parser parser;

			struct listener : public tml::listener {
				bool string_parsed = false;
				void on_string_parsed(std::string_view str, const tml::extra_info& info) override
				{
					if (str == "hello") {
						this->string_parsed = true;
						tst::check(!info.flags.get(tml::flag::raw), SL);
					}
				}

				void on_children_parse_started(tml::location) override {}
				void on_children_parse_finished(tml::location) override {}
			} l;

			parser.parse_data_chunk(p, l);
			parser.end_of_data(l);

			tst::check(l.string_parsed, SL);
		}
	);

	suite.add<std::string_view>(
		"flag_raw_should_be_true",
		{
			"\"\"\"(hello)\"\"\"",
			" \"\"\"(hello)\"\"\"",
			"\"\"\"(hello)\"\"\" ",
			" \"\"\"(hello)\"\"\" ",
			"pre \"\"\"(hello)\"\"\" ",
			"pre \"\"\"(hello)\"\"\" post",
			"pre\n\"\"\"(hello)\"\"\"\npost",
			"pre\t\"\"\"(hello)\"\"\"\tpost",
		},
		[](const auto& p) {
			tml::parser parser;

			struct listener : public tml::listener {
				bool string_parsed = false;
				void on_string_parsed(std::string_view str, const tml::extra_info& info) override
				{
					if (str == "hello") {
						this->string_parsed = true;
						tst::check(info.flags.get(tml::flag::raw), SL);
					}
				}

				void on_children_parse_started(tml::location) override {}
				void on_children_parse_finished(tml::location) override {}
			} l;

			parser.parse_data_chunk(p, l);
			parser.end_of_data(l);

			tst::check(l.string_parsed, SL);
		}
	);

	// test that a single `R` followed by a quoted string works as expected
	suite.add<std::string_view>(
		"failed_raw_sequence_flags_should_be_false_for_r",
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
		[](const auto& p) {
			tml::parser parser;

			struct listener : public tml::listener {
				bool string_parsed = false;
				void on_string_parsed(std::string_view str, const tml::extra_info& info) override
				{
					if (str == "R") {
						this->string_parsed = true;
						tst::check(!info.flags.get(tml::flag::quoted), SL);
						tst::check(!info.flags.get(tml::flag::raw), SL);
					}
				}

				void on_children_parse_started(tml::location) override {}
				void on_children_parse_finished(tml::location) override {}
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
		[](const auto& p) {
			tml::parser parser;

			struct listener : public tml::listener {
				bool string_parsed = false;
				void on_string_parsed(std::string_view str, const tml::extra_info& info) override
				{
					if (str == "R") {
						this->string_parsed = true;
						tst::check(!info.flags.get(tml::flag::space), SL);
					}
				}

				void on_children_parse_started(tml::location) override {}
				void on_children_parse_finished(tml::location) override {}
			} l;

			parser.parse_data_chunk(p, l);
			parser.end_of_data(l);

			tst::check(l.string_parsed, SL);
		}
	);

	suite.add<std::string_view>(
		"space_flag_should_be_true_for_r",
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
		[](const auto& p) {
			tml::parser parser;

			struct listener : public tml::listener {
				bool string_parsed = false;
				void on_string_parsed(std::string_view str, const tml::extra_info& info) override
				{
					if (str == "R") {
						this->string_parsed = true;
						tst::check(info.flags.get(tml::flag::space), SL);
					}
				}

				void on_children_parse_started(tml::location) override {}
				void on_children_parse_finished(tml::location) override {}
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
			"hi \"hello\" {}",
			"\"\"\"(hello)\"\"\"{}",
			"\"\"\"(hello)\"\"\" {}",
			"\"\"\"(hello)\"\"\"{}",
			"\"\"\"(hello)\"\"\" {}",

		},
		[](const auto& p) {
			tml::parser parser;

			struct listener : public tml::listener {
				bool string_parsed = false;
				void on_string_parsed(std::string_view str, const tml::extra_info& info) override
				{
					if (str == "hello") {
						this->string_parsed = true;
						tst::check(info.flags.get(tml::flag::curly_braces), SL);
					}
				}

				void on_children_parse_started(tml::location) override {}
				void on_children_parse_finished(tml::location) override {}
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
		[](const auto& p) {
			tml::parser parser;

			struct listener : public tml::listener {
				bool string_parsed = false;
				void on_string_parsed(std::string_view str, const tml::extra_info& info) override
				{
					if (str == "hello") {
						this->string_parsed = true;
						tst::check(!info.flags.get(tml::flag::curly_braces), SL);
					}
				}

				void on_children_parse_started(tml::location) override {}
				void on_children_parse_finished(tml::location) override {}
			} l;

			parser.parse_data_chunk(p, l);
			parser.end_of_data(l);

			tst::check(l.string_parsed, SL);
		}
	);
});
} // namespace

namespace {
const tst::set set2("parser_location", [](tst::suite& suite) {
	suite.add<std::tuple<std::string_view, size_t, size_t>>(
		"location",
		{
			{																		"hello", 1,  1},
			{																	   " hello", 1,  2},
			{																	  "\nhello", 2,  1},
			{																	"\n  hello", 2,  3},
			{												"bla bla{child} bla{}\n  hello", 2,  3},
			{							 "bla bla{child} \"bla\n bla\" bla bla{child}hello", 2, 21},
			{																	"\"hello\"", 1,  1},
			{																   " \"hello\"", 1,  2},
			{																  "\n\"hello\"", 2,  1},
			{																"\n  \"hello\"", 2,  3},
			{															"\n   pre\"hello\"", 2,  7},
			{														   "\n   pre \"hello\"", 2,  8},
			{											"bla bla{child} bla{}\n  \"hello\"", 2,  3},
			{				 "bla bla{child} \"bla\n bla\" bla bla{child}\"hello\" bla bla", 2, 21},
			{															"\"\"\"(hello)\"\"\"", 1,  1},
			{														   " \"\"\"(hello)\"\"\"", 1,  2},
			{														  "\n\"\"\"(hello)\"\"\"", 2,  1},
			{														"\n  \"\"\"(hello)\"\"\"", 2,  3},
			{									"bla bla{child} bla{}\n  \"\"\"(hello)\"\"\"", 2,  3},
			{         "bla bla{child} \"bla\n bla\" bla bla{child}\"\"\"(hello)\"\"\" bla bla", 2, 21},
			{"bla bla{child} R\"qwe(bla\n bla)qwe\" bla bla{child}\"\"\"(hello)\"\"\" bla bla", 2, 25},
			{																  " R\"hello\"", 1,  3},
    },
		[](const auto& p) {
			tml::parser parser;

			struct listener : public tml::listener {
				bool string_parsed = false;
				tml::extra_info info = {
					.location = {.line = 0, .offset = 0}
				};
				void on_string_parsed(std::string_view str, const tml::extra_info& info) override
				{
					if (str == "hello") {
						this->string_parsed = true;
						this->info = info;
					}
				}

				void on_children_parse_started(tml::location) override {}
				void on_children_parse_finished(tml::location) override {}
			} l;

			parser.parse_data_chunk(std::get<0>(p), l);
			parser.end_of_data(l);

			tst::check(l.string_parsed, SL);
			tst::check_eq(l.info.location.line, std::get<1>(p), SL);
			tst::check_eq(l.info.location.offset, std::get<2>(p), SL);
		}
	);
});
} // namespace