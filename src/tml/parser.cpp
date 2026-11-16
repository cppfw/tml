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

#include "parser.hpp"

#include <charconv>
#include <cstring>
#include <sstream>

#include <utki/debug.hpp>
#include <utki/string.hpp>
#include <utki/unicode.hpp>

#ifdef DEBUG
#	include <utki/string.hpp>
#endif

using namespace tml;

namespace {
constexpr size_t file_read_chunk_size = 0x4ff;
} // namespace

void parser::next_line()
{
	++this->cur_loc.line;
	this->cur_loc.offset = 0;
}

void parser::handle_string_parsed(listener& listener)
{
	// string values (quoted and raw alike) are byte-exact: no leading/trailing
	// newline stripping is performed
	listener.on_string_parsed(utki::make_string_view(this->buf), this->string_parsed_info);
	this->buf.clear();
}

void parser::set_string_start_pos()
{
	this->info.location = this->cur_loc;
}

void parser::process_char_in_initial(char c, listener& listener)
{
	utki::assert(this->cur_state == state::initial, SL);
	switch (c) {
		case '\n':
			this->info.flags.set(tml::flag::first_on_line);
		case ' ':
		case '\t':
		case '\r':
			break;
		case '#':
			this->set_string_start_pos();
			this->previous_state = state::initial;
			this->cur_state = state::single_line_comment;
			break;
		case '\0':
			this->cur_state = state::idle; // parser should remain in idle state after data end
			break;
		default:
			this->process_char_in_idle(c, listener);
			break;
	}
}

void parser::process_char_in_idle(char c, listener& listener)
{
	switch (c) {
		case '\n':
			this->info.flags.set(tml::flag::first_on_line);
		case ' ':
		case '\t':
		case '\r':
			this->info.flags.set(tml::flag::space);
			break;
		case '\0':
			break;
		case '{':
			utki::assert(this->buf.empty(), SL);
			{
				std::stringstream ss;
				ss << "Malformed tml document fed. Unexpected { at line: " << this->cur_loc.line;
				throw std::invalid_argument(ss.str());
			}
			break;
		case '}':
			listener.on_children_parse_finished(this->cur_loc);
			--this->nesting_level;

			// Some other states forward processing to 'process_char_in_idle()' by explicitly calling it,
			// thus this function can be called even when parser is not in idle state.
			// This is why here we set the state to idle.
			this->cur_state = state::idle;

			this->info.flags.clear(flag::space);
			break;
		case '"':
			this->set_string_start_pos();
			this->cur_state = state::raw_quotes_string_opening_sequence;
			this->sequence_index = 1;
			break;
		case '#':
			this->set_string_start_pos();
			this->previous_state = state::idle;
			this->cur_state = state::single_line_comment;
			break;
		case '\\':
			this->set_string_start_pos();
			this->previous_state = state::unquoted_string;
			this->cur_state = state::escape_sequence;
			break;
		default:
			this->buf.push_back(c);
			this->set_string_start_pos();
			this->cur_state = state::unquoted_string;
			break;
	}
}

void parser::set_string_parsed_state()
{
	this->string_parsed_info = this->info;
	this->info.flags.clear();
	this->cur_state = state::string_parsed;
}

void parser::process_char_in_string_parsed(char c, listener& listener)
{
	utki::assert(this->cur_state == state::string_parsed, SL);
	switch (c) {
		case '\n':
			this->info.flags.set(tml::flag::first_on_line);
		case ' ':
		case '\r':
		case '\t':
			this->info.flags.set(tml::flag::space);
			break;
		case '#':
			this->set_string_start_pos();
			this->previous_state = state::string_parsed;
			this->cur_state = state::single_line_comment;
			break;
		case '{':
			this->string_parsed_info.flags.set(tml::flag::curly_braces);
			this->handle_string_parsed(listener);
			listener.on_children_parse_started(this->cur_loc);
			this->cur_state = state::initial;
			++this->nesting_level;
			this->info.flags.clear(tml::flag::space);
			this->info.flags.clear(tml::flag::first_on_line);
			break;
		default:
			this->handle_string_parsed(listener);
			this->cur_state = state::idle;
			this->process_char_in_idle(c, listener);
			break;
	}
}

