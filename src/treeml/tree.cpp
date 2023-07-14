/*
The MIT License (MIT)

Copyright (c) 2012-2023 Ivan Gagis <igagis@gmail.com>

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

#include <cinttypes>
#include <cstring>
#include <stack>

#include <papki/span_file.hpp>
#include <papki/vector_file.hpp>
#include <utki/string.hpp>

#include "parser.hpp"

using namespace tml;

forest tml::read(const papki::file& fi)
{
	class the_listener : public tml::listener
	{
		std::stack<forest> stack;

	public:
		forest cur_forest;

		void on_children_parse_started(location loc) override
		{
			this->stack.push(std::move(this->cur_forest));
			ASSERT(this->cur_forest.size() == 0)
		}

		void on_children_parse_finished(location loc) override
		{
			if (this->stack.size() == 0) {
				std::stringstream ss;
				ss << "malformed treeml: unopened curly brace encountered at ";
				ss << loc.line << ":" << loc.offset;
				throw std::invalid_argument(ss.str());
			}
			this->stack.top().back().children = std::move(this->cur_forest);
			this->cur_forest = std::move(this->stack.top());
			this->stack.pop();
		}

		void on_string_parsed(std::string_view str, const extra_info& info) override
		{
			this->cur_forest.emplace_back(std::string(str.data(), str.size()));
		}
	} listener;

	tml::parse(fi, listener);

	return std::move(listener.cur_forest);
}

forest tml::read(const std::string& str)
{
	const papki::span_file fi(to_uint8_t(utki::make_span(str)));

	return read(fi);
}

namespace {
bool can_string_be_unquoted(const char* s, size_t& out_length, unsigned& out_num_escapes)
{
	//	TRACE(<< "CanStringBeUnquoted(): enter" << std::endl)

	out_num_escapes = 0;
	out_length = 0;

	if (s == nullptr) {
		// empty string can be unquoted when it has children, so return true.
		return true;
	}

	bool ret = true;

	// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
	for (; *s != '\0'; ++s, ++out_length) {
		//		TRACE(<< "CanStringBeUnquoted(): c = " << (*c) << std::endl)
		switch (*s) {
			case '\t':
			case '\n':
			case '\\':
			case '"':
				++out_num_escapes;
			case '{':
			case '}':
			case ' ':
				ret = false;
				break;
			default:
				break;
		}
	}
	return ret;
}

void make_escaped_string(const char* str, utki::span<uint8_t> out)
{
	auto p = out.begin();
	// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
	for (const char* c = str; *c != 0; ++c) {
		ASSERT(p != out.end())

		switch (*c) {
			case '\t':
				*p = '\\';
				++p;
				ASSERT(p != out.end())
				*p = 't';
				break;
			case '\n':
				*p = '\\';
				++p;
				ASSERT(p != out.end())
				*p = 'n';
				break;
			case '\\':
				*p = '\\';
				++p;
				ASSERT(p != out.end())
				*p = '\\';
				break;
			case '"':
				*p = '\\';
				++p;
				ASSERT(p != out.end())
				*p = '"';
				break;
			default:
				*p = *c;
				break;
		}
		++p;
	}
}

void write_internal(const tml::forest& roots, papki::file& fi, formatting fmt, unsigned indentation)
{
	const std::array<uint8_t, 1> quote = {{'"'}};
	const std::array<uint8_t, 1> lcurly = {{'{'}};
	const std::array<uint8_t, 1> rcurly = {{'}'}};
	const std::array<uint8_t, 1> space = {{' '}};
	const std::array<uint8_t, 1> tab = {{'\t'}};
	const std::array<uint8_t, 1> new_line = {{'\n'}};

	// used to detect case of two adjacent unquoted strings without children, need to insert space between them
	bool prev_was_unquoted_without_children = false;

	bool prev_had_children = true;

	for (auto& n : roots) {
		// indent
		if (fmt == formatting::normal) {
			for (unsigned i = 0; i != indentation; ++i) {
				fi.write(utki::make_span(tab));
			}
		}

		// write node value

		//		TRACE(<< "writing node: " << n.value.c_str() << std::endl)

		unsigned num_escapes = 0;
		size_t length = 0;
		bool unqouted = can_string_be_unquoted(n.value.c_str(), length, num_escapes);

		if (!unqouted) {
			fi.write(utki::make_span(quote));

			if (num_escapes == 0) {
				fi.write(utki::make_span(n.value.c_str(), length));
			} else {
				std::vector<uint8_t> buf(length + num_escapes);

				make_escaped_string(n.value.c_str(), utki::make_span(buf));

				fi.write(utki::make_span(buf));
			}

			fi.write(utki::make_span(quote));
		} else {
			bool is_quoted_empty_string = false;

			if (length == 0) {
				if (n.children.size() == 0 || !prev_had_children) {
					is_quoted_empty_string = true;
				}
			}

			// if the string is unquoted then write space in case the output is unformatted
			if (fmt != formatting::normal && prev_was_unquoted_without_children && !is_quoted_empty_string) {
				fi.write(utki::make_span(space));
			}

			if (length == 0) {
				if (is_quoted_empty_string) {
					fi.write(utki::make_span(quote));
					fi.write(utki::make_span(quote));
				}
			} else {
				ASSERT(num_escapes == 0)
				fi.write(utki::make_span(n.value.c_str(), length));
				ASSERT(n.value.to_string().length() != 0)
				if (n.children.size() == 0 && length == 1 && n.value[0] == 'R') {
					fi.write(utki::make_span(space));
				}
			}
		}

		prev_had_children = (n.children.size() != 0);

		if (n.children.size() == 0) {
			if (fmt == formatting::normal) {
				fi.write(utki::make_span(new_line));
			}
			prev_was_unquoted_without_children = (unqouted && length != 0);
			continue;
		} else {
			prev_was_unquoted_without_children = false;
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

void tml::write(const tml::forest& wood, papki::file& fi, formatting fmt)
{
	papki::file::guard file_guard(fi, papki::file::mode::create);

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
	papki::vector_file fi;
	tml::write(f, fi, tml::formatting::minimal);
	auto v = fi.reset_data();
	return utki::make_string(v);
}
