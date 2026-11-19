/*
The MIT License (MIT)

Copyright (c) 2012-2025 Ivan Gagis <igagis@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/* ================ LICENSE END ================ */

#include "tree.hpp"

#include <algorithm>
#include <cinttypes>
#include <cstring>
#include <stack>
#include <tuple>

#include <fsif/span_file.hpp>
#include <fsif/vector_file.hpp>
#include <utki/string.hpp>

#include "parser.hpp"

using namespace tml;

forest tml::read(const fsif::file& fi)
{
	class the_listener : public tml::listener
	{
		std::stack<forest> stack;

	public:
		forest cur_forest;

		void on_children_parse_started(location) override
		{
			this->stack.push(std::move(this->cur_forest));
			utki::assert(this->cur_forest.size() == 0, SL);
		}

		void on_children_parse_finished(location loc) override
		{
			if (this->stack.size() == 0) {
				std::stringstream ss;
				ss << "malformed tml: unopened curly brace encountered at ";
				ss << loc.line << ":" << loc.offset;
				throw std::invalid_argument(ss.str());
			}
			this->stack.top().back().children = std::move(this->cur_forest);
			this->cur_forest = std::move(this->stack.top());
			this->stack.pop();
		}

		void on_string_parsed(std::string_view str, const extra_info&) override
		{
			this->cur_forest.emplace_back(str);
		}
	} listener;

	tml::parse(fi, listener);

	return std::move(listener.cur_forest);
}

forest tml::read(std::string_view str)
{
	const fsif::span_file fi(to_uint8_t(utki::make_span(str)));

	return read(fi);
}