void parser::process_char_in_unquoted_string(char c, listener& listener)
{
	utki::assert(this->cur_state == state::unquoted_string, SL);
	switch (c) {
		case '"':
			utki::assert(this->buf.size() != 0, SL);
			this->set_string_parsed_state();
			this->handle_string_parsed(listener);
			this->cur_state = state::raw_quotes_string_opening_sequence;
			this->sequence_index = 1;
			this->set_string_start_pos();
			break;
		case '\n':
		case ' ':
		case '\r':
		case '\t':
			utki::assert(this->buf.size() != 0, SL);
			// this->handle_string_parsed(listener);
			this->set_string_parsed_state();

			this->info.flags.set(tml::flag::space);
			if (c == '\n') {
				this->info.flags.set(tml::flag::first_on_line);
			}
			break;
		case '\0': // end of data
			this->set_string_parsed_state();
			this->handle_string_parsed(listener);
			this->cur_state = state::idle;
			break;
		case '\\':
			this->previous_state = this->cur_state;
			this->cur_state = state::escape_sequence;
			break;
		case '{':
			utki::assert(this->buf.size() != 0, SL);
			this->info.flags.set(tml::flag::curly_braces);
			this->set_string_parsed_state();
			this->handle_string_parsed(listener);
			this->cur_state = state::initial;
			listener.on_children_parse_started(this->cur_loc);
			++this->nesting_level;
			break;
		case '}':
			utki::assert(this->buf.size() != 0, SL);
			this->set_string_parsed_state();
			this->handle_string_parsed(listener);
			this->cur_state = state::idle;
			listener.on_children_parse_finished(this->cur_loc);
			--this->nesting_level;
			break;
		case '#':
			utki::assert(this->buf.size() != 0, SL);
			this->set_string_parsed_state();
			this->previous_state = state::string_parsed;
			this->cur_state = state::single_line_comment;
			break;
		default:
			this->buf.push_back(c);
			break;
	}
}

void parser::process_char_in_quoted_string(char c, listener&)
{
	utki::assert(this->cur_state == state::quoted_string, SL);
	switch (c) {
		case '"':
			this->set_string_parsed_state();
			break;
		case '\\':
			this->previous_state = this->cur_state;
			this->cur_state = state::escape_sequence;
			break;
		default:
			// byte-exact: a literal new line / tab / carriage return typed
			// between the quotes is kept verbatim as part of the string value
			this->buf.push_back(c);
			break;
	}
}

void parser::process_char_in_escape_sequence(char c, listener& listener)
{
	constexpr auto short_unicode_sequence_length = 4;
	constexpr auto long_unicode_sequence_length = 8;

	utki::assert(this->cur_state == state::escape_sequence, SL);
	switch (c) {
		case 'u':
			this->cur_state = state::unicode_sequence;
			this->sequence_index = 0;
			this->sequence.resize(short_unicode_sequence_length);
			return;
		case 'U':
			this->cur_state = state::unicode_sequence;
			this->sequence_index = 0;
			this->sequence.resize(long_unicode_sequence_length);
			return;
		case 'n':
			this->buf.push_back('\n');
			break;
		case 't':
			this->buf.push_back('\t');
			break;
		case 'r':
			this->buf.push_back('\r');
			break;
		case '\n':
			this->buf.push_back('\\');
			if (this->previous_state == state::unquoted_string) {
				this->cur_state = state::unquoted_string;
				this->process_char_in_unquoted_string('\n', listener);
				return;
			}
			break;
		case '"':
		case '\\':
			this->buf.push_back(c);
			break;
		case ' ':
		case '{':
		case '}':
			if (this->previous_state == state::quoted_string) {
				this->buf.push_back('\\');
			}
			this->buf.push_back(c);
			break;
		default:
			this->buf.push_back('\\');
			this->buf.push_back(c);
			break;
	}
	this->cur_state = this->previous_state;
}

