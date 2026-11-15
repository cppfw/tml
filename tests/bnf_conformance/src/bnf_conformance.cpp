#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include <bnf/tml_bnf.hpp>
#include <tml/tree.hpp>
#include <tst/check.hpp>
#include <tst/set.hpp>
#include <utki/debug.hpp>

namespace {

// Escape a value for unambiguous, canonical comparison.
std::string escape_value (const std::string& value)
{
	std::string s;
	for (char c : value) {
		switch (c) {
		case '\n': s += "\\n"; break;
		case '\t': s += "\\t"; break;
		case '\r': s += "\\r"; break;
		case '\\': s += "\\\\"; break;
		case '[': s += "\\["; break;
		case ']': s += "\\]"; break;
		default: s += c; break;
		}
	}
	return s;
}

std::string tree_to_string (const tml::tree& tree, int depth)
{
	std::string s (size_t(depth), '\t');
	s += "[";
	s += escape_value (tree.value.string);
	s += "]";
	for (const auto& child : tree.children) {
		s += "\n";
		s += tree_to_string (child, depth + 1);
	}
	return s;
}

std::string forest_to_string (const tml::forest& forest)
{
	std::string s;
	for (const auto& tree : forest) {
		s += tree_to_string (tree, 0);
		s += "\n";
	}
	return s;
}

struct parse_result {
	bool ok = false;
	std::string forest;  // canonical representation (when ok)
	std::string error;   // error message (when !ok)
};

parse_result parse_ref (std::string_view input)
{
	try {
		auto f = tml::read (input);
		return { true, forest_to_string (f), {} };
	} catch (const std::exception& e) {
		return { false, {}, e.what () };
	}
}

parse_result parse_bnf (std::string_view input)
{
	try {
		auto f = tml_bnf::parse (input);
		return { true, forest_to_string (f), {} };
	} catch (const std::exception& e) {
		return { false, {}, e.what () };
	}
}

struct case_t {
	std::string input;
	std::string desc;
};

}  // namespace

