#pragma once


#include <utki/Buf.hpp>
#include <papki/File.hpp>



/**
 * STOB is a very simple markup language. It is used to describe object
 * hierarchies. The only kind of objects present in STOB are Strings.
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
namespace stob{



/**
 * @brief Listener interface for STOB parser.
 * During the STOB document parsing the Parser notifies this listener object
 * about parsed tokens.
 */
class ParseListener{
public:
	/**
	 * @brief A string token has been parsed.
	 * This method is called by Parser when String token has been parsed.
     * @param str - parsed string.
     */
	virtual void onStringParsed(const utki::Buf<char> str) = 0;
	
	/**
	 * @brief Children list parsing started.
	 * This method is called by Parser when '{' token has been parsed.
     */
	virtual void onChildrenParseStarted() = 0;
	
	/**
	 * @brief Children list parsing finished.
	 * This method is called by Parser when '}' token has been parsed.
     */
	virtual void onChildrenParseFinished() = 0;
	
	virtual ~ParseListener()noexcept{}
};



/**
 * @brief STOB Parser.
 * This is a class of STOB parser. It is used for event-based parsing of STOB
 * documents.
 */
class Parser{
	unsigned curLine;//current line into the document being parsed, used for pointing place of format error.
	
	
	std::vector<char> buf;//buffer for current string being parsed

	
	//This variable is used for tracking current nesting level to make checks for detecting malformed STOB document
	unsigned nestingLevel;
	
	//Previous character, used to detect two character sequences like //, /*, */, escape sequences.
	std::uint8_t prevChar;
	
	enum class CommentState_e{
		NO_COMMENT,
		LINE_COMMENT,
		MULTILINE_COMMENT
	} commentState;
	
	enum class State_e{
		IDLE,
		QUOTED_STRING,
		UNQUOTED_STRING,
	} state;
	
	//This flag indicates that a string has been parsed before but its children list is not yet parsed.
	//This is used to detect cases when curly braces go right after another curly braces, thus omitting the string declaration
	//which is allowed by the STOB format and means that string is empty.
	bool stringParsed;
	
	void parseChar(std::uint8_t c, ParseListener& listener);
	void preParseChar(std::uint8_t c, ParseListener& listener);
	
	void appendCharToString(std::uint8_t c);
	
	void handleLeftCurlyBracket(ParseListener& listener);
	void handleRightCurlyBracket(ParseListener& listener);
	
	void handleStringEnd(ParseListener& listener);
public:
	/**
	 * @brief Constructor.
	 * Creates an initially reset Parser object.
     */
	Parser(){
		this->reset();
	}
	
	/**
	 * @brief Reset parser.
	 * Resets the parser to initial state, discarding all the temporary parsed data and state.
     */
	void reset();
	
	/**
	 * @brief Parse chunk of STOB data.
	 * Use this method to feed the STOB data to the parser.
     * @param chunk - data chunk to parse.
     * @param listener - listener object which will receive notifications about parsed tokens.
	 * @throw stob::Exc - in case of malformed STOB document.
     */
	void parseDataChunk(const utki::Buf<std::uint8_t> chunk, ParseListener& listener);
	
	/**
	 * @brief Finalize parsing.
	 * Call this method to finalize parsing after all the available STOB data has been fed to the parser.
	 * This will tell parser that there will be no more data and the temporary stored data should be interpreted as it is.
     * @param listener - listener object which will receive notifications about parsed tokens.
	 * @throw stob::Exc - in case of malformed STOB document.
     */
	void endOfData(ParseListener& listener);
};



/**
 * @brief Parse STOB document provided by given file interface.
 * Use this function to parse the STOB document from file.
 * @param fi - file interface to use for getting the data to parse.
 * @param listener - listener object which will receive notifications about parsed tokens.
 * @throw stob::Exc - in case of malformed STOB document.
 */
void parse(const papki::File& fi, ParseListener& listener);



}//~namespace
