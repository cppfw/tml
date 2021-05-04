#pragma once

#include <string_view>

#include <utki/span.hpp>

#include <papki/file.hpp>

#include "extra_info.hpp"

/**
 * treeml is a very simple markup language. It is used to describe object
 * hierarchies. The only kind of objects present in treeml are Strings.
 * The name of the language comes from "STring OBjects".
 * Objects (which are strings) can have arbitrary number of child objects.
 * Example:
 * @code
 * "String object"
 * AnotherStringObject
 * "String with children"{
 *	"child 1"
 *	Child2
 *	"child three"{
 *		SubChild1
 *		"Subchild two"
 *
 *		Property1 {value1}
 *		"Property two" {"value 2"}
 *
 *		//comment
 *
 *		/ * multi line
 *		   comments as in C * /
 *
 *		"Escape sequences \" \n \r \t \\ \/"
 *	}
 * }
 * @endcode
 */
namespace treeml{

/**
 * @brief Listener interface for treeml parser.
 * During the treeml document parsing the Parser notifies this listener object
 * about parsed tokens.
 */
class listener{
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
	virtual void on_children_parse_started() = 0;

	/**
	 * @brief Children list parsing finished.
	 * This method is called by Parser when '}' token has been parsed.
     */
	virtual void on_children_parse_finished() = 0;

	virtual ~listener()noexcept{}
};

/**
 * @brief treeml parser.
 * This is a class of treeml parser. It is used for event-based parsing of treeml
 * documents.
 */
class parser{
	std::vector<char> buf; // buffer for current string being parsed

	std::string sequenece; // used for raw string open/close sequences
	size_t sequence_index;

	// This variable is used for tracking current nesting level to make checks for detecting malformed treeml document
	unsigned nesting_level;

	enum class state{
		initial, // state before parsing the first node
		idle,
		string_parsed,
		quoted_string,
		escape_sequence,
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
	} cur_state;

	state state_after_comment;

	void handle_string_parsed(treeml::listener& listener);

	void process_char(char c, treeml::listener& listener);
	void process_char_in_initial(char c, treeml::listener& listener);
	void process_char_in_idle(char c, treeml::listener& listener);
	void process_char_in_string_parsed(char c, treeml::listener& listener);
	void process_char_in_unquoted_string(char c, treeml::listener& listener);
	void process_char_in_quoted_string(char c, treeml::listener& listener);
	void process_char_in_escape_sequence(char c, treeml::listener& listener);
	void process_char_in_comment_sequence(char c, treeml::listener& listener);
	void process_char_in_single_line_comment(char c, treeml::listener& listener);
	void process_char_in_multiline_comment(char c, treeml::listener& listener);
	void process_char_in_raw_cpp_string_opening_sequence(char c, treeml::listener& listener);
	void process_char_in_raw_cpp_string(char c, treeml::listener& listener);
	void process_char_in_raw_cpp_string_closing_sequence(char c, treeml::listener& listener);
	void process_char_in_raw_python_string_opening_sequence(char c, treeml::listener& listener);
	void process_char_in_raw_python_string(char c, treeml::listener& listener);
	void process_char_in_raw_python_string_closing_sequence(char c, treeml::listener& listener);

	location cur_loc = {1, 1}; // offset starts with 1

	void next_line();

	extra_info info;

	void set_string_start_pos();

public:
	/**
	 * @brief Constructor.
	 * Creates an initially reset Parser object.
     */
	parser(){
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
	void parse_data_chunk(utki::span<const std::uint8_t> chunk, listener& listener);

	void parse_data_chunk(std::string_view chunk, listener& listener){
		this->parse_data_chunk(
				utki::make_span(
						reinterpret_cast<const uint8_t*>(chunk.data()),
						chunk.size()
					),
				listener
			);
	}

	/**
	 * @brief Finalize parsing.
	 * Call this method to finalize parsing after all the available treeml data has been fed to the parser.
	 * This will tell parser that there will be no more data and the temporary stored data should be interpreted as it is.
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

}