namespace {
enum class tml_representation {
	unquoted,
	quoted,
	raw
};

// select the shortest textual presentation of the string in a single pass,
// returning {representation, raw_delimiter_length}:
// - representation: unquoted, if no escapes are needed at all
//   (non-empty, no whitespace, no " { } #, no characters that require
//   escaping in a quoted string); otherwise whichever is shorter: a raw
//   string written with the minimal opening sequence, or a quoted string
// - raw_delimiter_length: the length of the raw string opening sequence to
//   use if representation is raw, 0 otherwise
std::tuple<tml_representation, size_t> get_best_tml_representation(std::string_view str)
{
	// the minimum number of quotes that make a string raw: a single quote
	// opens a quoted string, two quotes form an empty string, three or more
	// quotes open a raw string
	constexpr size_t min_raw_string_delimeter_length = 3;

	bool unquoted = !str.empty();
	size_t num_escapes = 0;
	size_t max_num_quotes_in_a_row = 0;
	size_t num_quotes_in_a_row = 0;

	for (const auto c : str) {
		if (c == '"') {
			++num_quotes_in_a_row;
			if (num_quotes_in_a_row > max_num_quotes_in_a_row) {
				max_num_quotes_in_a_row = num_quotes_in_a_row;
			}
		} else {
			num_quotes_in_a_row = 0;
		}

		switch (c) {
			case '\t':
			case '\n':
			case '\\':
			case '"':
				++num_escapes;
				unquoted = false;
				break;
			case ' ':
			case '\r':
			case '{':
			case '}':
			case '#':
				unquoted = false;
				break;
			default:
				break;
		}
	}

	if (unquoted) {
		return {tml_representation::unquoted, 0};
	}

	const size_t quoted_length = 2 + str.size() + num_escapes;

	// a string cannot be written as a raw string if it is empty or starts or
	// ends with a quote: the leading / trailing quote run of the content
	// would merge with the delimiters, so the parsed value would not match
	// the original
	size_t min_raw = 0;

	if (str.empty() == false && str.front() != '"' && str.back() != '"') {
		min_raw = std::max(min_raw_string_delimeter_length, max_num_quotes_in_a_row + 1);
	}

	if (min_raw != 0 && 2 * min_raw + str.size() < quoted_length) {
		return {tml_representation::raw, min_raw};
	}

	return {tml_representation::quoted, 0};
}

void make_escaped_string(
	std::string_view str, //
	utki::span<uint8_t> out
)
{
	auto p = out.begin();

	for (const auto& c : str) {
		utki::assert(p != out.end(), SL);

		switch (c) {
			case '\t':
				*p = '\\';
				p = std::next(p);
				utki::assert(p != out.end(), SL);
				*p = 't';
				break;
			case '\n':
				*p = '\\';
				p = std::next(p);
				utki::assert(p != out.end(), SL);
				*p = 'n';
				break;
			case '\\':
				*p = '\\';
				p = std::next(p);
				utki::assert(p != out.end(), SL);
				*p = '\\';
				break;
			case '"':
				*p = '\\';
				p = std::next(p);
				utki::assert(p != out.end(), SL);
				*p = '"';
				break;
			default:
				*p = c;
				break;
		}
		p = std::next(p);
	}
}

void write_internal(const tml::forest& roots, fsif::file& fi, formatting fmt, unsigned indentation)
{
	const std::array<uint8_t, 1> quote = {{'"'}};
	const std::array<uint8_t, 1> lcurly = {{'{'}};
	const std::array<uint8_t, 1> rcurly = {{'}'}};
	const std::array<uint8_t, 1> space = {{' '}};
	const std::array<uint8_t, 1> tab = {{'\t'}};
	const std::array<uint8_t, 1> new_line = {{'\n'}};

	// remembers the representation of the previous leaf node, used to decide
	// whether a separator space is needed between adjacent leaves in minimal formatting
	tml_representation prev_rep = tml_representation::unquoted;
	bool prev_was_leaf = false;

	for (auto& n : roots) {
		// indent
		if (fmt == formatting::normal) {
			for (unsigned i = 0; i != indentation; ++i) {
				fi.write(utki::make_span(tab));
			}
		}

		// write node value
		const auto& str = n.value.string;
		const size_t length = str.size();
		const auto [rep, raw_delimiter_length] = get_best_tml_representation(str);

		// in minimal formatting a space is needed between adjacent leaves when
		// they would otherwise merge:
		// - two unquoted strings merge into one token
		// - a raw string adjacent to a quoted or raw string produces a run of
		//   more quotes than the raw string's delimiter length
		if (fmt != formatting::normal && prev_was_leaf) {
			const bool cur_starts_with_quote = (rep == tml_representation::quoted || rep == tml_representation::raw);
			const bool prev_ends_with_quote =
				(prev_rep == tml_representation::quoted || prev_rep == tml_representation::raw);
			const bool need_space = (rep == tml_representation::unquoted && prev_rep == tml_representation::unquoted) ||
				(cur_starts_with_quote && prev_ends_with_quote &&
				 (rep == tml_representation::raw || prev_rep == tml_representation::raw));
			if (need_space) {
				fi.write(utki::make_span(space));
			}
		}

		if (rep == tml_representation::unquoted) {
			fi.write(utki::make_span(str.c_str(), length));

			if (n.children.size() == 0 && length == 1 && n.value[0] == 'R') {
				fi.write(utki::make_span(space));
			}
		} else if (rep == tml_representation::raw) {
			for (size_t i = 0; i != raw_delimiter_length; ++i) {
				fi.write(utki::make_span(quote));
			}
			fi.write(utki::make_span(str.c_str(), length));
			for (size_t i = 0; i != raw_delimiter_length; ++i) {
				fi.write(utki::make_span(quote));
			}
		} else {
			unsigned num_escapes = 0;

			for (const auto c : str) {
				if (c == '\t' || c == '\n' || c == '\\' || c == '"') {
					++num_escapes;
				}
			}

			fi.write(utki::make_span(quote));

			if (num_escapes == 0) {
				fi.write(utki::make_span(str.c_str(), length));
			} else {
				std::vector<uint8_t> buf(length + num_escapes);

				make_escaped_string(
					str, //
					utki::make_span(buf)
				);

				fi.write(utki::make_span(buf));
			}

			fi.write(utki::make_span(quote));
		}

		if (n.children.size() == 0) {
			if (fmt == formatting::normal) {
				fi.write(utki::make_span(new_line));
			}
			prev_was_leaf = true;
			prev_rep = rep;
			continue;
		} else {
			prev_was_leaf = false;
		}

		if (fmt != formatting::normal) {
			fi.write(utki::make_span(lcurly));

			write_internal(n.children, fi, fmt, 0);

			fi.write(utki::make_span(rcurly));
		} else {
			fi.write(utki::make_span(lcurly));

			if (n.children.size() == 1 && n.children[0].children.size() == 0) {
				// if only one child and that child has no children then write the only child on the same line
				write_internal(n.children, fi, formatting::minimal, 0);
			} else {
				fi.write(utki::make_span(new_line));
				write_internal(n.children, fi, fmt, indentation + 1);

				// indent
				for (unsigned i = 0; i != indentation; ++i) {
					fi.write(utki::make_span(tab));
				}
			}
			fi.write(utki::make_span(rcurly));
			fi.write(utki::make_span(new_line));
		}
	}
}
} // namespace

