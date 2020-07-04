#pragma once

#include <utki/span.hpp>
#include <papki/file.hpp>

/**
 * puu is a very simple markup language. It is used to describe object
 * hierarchies. The only kind of objects present in puu are Strings.
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
namespace puu{

/**
 * @brief Listener interface for puu parser.
 * During the puu document parsing the Parser notifies this listener object
 * about parsed tokens.
 */
class listener{
public:
	/**
	 * @brief A string token has been parsed.
	 * This method is called by Parser when String token has been parsed.
     * @param str - parsed string.
     */
	virtual void on_string_parsed(utki::span<const char> str) = 0;

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
 * @brief puu parser.
 * This is a class of puu parser. It is used for event-based parsing of puu
 * documents.
 */
class parser{
	std::vector<char> stringBuf;//buffer for current string being parsed

	std::string rawStringDelimeter;//delimeter for raw string
	size_t rawStringDelimeterIndex;//index into the raw string delimeter

	//This variable is used for tracking current nesting level to make checks for detecting malformed puu document
	unsigned nestingLevel;

	enum class State_e{
		IDLE,
		STRING_PARSED,
		QUOTED_STRING,
		ESCAPE_SEQUENCE,
		UNQUOTED_STRING,
		SINGLE_LINE_COMMENT,
		MULTILINE_COMMENT,
		RAW_STRING_OPENING_DELIMETER,
		RAW_STRING_CLOSING_DELIMETER,
		RAW_STRING
	} state;

	State_e stateAfterComment;

	void handleStringParsed(listener& listener);

	void processChar(char c, listener& listener);
	void processCharInIdle(char c, listener& listener);
	void processCharInStringParsed(char c, listener& listener);
	void processCharInUnquotedString(char c, listener& listener);
	void processCharInQuotedString(char c, listener& listener);
	void processCharInEscapeSequence(char c, listener& listener);
	void processCharInSingleLineComment(char c, listener& listener);
	void processCharInMultiLineComment(char c, listener& listener);
	void processCharInRawStringOpeningDelimeter(char c, listener& listener);
	void processCharInRawString(char c, listener& listener);
	void processCharInRawStringClosingDelimeter(char c, listener& listener);

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
	 * @brief Parse chunk of puu data.
	 * Use this method to feed the puu data to the parser.
     * @param chunk - data chunk to parse.
     * @param listener - listener object which will receive notifications about parsed tokens.
     */
	void parse_data_chunk(utki::span<const std::uint8_t> chunk, listener& listener);

	/**
	 * @brief Finalize parsing.
	 * Call this method to finalize parsing after all the available puu data has been fed to the parser.
	 * This will tell parser that there will be no more data and the temporary stored data should be interpreted as it is.
     * @param listener - listener object which will receive notifications about parsed tokens.
     */
	void end_of_data(listener& listener);
};

/**
 * @brief Parse puu document provided by given file interface.
 * Use this function to parse the puu document from file.
 * @param fi - file interface to use for getting the data to parse.
 * @param listener - listener object which will receive notifications about parsed tokens.
 */
void parse(const papki::file& fi, listener& listener);

}
