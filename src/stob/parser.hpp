/* The MIT License:

Copyright (c) 2012-2014 Ivan Gagis <igagis@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE. */

// Home page: http://stob.googlecode.com

/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once


#include <ting/Buffer.hpp>
#include <ting/fs/File.hpp>



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
 *		Property1 {Value1}
 *		"Property two" {"Value 2"}
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
	virtual void OnStringParsed(const ting::Buffer<char> str) = 0;
	
	/**
	 * @brief Children list parsing started.
	 * This method is called by Parser when '{' token has been parsed.
     */
	virtual void OnChildrenParseStarted() = 0;
	
	/**
	 * @brief Children list parsing finished.
	 * This method is called by Parser when '}' token has been parsed.
     */
	virtual void OnChildrenParseFinished() = 0;
	
	virtual ~ParseListener()noexcept{}
};



/**
 * @brief STOB Parser.
 * This is a class of STOB parser. It is used for event-based parsing of STOB
 * documents.
 */
class Parser{
	unsigned curLine;//current line into the document being parsed, used for pointing place of format error.
	
	std::array<std::uint8_t, 256> staticBuf; //string buffer
	std::vector<std::uint8_t> arrayBuf;
	ting::Buffer<std::uint8_t> buf;
	
	decltype(buf)::iterator p; //current position into the string buffer
	
	unsigned nestingLevel;
	
	//Previous character, used to detect two character sequences like //, /*, */, escape sequences.
	std::uint8_t prevChar;
	
	enum E_CommentState{
		NO_COMMENT,
		LINE_COMMENT,
		MULTILINE_COMMENT
	};
	E_CommentState commentState;
	
	enum E_State{
		IDLE,
		QUOTED_STRING,
		UNQUOTED_STRING,
	};
	E_State state;
	
	//This flag indicates that a string has been parsed before but its children list is not yet parsed.
	//This is used to detect cases when curly braces go right after another curly braces, thus omitting the string declaration
	//which is allowed by the STOB format and means that string is empty.
	bool stringParsed;
	
	void ParseChar(std::uint8_t c, ParseListener& listener);
	void PreParseChar(std::uint8_t c, ParseListener& listener);
	
	void AppendCharToString(std::uint8_t c);
	
	void HandleLeftCurlyBracket(ParseListener& listener);
	void HandleRightCurlyBracket(ParseListener& listener);
	
	void HandleStringEnd(ParseListener& listener);
public:
	/**
	 * @brief Constructor.
	 * Creates an initially reset Parser object.
     */
	Parser(){
		this->Reset();
	}
	
	/**
	 * @brief Reset parser.
	 * Resets the parser to initial state, discarding all the temporary parsed data and state.
     */
	void Reset(){
		this->curLine = 1;
		this->buf = this->staticBuf;
		this->arrayBuf.clear();
		this->p = this->buf.begin();
		this->nestingLevel = 0;
		this->prevChar = 0;
		this->commentState = NO_COMMENT;
		this->state = IDLE;
		this->stringParsed = false;
	}
	
	/**
	 * @brief Parse chunk of STOB data.
	 * Use this method to feed the STOB data to the parser.
     * @param chunk - data chunk to parse.
     * @param listener - listener object which will receive notifications about parsed tokens.
	 * @throw stob::Exc - in case of malformed STOB document.
     */
	void ParseDataChunk(const ting::Buffer<std::uint8_t> chunk, ParseListener& listener);
	
	/**
	 * @brief Finalize parsing.
	 * Call this method to finalize parsing after all the available STOB data has been fed to the parser.
	 * This will tell parser that there will be no more data and the temporary stored data should be interpreted as it is.
     * @param listener - listener object which will receive notifications about parsed tokens.
	 * @throw stob::Exc - in case of malformed STOB document.
     */
	void EndOfData(ParseListener& listener);
};



/**
 * @brief Parse STOB document provided by given file interface.
 * Use this function to parse the STOB document from file.
 * @param fi - file interface to use for getting the data to parse.
 * @param listener - listener object which will receive notifications about parsed tokens.
 * @throw stob::Exc - in case of malformed STOB document.
 */
void Parse(const ting::fs::File& fi, ParseListener& listener);



}//~namespace