void parser::process_char_in_unicode_sequence(char c, listener& listener)
{
	utki::assert(this->cur_state == state::unicode_sequence, SL);

	this->sequence[this->sequence_index] = c;
	++this->sequence_index;

	if (this->sequence_index == this->sequence.size()) {
		uint32_t value = 0;
		auto span = utki::make_span(this->sequence);
		auto res = std::from_chars(
			span.data(), //
			utki::end_pointer(span),
			value,
			utki::to_int(utki::integer_base::hex)
		);

		// require that the entire sequence was consumed, otherwise a trailing
		// non-hex character would be silently dropped (e.g. \u12G4)
		if (res.ec == std::errc::invalid_argument || res.ptr != utki::end_pointer(span)) {
			throw std::invalid_argument(utki::cat(
				"malformed document: could not parse hexadecimal number of unicode escape sequence at line: ",
				this->cur_loc.line
			));
		}

		auto bytes = utki::to_utf8(char32_t(value));

		// NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
		for (auto b : bytes) {
			if (b == '\0') {
				break;
			}
			this->buf.push_back(b);
		}

		this->cur_state = this->previous_state;
		this->sequence.clear();
	}
}

void parser::process_char_in_single_line_comment(char c, listener& listener)
{
	utki::assert(this->cur_state == state::single_line_comment, SL);
	this->info.flags.set(tml::flag::space);
	switch (c) {
		case '\0':
			this->cur_state = this->previous_state;
			this->process_char('\0', listener);
			break;
		case '\n':
			this->cur_state = this->previous_state;
			break;
		default:
			break;
	}
}

void parser::process_char_in_raw_quotes_string_opening_sequence(char c, listener& listener)
{
	utki::assert(this->cur_state == state::raw_quotes_string_opening_sequence, SL);
	utki::assert(this->buf.empty(), SL);
	switch (c) {
		case '"':
			// keep counting the maximal run of consecutive quotes; the token type
			// (quoted / empty / raw) is decided when the run is terminated by a non-quote char
			++this->sequence_index;
			break;
		default:
			switch (this->sequence_index) {
				case 1:
					// a single quote: a quoted string
					this->info.flags.set(flag::quoted);
					this->cur_state = state::quoted_string;
					this->process_char_in_quoted_string(c, listener);
					break;
				case 2:
					// two quotes: an empty quoted string
					this->info.flags.set(flag::quoted);
					this->set_string_parsed_state();
					this->process_char_in_string_parsed(c, listener);
					break;
				default:
					// N >= 3 consecutive quotes: a raw string with an N-quote delimiter
					this->raw_string_delimiter_length = this->sequence_index;
					this->sequence_index = 0;
					this->cur_state = state::raw_quotes_string;
					this->info.flags.set(flag::raw);
					if (c != '\0') {
						this->buf.push_back(c);
					}
					break;
			}
			break;
	}
}

void parser::process_char_in_raw_quotes_string(char c, listener& listener)
{
	utki::assert(this->cur_state == state::raw_quotes_string, SL);
	switch (c) {
		case '"':
			this->cur_state = state::raw_quotes_string_closing_sequence;
			this->sequence_index = 1;
			break;
		default:
			this->buf.push_back(c);
			break;
	}
}

void parser::process_char_in_raw_quotes_string_closing_sequence(char c, listener& listener)
{
	utki::assert(this->cur_state == state::raw_quotes_string_closing_sequence, SL);
	switch (c) {
		case '"':
			// count the run of consecutive quotes; it is either the closing delimiter
			// (exactly N quotes), part of the content (fewer than N), or malformed (more than N)
			++this->sequence_index;
			break;
		default:
			if (this->sequence_index > this->raw_string_delimiter_length) {
				throw std::invalid_argument(utki::cat(
					"Malformed tml document fed. A run of more than ",
					this->raw_string_delimiter_length,
					" consecutive quotes in a raw string at line: ",
					this->cur_loc.line
				));
			}
			if (this->sequence_index == this->raw_string_delimiter_length) {
				// a run of exactly N quotes: the raw string is closed
				this->set_string_parsed_state();
				this->process_char_in_string_parsed(c, listener);
			} else {
				// a run of fewer than N quotes: they are part of the raw string content
				this->buf.insert(this->buf.end(), this->sequence_index, '"');
				if (c != '\0') {
					this->buf.push_back(c);
				}
				this->cur_state = state::raw_quotes_string;
			}
			break;
	}
}

