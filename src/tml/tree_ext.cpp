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

#include "tree_ext.hpp"

#include <algorithm>
#include <stack>

#include <fsif/span_file.hpp>
#include <fsif/vector_file.hpp>
#include <utki/string.hpp>

#include "parser.hpp"

using namespace tml;

forest_ext tml::read_ext(const fsif::file& fi)
{
	class the_listener : public tml::listener
	{
		std::stack<forest_ext> stack;

	public:
		forest_ext cur_forest;

		void on_children_parse_started(location /* loc */) override
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

		void on_string_parsed(std::string_view str, const extra_info& info) override
		{
			this->cur_forest.emplace_back(leaf_ext(std::string(str.data(), str.size()), info));
		}
	} listener;

	tml::parse(fi, listener);

	return std::move(listener.cur_forest);
}

forest_ext tml::read_ext(const std::string& str)
{
	const fsif::span_file fi(to_uint8_t(utki::make_span(str)));

	return read_ext(fi);
}

tree tml::to_non_ext(const tree_ext& t)
{
	tree ret;

	ret.value = static_cast<const leaf&>(t.value);
	ret.children = to_non_ext(t.children);

	return ret;
}

forest tml::to_non_ext(const forest_ext& f)
{
	forest ret;

	for (const auto& c : f) {
		ret.push_back(to_non_ext(c));
	}

	return ret;
}

namespace {
// the minimal number of quotes in a raw string delimiter that makes the
// given string round-trippable: no quote run following ')' in the string
// may look like a closing delimiter
size_t calc_raw_string_delimiter_length(std::string_view str)
{
	constexpr size_t min_raw_string_delimeter_length = 3;

	size_t max_num_quotes_after_close_paren = 0;
	size_t num_quotes_after_close_paren = 0;
	bool close_paren_seen = false;

	for (const auto c : str) {
		if (c == ')') {
			close_paren_seen = true;
			num_quotes_after_close_paren = 0;
		} else if (c == '"') {
			if (close_paren_seen) {
				++num_quotes_after_close_paren;
				if (num_quotes_after_close_paren > max_num_quotes_after_close_paren) {
					max_num_quotes_after_close_paren = num_quotes_after_close_paren;
				}
			}
		} else {
			close_paren_seen = false;
		}
	}

	return std::max(min_raw_string_delimeter_length, max_num_quotes_after_close_paren + 1);
}

void make_escaped_string_ext(
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

void write_ext_internal(
	const forest_ext& roots, //
	fsif::file& fi,
	formatting fmt,
	unsigned indentation
)
{
	const std::array<uint8_t, 1> quote = {{'"'}};
	const std::array<uint8_t, 1> lparen = {{'('}};
	const std::array<uint8_t, 1> rparen = {{')'}};
	const std::array<uint8_t, 1> lcurly = {{'{'}};
	const std::array<uint8_t, 1> rcurly = {{'}'}};
	const std::array<uint8_t, 1> space = {{' '}};
	const std::array<uint8_t, 1> tab = {{'\t'}};
	const std::array<uint8_t, 1> new_line = {{'\n'}};

	bool first = true;

	for (const auto& n : roots) {
		const auto& info = n.value.info;

		if (fmt == formatting::normal) {
			// indent
			for (unsigned i = 0; i != indentation; ++i) {
				fi.write(utki::make_span(tab));
			}
		} else {
			// minimal: use space flag to decide whether to write a space
			if (!first && info.flags.get(tml::flag::space)) {
				fi.write(utki::make_span(space));
			}
		}

		// write node value
		if (info.flags.get(tml::flag::raw)) {
			// write as raw string
			const auto& str = n.value.string;
			const size_t num_quotes = calc_raw_string_delimiter_length(str);
			for (size_t i = 0; i != num_quotes; ++i) {
				fi.write(utki::make_span(quote));
			}
			fi.write(utki::make_span(lparen));
			fi.write(utki::make_span(str.c_str(), str.size()));
			fi.write(utki::make_span(rparen));
			for (size_t i = 0; i != num_quotes; ++i) {
				fi.write(utki::make_span(quote));
			}
		} else if (info.flags.get(tml::flag::quoted)) {
			// write as quoted string
			const auto& str = n.value.string;
			unsigned num_escapes = 0;
			for (const auto c : str) {
				if (c == '\t' || c == '\n' || c == '\\' || c == '"') {
					++num_escapes;
				}
			}

			fi.write(utki::make_span(quote));
			if (num_escapes == 0) {
				fi.write(utki::make_span(str.c_str(), str.size()));
			} else {
				std::vector<uint8_t> buf(str.size() + num_escapes);
				make_escaped_string_ext(
					str, //
					utki::make_span(buf)
				);
				fi.write(utki::make_span(buf));
			}
			fi.write(utki::make_span(quote));
		} else {
			// unquoted: write as-is (the value was originally unquoted)
			fi.write(utki::make_span(n.value.string.c_str(), n.value.string.size()));
		}

		first = false;

		// write children
		const bool has_children = (n.children.size() != 0);
		const bool write_braces = has_children || info.flags.get(tml::flag::curly_braces);

		if (!write_braces) {
			if (fmt == formatting::normal) {
				fi.write(utki::make_span(new_line));
			}
			continue;
		}

		if (fmt != formatting::normal) {
			fi.write(utki::make_span(lcurly));
			write_ext_internal(n.children, fi, fmt, 0);
			fi.write(utki::make_span(rcurly));
		} else {
			fi.write(utki::make_span(lcurly));

			if (n.children.size() == 1 && n.children[0].children.size() == 0) {
				write_ext_internal(n.children, fi, formatting::minimal, 0);
			} else {
				fi.write(utki::make_span(new_line));
				write_ext_internal(n.children, fi, fmt, indentation + 1);
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
	const forest_ext& wood, //
	fsif::file& fi,
	formatting fmt
)
{
	fsif::file::guard file_guard(
		fi, //
		fsif::mode::create
	);

	write_ext_internal(wood, fi, fmt, 0);
}

std::string tml::to_string(const forest_ext& f)
{
	fsif::vector_file fi;
	tml::write(f, fi, tml::formatting::minimal);
	auto v = fi.reset_data();
	return utki::make_string(v);
}
