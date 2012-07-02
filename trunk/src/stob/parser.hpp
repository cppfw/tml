/* The MIT License:

Copyright (c) 2012 Ivan Gagis

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

#pragma once



#include <ting/Buffer.hpp>
#include <ting/Array.hpp>
#include <ting/fs/File.hpp>



namespace stob{



class ParseListener{
public:
	/**
	 * TODO:
     * @param s - pointer to null-terminated string.
     * @param size - string length without terminating 0.
     */
	virtual void OnStringParsed(const char* s, ting::u32 size) = 0;
	
	virtual void OnChildrenParseStarted() = 0;
	
	virtual void OnChildrenParseFinished() = 0;
};



class Parser{
	unsigned curLine;//current line into the document being parsed, used for pointing place of format error.
	
	ting::StaticBuffer<ting::u8, 256> staticBuf; //string buffer
	ting::Array<ting::u8> arrayBuf;
	ting::Buffer<ting::u8>* buf;
	
	ting::u8* p; //current position into the string buffer
	
	unsigned nestingLevel;
	
	//Previous character, used to detect two character sequences like //, /*, */, escape sequences.
	ting::u8 prevChar;
	
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
	//which is not allowed by the STOB format.
	bool stringParsed;
	
	void ParseChar(ting::u8 c, ParseListener& listener);
	void PreParseChar(ting::u8 c, ParseListener& listener);
	
	void AppendCharToString(ting::u8 c);
	
	void HandleLeftCurlyBracket(ParseListener& listener);
	void HandleRightCurlyBracket(ParseListener& listener);
	
	void HandleStringEnd(ParseListener& listener);
public:
	Parser(){
		this->Reset();
	}
	
	void Reset(){
		this->curLine = 1;
		this->buf = &this->staticBuf;
		this->arrayBuf.Reset();
		this->p = this->buf->Begin();
		this->nestingLevel = 0;
		this->prevChar = 0;
		this->commentState = NO_COMMENT;
		this->state = IDLE;
		this->stringParsed = false;
	}
	
	void ParseDataChunk(const ting::Buffer<ting::u8>& chunk, ParseListener& listener);
	
	void EndOfData(ParseListener& listener);
};



void Parse(ting::fs::File& fi, ParseListener& listener);



}//~namespace