void parser::process_char(char c, listener& listener)
{
	switch (this->cur_state) {
		case state::initial:
			this->process_char_in_initial(c, listener);
			break;
		case state::idle:
			this->process_char_in_idle(c, listener);
			break;
		case state::string_parsed:
			this->process_char_in_string_parsed(c, listener);
			break;
		case state::unquoted_string:
			this->process_char_in_unquoted_string(c, listener);
			break;
		case state::quoted_string:
			this->process_char_in_quoted_string(c, listener);
			break;
		case state::escape_sequence:
			this->process_char_in_escape_sequence(c, listener);
			break;
		case state::unicode_sequence:
			this->process_char_in_unicode_sequence(c, listener);
			break;
		case state::single_line_comment:
			this->process_char_in_single_line_comment(c, listener);
			break;
		case state::raw_quotes_string_opening_sequence:
			this->process_char_in_raw_quotes_string_opening_sequence(c, listener);
			break;
		case state::raw_quotes_string:
			this->process_char_in_raw_quotes_string(c, listener);
			break;
		case state::raw_quotes_string_closing_sequence:
			this->process_char_in_raw_quotes_string_closing_sequence(c, listener);
			break;
		default:
			utki::assert(
				false,
				[&](auto& o) {
					o << "this->cur_state = " << unsigned(this->cur_state);
				},
				SL
			);
			break;
	}
}

void parser::parse_data_chunk(utki::span<const char> chunk, listener& listener)
{
	for (auto c : chunk) {
		if (c == '\0') {
			// A NUL byte terminates the document: stop parsing and ignore any
			// remaining input.
			this->null_terminator_encountered_ = true;
			break;
		}
		if (c == '\n') {
			this->next_line();
		}
		this->process_char(c, listener);
		++this->cur_loc.offset;
	}
}

void parser::end_of_data(listener& listener)
{
	this->process_char('\0', listener);

	if (this->nesting_level != 0) {
		throw std::invalid_argument("Malformed tml document fed. Document end reached while parsing children block.");
	}

	if (this->cur_state != state::idle) {
		throw std::invalid_argument(
			"Malformed tml document fed. After parsing all the data, the parser remained in the middle of some parsing task."
		);
	}

	this->reset();
}

void tml::parse(const fsif::file& fi, listener& listener)
{
	fsif::file::guard file_guard(fi);

	tml::parser parser;

	// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
	std::array<uint8_t, file_read_chunk_size> buf;

	// size_t num_bytes_read;

	for (;;) {
		size_t num_bytes_read = fi.read(utki::make_span(buf));

		parser.parse_data_chunk(utki::make_span(buf.data(), num_bytes_read), listener);

		if (num_bytes_read != buf.size() || parser.data_ended()) {
			break;
		}
	}

	if (parser.data_ended()) {
		parser.terminate_document(listener);
	} else {
		parser.end_of_data(listener);
	}
}

void parser::terminate_document(listener& listener)
{
	// A NUL byte terminates the document: emit the current value (if any and
	// complete) and discard an incomplete value. Unlike end_of_data this does
	// not throw for an incomplete value (the NUL byte is a soft EOF marker).
	if (this->cur_state == state::string_parsed || this->cur_state == state::unquoted_string) {
		this->handle_string_parsed(listener);
	}
	this->reset();
}

void parser::reset()
{
	this->buf.clear();
	this->nesting_level = 0;
	this->cur_state = state::initial;
	this->null_terminator_encountered_ = false;
}
