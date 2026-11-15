#include <tst/check.hpp>
#include <tst/set.hpp>

#include "../../../src/tml/tree.hpp"

namespace {
const tst::set set("parser_correctness", [](tst::suite& suite) {
	suite.add<std::pair<const char*, const char*>>(
		"single_slash_is_parsed_as_node",
		// the test checks that 5th element is as expected
		{
			{        "a b c d e f g",  "f"},
			{        "a b c d e / g",  "/"},
			{       "a b c d e /\rg",  "/"},
			{       "a b c d e /\tg",  "/"},
			{       "a b c d e /\ng",  "/"},
			{       "a b c d e / /g",  "/"},
			{       "a b c d e /f g", "/f"},
			{     "a b c d e /{z} g",  "/"},
			{"a b c d e #{z} g\nf g",  "f"},
    },
		[](auto& p) {
			auto r = tml::read(p.first);

			tst::check_ge(r.size(), size_t(5), SL);

			tst::check_eq(r[5].value.string, std::string(p.second), SL);
		}
	);

	suite.add<std::string>(
		"comment_in_the_beginning_is_ignored",
		{
			"# bla bla"
			"\n"
			"# bla bla bla"
			"\n"
			"first"
			"\n"
			"second",

			"\n"
			"# bla bla"
			"\n"
			"# bla bla bla"
			"\n"
			"first"
			"\n"
			"second",

			"\n"
			" # bla bla"
			"\n"
			"# bla bla bla"
			"\n"
			"first"
			"\n"
			"second",

			"# bla bla bla"
			"\n"
			"# bla bla"
			"\n"
			"first"
			"\n"
			"second",

			"\n"
			"# bla bla bla"
			"\n"
			"# bla bla"
			"\n"
			"first"
			"\n"
			"second",

			"\n"
			" # bla bla bla"
			"\n"
			"# bla bla"
			"\n"
			"first"
			"\n"
			"second",
		},
		[](auto& p) {
			auto in = tml::read(p);

			tst::check_eq(in.size(), size_t(2), SL);
			tst::check_eq(in[0].value.string, std::string("first"), SL);
			tst::check_eq(in[1].value.string, std::string("second"), SL);
		}
	);

	suite.add("only_quotes_style_raw_string_in_the_document", []() {
		auto r = tml::read("\"\"\"hello\"\"\"");
		tst::check_eq(r.size(), size_t(1), SL);
		tst::check_eq(r.front().children.size(), size_t(0), SL);
		tst::check_eq(r.front().value.string, std::string("hello"), SL);
	});

	suite.add<std::pair<std::string, tml::forest>>(
		"parsed_tree_is_as_expected",
		{
			// quotes-style raw strings: N-quote delimiters (N >= 3), values are byte-exact
			{								  "\"\"\"hello\"\"\"",{{"hello"}}																   }, // 0
			{								 " \"\"\"hello\"\"\"",                       {{"hello"}}},
			{							   "\n\"\"\"hello\"\"\" ",                       {{"hello"}}},
			{							   "\t\"\"\"hello\"\"\" ",                       {{"hello"}}},
			{							  "pre\"\"\"hello\"\"\" ",              {{"pre"}, {"hello"}}},
			{					 "pre{}\"\"\"hello\"\"\"{child} ", {{"pre"}, {"hello", {{"child"}}}}}, // 5
			{				   "pre{} \"\"\"hello\"\"\" {child} ", {{"pre"}, {"hello", {{"child"}}}}},
			{				  "pre{} \"\"\"hello\"\"\"\n{child} ", {{"pre"}, {"hello", {{"child"}}}}},
			{				  "pre{} \"\"\"hello\"\"\"\t{child} ", {{"pre"}, {"hello", {{"child"}}}}},
			{						  "\"pre\"\"\"\"hello\"\"\" ",              {{"pre"}, {"hello"}}},
			{						 "\"pre\" \"\"\"hello\"\"\" ",              {{"pre"}, {"hello"}}}, // 10
			{							"\"\" \"\"\"hello\"\"\" ",                 {{""}, {"hello"}}},
			{								 "\"\"\"hello \"\"\"",                      {{"hello "}}},
			{							  "\"\"\"he\"\"llo\"\"\"",                   {{"he\"\"llo"}}},
			{						   "\"\"\"hello\nworld\"\"\"",                {{"hello\nworld"}}}, // 15

			// raw string values are byte-exact: a leading/trailing newline is NOT stripped
			{								"\"\"\"\nhello\"\"\"",                     {{"\nhello"}}},
			{								"\"\"\"hello\n\"\"\"",                     {{"hello\n"}}},
			{							  "\"\"\"h\nello\n\"\"\"",                   {{"h\nello\n"}}},
			{							  "\"\"\"\nhello\n\"\"\"",                   {{"\nhello\n"}}},
			{								  "\"\"\" \n\t\"\"\"",                       {{" \n\t"}}}, // 20
			{							  "\"\"\"\r\nhello\"\"\"",                   {{"\r\nhello"}}},
			{							  "\"\"\"hello\r\n\"\"\"",                   {{"hello\r\n"}}},
			{							 "\"\"\"\r \nhello\"\"\"",                  {{"\r \nhello"}}},
			{							 "\"\"\"hello\r \n\"\"\"",                  {{"hello\r \n"}}},
			{				  "\"\"\"\r\n\r\nhello\r\n\r\n\"\"\"",       {{"\r\n\r\nhello\r\n\r\n"}}}, // 25

			// a raw string opened with a longer (5-quote) delimiter may hold a run of 3 quotes in its content
			{					   "\"\"\"\"\"a\"\"\"b\"\"\"\"\"",                    {{"a\"\"\"b"}}},
			{							"\"\"\"\"\"abc\"\"\"\"\"",                         {{"abc"}}},
			{								   "\"\"\"a\"b\"\"\"",                        {{"a\"b"}}},
			{								 "\"\"\"a\"\"b\"\"\"",                      {{"a\"\"b"}}},
			{				"\"\"\"hello\"\"\" \"\"\"world\"\"\"",            {{"hello"}, {"world"}}}, // 30

			// / and // are ordinary characters in unquoted strings (URLs, slashes)
			{											"hello//",                     {{"hello//"}}},
			{										  "hello//\n",                     {{"hello//"}}}, // 40
			{									 "hello//bla bla",         {{"hello//bla"}, {"bla"}}},
			{								   "hello//bla bla\n",         {{"hello//bla"}, {"bla"}}},
			{										  "hello/**/",                   {{"hello/**/"}}},
			{								   "hello/*bla bla*/",       {{"hello/*bla"}, {"bla*/"}}},
			{"url = http://my-site.com/path/index.html (homepage)",
			 {{"url"}, {"="}, {"http://my-site.com/path/index.html"}, {"(homepage)"}}                }, // 45

			// # line comments: unconditional, run to end of line, / is data
			{							 "# a whole line comment",                                {}},
			{							  "a # inline comment\nb",                    {{"a"}, {"b"}}},
			{				 "a# inline comment without space\nb",                    {{"a"}, {"b"}}},
			{											"abc#def",						 {{"abc"}}}, // bare # in an unquoted value starts a comment
			{											"\"a#b\"",						 {{"a#b"}}}, // # inside a quoted string is data

			// parsing empty document
			{												   "",								{}},

			// unquoted string escape sequences
			{									  "hello\\ world",                 {{"hello world"}}},
			{									"hello\\\\ world",          {{"hello\\"}, {"world"}}},
			{									 "hello\\\nworld",          {{"hello\\"}, {"world"}}}, // in unquoted string the new line terminates the string
			{									  "hello\\nworld",                {{"hello\nworld"}}},
			{									  "hello\\tworld",                {{"hello\tworld"}}},
			{						  "\\\"hello\\\" \\{world\\}",      {{"\"hello\""}, {"{world}"}}},
			{								  "hello {\\\"world}",        {{"hello", {{"\"world"}}}}},
			{									   "hello\\u0bf5",                 {{"hello\u0bf5"}}},
			{								   "hello\\U00026218",             {{"hello\U00026218"}}},
			{								    "hello\\rworld",  {{"hello\rworld"}}},  // \r escape is recognised in unquoted strings

			// quoted string escape sequences
			{								  "\"hello\\ world\"",               {{"hello\\ world"}}},

			{								 "\"hello\\\nworld\"",                {{"hello\\world"}}}, // in quoted strings the new line is ignored
			{								  "\"hello\\nworld\"",                {{"hello\nworld"}}},
			{								  "\"hello\\tworld\"",                {{"hello\tworld"}}},
			{							  "hello {\"\\\"world\"}",        {{"hello", {{"\"world"}}}}},
			{				  "\"\\\"hello\\\"\" \"\\{world\\}\"",  {{"\"hello\""}, {"\\{world\\}"}}},
			{								   "\"hello\\u0bf5\"",                 {{"hello\u0bf5"}}},
			{							   "\"hello\\U00026218\"",             {{"hello\U00026218"}}},
			{								    "\"hello\\rworld\"",  {{"hello\rworld"}}},  // \r escape is recognised in quoted strings
			{								    "\"hello\nworld\"",  {{"hello\nworld"}}},  // byte-exact: a literal new line typed between the quotes is kept
			{								    "\"hello\tworld\"",  {{"hello\tworld"}}},  // byte-exact: a literal tab typed between the quotes is kept
			{								    "\"hello\rworld\"",  {{"hello\rworld"}}},  // byte-exact: a literal carriage return typed between the quotes is kept
			{								    "\"hello\r\nworld\"",  {{"hello\r\nworld"}}},  // byte-exact: a literal CRLF typed between the quotes is kept
    },
		[](auto& p) {
			auto r = tml::read(p.first);
			tst::check_eq(r, p.second, SL);
		}
	);

	suite.add<std::string_view>(
		"malformed_document_should_throw_exception",
		{"{}",
		 "{",
		 "}",
		 " {qw}",
		 "asdf{{}}",
		 "afd{}{}",
		 // raw strings: a run of more than N consecutive quotes is malformed
		 "\"\"\"abc\"\"\"\"",
		 "\"\"\"\"\"abc\"\"\"",
		 "\"\"\"\"\"\"",

		 // unicode escape sequences must be fully consumed (all 4/8 hex digits valid), otherwise malformed
		 // NOLINTNEXTLINE(bugprone-suspicious-missing-comma, "intentional non-hex digit in unicode escape")
		 "\\u12G4"  // a non-hex digit before the end is malformed,
		 "\\U000000G1"  // long unicode escape with a trailing non-hex digit is malformed,
		 "\"\\u12 4\""  // a trailing space inside the sequence is malformed,
		},
		[](const auto& p) {
			bool thrown = false;
			try {
				tml::read(std::string(p));
				tst::check(
					false,
					[&](auto& o) {
						o << "exception is unexpectedly not thrown";
					},
					SL
				);
			} catch (const std::invalid_argument&) {
				thrown = true;
			}
			tst::check(thrown, SL);
		}
	);
});
} // namespace