const tst::set set (
	"bnf_conformance",
	[](auto& suite) {
		suite.add (
			"compare",
			[]() {
				const std::vector<case_t> cases = {
					// --- Group 1: basic structure (should conform) ---
					{"", "empty document"},
					{"   \t  ", "whitespace only"},
					{"# comment only", "comment only"},
					{"abc", "single unquoted"},
					{"abc def", "two unquoted"},
					{"abc { def ghi }", "children block"},
					{"abc { }", "empty children block"},
					{"abc { def { ghi } jkl }", "nested children"},
					{"abc { def } ghi", "value after children"},
					{"# a\n# b\n", "multiple comments"},
					{"abc\n\ndef", "blank line between values"},
					{"abc\r\ndef", "crlf between values"},

					// --- Group 2: brace structure errors (both reject) ---
					{"{ def }", "lbrace at start"},
					{"abc }", "rbrace without lbrace"},
					{"abc { def", "unclosed block"},
					{"abc { def } }", "extra rbrace"},
					{"{", "lone lbrace"},
					{"}", "lone rbrace"},
					{"{abc", "lbrace then value, unclosed"},

					// --- Group 3: unquoted escapes (should conform) ---
					{"a\\ b", "escaped space"},
					{"a\\\\b", "escaped backslash"},
					{"a\\\"b", "escaped quote"},
					{"a\\{b", "escaped lbrace"},
					{"a\\}b", "escaped rbrace"},
					{"a\\nb", "escape newline"},
					{"a\\tb", "escape tab"},
					{"a\\rb", "escape cr"},
					{"a\\u0041b", "unicode 4"},
					{"a\\U00000042b", "unicode 8"},
					{"\\n", "unquoted value that is a newline escape"},

					// --- Group 4: unlisted escapes (DISCREPANCIES expected) ---
					{"a\\Xb", "unlisted escape in unquoted"},
					{"\\a", "unquoted value that is an unlisted escape"},
					{"a\\\nb", "backslash then literal newline in unquoted"},

					// --- Group 5: comments (should conform) ---
					{"abc#def", "unconditional # after value"},
					{"abc # def", "comment after space"},
					{"#abc", "comment at start"},
					{"a#b#c", "value then comment"},
					{"abc\n# comment\nxyz", "comment in middle"},

					// --- Group 6: quoted strings (should conform) ---
					{"\"abc\"", "quoted"},
					{"\"\"", "empty quoted"},
					{"\"a b\"", "literal space in quoted"},
					{"\"a\\nb\"", "escape newline in quoted"},
					{"\"a\\\"b\"", "escape quote in quoted"},
					{"\"a\\\\b\"", "escape backslash in quoted"},
					{"\"a\\tb\"", "escape tab in quoted"},
					{"\"a\\rb\"", "escape cr in quoted"},
					{"\"a b\n c\"", "literal newline in quoted (byte-exact)"},
					{"\"a#b\"", "literal # in quoted"},
					{"\"a{b}\"", "literal braces in quoted"},
					{"\"abc", "unterminated quoted"},

					// --- Group 7: quoted unlisted escapes (DISCREPANCIES expected) ---
					{"\"\\{\"", "unlisted escape lbrace in quoted"},
					{"\"\\}\"", "unlisted escape rbrace in quoted"},
					{"\"\\ \"", "unlisted escape space in quoted"},
					{"\"\\a\"", "unlisted escape a in quoted"},

					// --- Group 8: raw strings (should conform) ---
					{"\"\"\"abc\"\"\"", "raw N=3"},
					{"\"\"\"a\"b\"\"\"", "raw with inner single quote"},
					{"\"\"\"\"\"it has \"\" quotes\"\"\"\"\"", "raw N=5 with inner 2-quote run"},
					{"\"\"\"a\\nb\"\"\"", "raw with backslash-n (no escape processing)"},
					{"\"\"\"a\"\"b\"\"\"", "raw with 2-quote run in middle (N=3)"},
					{"\"\"\"a\nb\"\"\"", "raw with literal newline (byte-exact)"},

					// --- Group 9: raw string errors (both reject) ---
					{"\"\"\"abc", "unterminated raw"},
					{"\"\"\"abc\"\"\"\"", "raw with 4-quote run after content (N=3)"},

					// --- Group 10: quote runs at token boundary (should conform) ---
					{"abc\"def\"", "unquoted then quoted"},
					{"abc\"\"", "unquoted then empty quoted"},
					{"abc\"\"\"def\"\"\"", "unquoted then raw"},

					// --- Group 11: NUL byte terminates the document (should conform) ---
					// A NUL byte (\0) is the end-of-document marker: parsing stops
					// at the NUL byte, the current value (if any and complete) is
					// emitted, and any remaining input is ignored. An incomplete
					// value (e.g. an unterminated quoted or raw string) is
					// discarded.
					{std::string ("abc\0def", 7), "nul terminates document (mid)"},
					{std::string ("\0abc", 4), "nul terminates document (at start)"},
					{std::string ("a\0b\0c", 5), "nul terminates document (first nul)"},
					{std::string ("abc\0", 4), "nul terminates document (at end)"},
					{std::string ("\"ab\0c\"", 6), "nul in quoted: string discarded"},
					{std::string ("\"ab\0", 4), "nul then eof in quoted: discarded"},
					{std::string ("\"\"\"ab\0c\"\"\"", 10), "nul in raw: string discarded"},

					// --- Group 12: structural and unicode edges (should conform) ---
					{"a { b { c { d { e } } } }", "deep nesting"},
					{"a b c d e", "five values"},
					{"abc   \n  ", "trailing whitespace"},
					{std::string ("a\\u0000b"), "unicode 0 (nul char) in value"},
					{"\\uffff", "unicode ffff"},
					{"\\U00010fff", "unicode 10fff (supplementary)"},
					{"\\uZZZZ", "malformed unicode (both reject)"},
					{"a { b c } d { e f }", "two blocks after value"},
				};

				int total = 0;
				int mismatches = 0;
				int both_reject = 0;
				std::string report;
				for (const auto& c : cases) {
					++total;
					auto ref = parse_ref (c.input);
					auto bnf = parse_bnf (c.input);
					// The two parsers agree when: both reject, or both accept and
					// build the same forest. (Error-message wording may differ
					// when both reject; that is not a discrepancy.)
					bool agree;
					if (!ref.ok && !bnf.ok) {
						agree = true;
					} else if (ref.ok && bnf.ok) {
						agree = (ref.forest == bnf.forest);
					} else {
						agree = false;
					}
					if (agree && !ref.ok) {
						++both_reject;
						continue;
					}
					if (agree) {
						continue;
					}
					// genuine mismatch
					++mismatches;
					report += "DISCREPANCY [";
					report += c.desc;
					report += "]\n";
					report += "  input: '";
					report += c.input;
					report += "'\n";
					report += "  ref:  ";
					report += ref.ok ? ("ok   => " + ref.forest) : ("err  => " + ref.error);
					report += "\n  bnf:  ";
					report += bnf.ok ? ("ok   => " + bnf.forest) : ("err  => " + bnf.error);
					report += "\n\n";
				}

				if (mismatches != 0) {
					std::cerr << report;
				}
				utki::log_debug ([total, mismatches, both_reject] (auto& o) {
					o << "bnf conformance: " << total << " cases, " << mismatches
					    << " mismatch, " << both_reject << " both-reject\n";
				});
				tst::check (
					mismatches == 0,
					[report, mismatches, total] (auto& o) {
						o << mismatches << " / " << total << " mismatched cases\n";
						o << report;
					},
					SL
				);
			}
		);
	}
);
