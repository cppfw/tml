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

#pragma once

#include <string_view>

#include <papki/file.hpp>
#include <utki/span.hpp>

#include "extra_info.hpp"

/**
 * treeml is a very simple markup language. It is used to describe object
 * hierarchies. The only kind of objects present in treeml are strings.
 * Objects (which are strings) can have arbitrary number of child objects.
 */
namespace tml {

/**
 * @brief Listener interface for treeml parser.
 * During the treeml document parsing the Parser notifies this listener object
 * about parsed tokens.
 */
class listener
{
public:
	/**
	 * @brief A string token has been parsed.
	 * This method is called by Parser when String token has been parsed.
	 * @param str - parsed string.
	 * @param info - extra information, like line:offset position in original text file.
	 */
	virtual void on_string_parsed(std::string_view str, const extra_info& info) = 0;

	/**
	 * @brief Children list parsing started.
	 * This method is called by Parser when '{' token has been parsed.
	 */
	virtual void on_children_parse_started(location loc) = 0;

	/**
	 * @brief Children list parsing finished.
	 * This method is called by Parser when '}' token has been parsed.
	 */
	virtual void on_children_parse_finished(location loc) = 0;

	listener() = default;

	listener(const listener&) = default;
	listener& operator=(const listener&) = default;

	listener(listener&&) = default;
	listener& operator=(listener&&) = default;

	virtual ~listener() = default;
};

/**
 * @brief treeml parser.
 * This is a class of treeml parser. It is used for event-based parsing of treeml
 * documents.
 */
class parser
{
	std::vector<char> buf; // buffer for current string being parsed

	// used for raw string open/close sequences, unicode sequences etc.
	std::string sequence;
	size_t sequence_index = 0;

	// this variable is used for tracking current nesting level to make checks for detecting malformed treeml document
	unsigned nesting_level = 0;

	enum class state {
		initial, // state before parsing the first node
		idle,
		string_parsed,
		quoted_string,
		escape_sequence,
		unicode_sequence,
		unquoted_string,
		comment_seqence,
		single_line_comment,
		multiline_comment,
		raw_cpp_string_opening_sequence,
		raw_cpp_string_closing_sequence,
		raw_cpp_string,
		raw_python_string_opening_sequence,
		raw_python_string_closing_sequence,
		raw_python_string,
	} cur_state = state::initial;

	state previous_state = state::idle;

	void handle_string_parsed(tml::listener& listener);

	void process_char(char c, tml::listener& listener);
	void process_char_in_initial(char c, tml::listener& listener);
	void process_char_in_idle(char c, tml::listener& listener);
	void process_char_in_string_parsed(char c, tml::listener& listener);
	void process_char_in_unquoted_string(char c, tml::listener& listener);
	void process_char_in_quoted_string(char c, tml::listener& listener);
	void process_char_in_escape_sequence(char c, tml::listener& listener);
	void process_char_in_unicode_sequence(char c, tml::listener& listener);
	void process_char_in_comment_sequence(char c, tml::listener& listener);
	void process_char_in_single_line_comment(char c, tml::listener& listener);
	void process_char_in_multiline_comment(char c, tml::listener& listener);
	void process_char_in_raw_cpp_string_opening_sequence(char c, tml::listener& listener);
	void process_char_in_raw_cpp_string(char c, tml::listener& listener);
	void process_char_in_raw_cpp_string_closing_sequence(char c, tml::listener& listener);
	void process_char_in_raw_python_string_opening_sequence(char c, tml::listener& listener);
	void process_char_in_raw_python_string(char c, tml::listener& listener);
	void process_char_in_raw_python_string_closing_sequence(char c, tml::listener& listener);

	location cur_loc = {1, 1}; // offset starts with 1

	void next_line();

	extra_info info = {{}, tml::flag::first_on_line};

	// extra info saved for parsed string when moving to string_parsed state
	extra_info string_parsed_info;

	void set_string_start_pos();

	void set_string_parsed_state();

public:
	/**
	 * @brief Constructor.
	 * Creates an initially reset Parser object.
	 */
	parser()
	{
		this->reset();
	}

	/**
	 * @brief Reset parser.
	 * Resets the parser to initial state, discarding all the temporary parsed data and state.
	 */
	void reset();

	/**
	 * @brief Parse chunk of treeml data.
	 * Use this method to feed the treeml data to the parser.
	 * @param chunk - data chunk to parse.
	 * @param listener - listener object which will receive notifications about parsed tokens.
	 */
	void parse_data_chunk(utki::span<const char> chunk, listener& listener);

	void parse_data_chunk(utki::span<const uint8_t> chunk, listener& listener)
	{
		// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
		this->parse_data_chunk(utki::make_span(reinterpret_cast<const char*>(chunk.data()), chunk.size()), listener);
	}

	/**
	 * @brief Finalize parsing.
	 * Call this method to finalize parsing after all the available treeml data has been fed to the parser.
	 * This will tell parser that there will be no more data and the temporary stored data should be interpreted as it
	 * is.
	 * @param listener - listener object which will receive notifications about parsed tokens.
	 */
	void end_of_data(listener& listener);
};

/**
 * @brief Parse treeml document provided by given file interface.
 * Use this function to parse the treeml document from file.
 * @param fi - file interface to use for getting the data to parse.
 * @param listener - listener object which will receive notifications about parsed tokens.
 */
void parse(const papki::file& fi, listener& listener);

} // namespace tml