void tml::write(
	const tml::forest& wood, //
	fsif::file& fi,
	formatting fmt
)
{
	fsif::file::guard file_guard(
		fi, //
		fsif::mode::create
	);

	write_internal(wood, fi, fmt, 0);
}

leaf::leaf(bool value) :
	string(value ? "true" : "false")
{}

leaf::leaf(int value) :
	string(utki::to_string(value))
{}

leaf::leaf(unsigned char value, utki::integer_base conversion_base) :
	leaf((unsigned short int)value, conversion_base)
{}

leaf::leaf(unsigned short int value, utki::integer_base conversion_base) :
	leaf((unsigned int)value, conversion_base)
{}

leaf::leaf(unsigned int value, utki::integer_base conversion_base) :
	string(utki::to_string(value, conversion_base))
{}

leaf::leaf(signed long int value) :
	string(utki::to_string(value))
{}

leaf::leaf(unsigned long int value, utki::integer_base conversion_base) :
	string(utki::to_string(value, conversion_base))
{}

leaf::leaf(signed long long int value) :
	string(utki::to_string(value))
{}

leaf::leaf(unsigned long long int value, utki::integer_base conversion_base) :
	string(utki::to_string(value, conversion_base))
{}

leaf::leaf(float value) :
	string([](float value) -> std::string {
		// TODO: use ::to_string<float>() when to_chars<float>() becomes available

		constexpr auto max_length = 64;
		std::array<char, max_length> buf{};

		// NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
		int res = snprintf(buf.data(), buf.size(), "%.8G", double(value));

		if (res < 0 || res > int(buf.size())) {
			return {};
		} else {
			return {buf.data(), size_t(res)};
		}
	}(value))
{}

leaf::leaf(double value) :
	string([](double value) -> std::string {
		// TODO: use ::to_string<double>() when to_chars<double>() becomes available
		constexpr auto max_length = 64;
		std::array<char, max_length> buf{};

		// NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
		int res = snprintf(buf.data(), buf.size(), "%.17G", double(value));

		if (res < 0 || res > int(buf.size())) {
			return {};
		} else {
			return {buf.data(), size_t(res)};
		}
	}(value))
{}

leaf::leaf(long double value) :
	string([](long double value) -> std::string {
		// TODO: use ::to_string<long double>() when to_chars<long double>() becomes available
		constexpr auto max_length = 128;
		std::array<char, max_length> buf{};

		// NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
		int res = snprintf(buf.data(), buf.size(), "%.31LG", (long double)(value));

		if (res < 0 || res > int(buf.size())) {
			return {};
		} else {
			return {buf.data(), size_t(res)};
		}
	}(value))
{}

bool leaf::to_bool() const
{
	return this->string == "true";
}

std::string tml::to_string(const forest& f)
{
	fsif::vector_file fi;
	tml::write(f, fi, tml::formatting::minimal);
	auto v = fi.reset_data();
	return utki::make_string(v);
}
