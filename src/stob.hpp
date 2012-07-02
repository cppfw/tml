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


#include <ting/Ptr.hpp>
#include <ting/PoolStored.hpp>
#include <ting/Exc.hpp>
#include <ting/fs/File.hpp>
#include <ting/Buffer.hpp>
#include <ting/Array.hpp>



namespace stob{



class Exc : public ting::Exc{
public:
	Exc(const std::string& message) :
			ting::Exc(message)
	{}
};



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
	ting::Inited<unsigned, 1> curLine;//current line into the document being parsed, used for pointing place of format error.
	
	ting::StaticBuffer<ting::u8, 256> staticBuf; //string buffer
	ting::Array<ting::u8> arrayBuf;
	ting::Buffer<ting::u8>* buf;
	
	ting::u8* p; //current position into the string buffer
	
	ting::Inited<unsigned, 0> nestingLevel;
	
	//Previous character, used to detect two character sequences like //, /*, */, escape sequences.
	ting::Inited<ting::u8, 0> prevChar;
	
	enum E_CommentState{
		NO_COMMENT,
		LINE_COMMENT,
		MULTILINE_COMMENT
	};
	ting::Inited<E_CommentState, NO_COMMENT> commentState;
	
	enum E_State{
		IDLE,
		QUOTED_STRING,
		UNQUOTED_STRING,
	};
	ting::Inited<E_State, IDLE> state;
	
	//This flag indicates that a string has been parsed before but its children list is not yet parsed.
	//This is used to detect cases when curly braces go right after another curly braces, thus omitting the string declaration
	//which is not allowed by the STOB format.
	ting::Inited<bool, false> stringParsed;
	
	void ParseChar(ting::u8 c, ParseListener& listener);
	void PreParseChar(ting::u8 c, ParseListener& listener);
	
	void AppendCharToString(ting::u8 c);
	
	void HandleLeftCurlyBracket(ParseListener& listener);
	void HandleRightCurlyBracket(ParseListener& listener);
	
	void HandleStringEnd(ParseListener& listener);
public:
	Parser() :
			buf(&this->staticBuf),
			p(this->buf->Begin())
	{
		//TODO: reset parser
	}
	
	void ParseDataChunk(const ting::Buffer<ting::u8>& chunk, ParseListener& listener);
	
	void EndOfData(ParseListener& listener);
};



void Parse(ting::fs::File& fi, ParseListener& listener);



class Node : public ting::PoolStored<Node, 4096 / (4 * sizeof(void*))>{
	ting::Ptr<const char> value; //node value
	
	ting::Ptr<Node> next; //next sibling node
	Node* prev; //previous sibling node
	
	ting::Ptr<Node> children; //pointer to the first child
	
	//constructor is private, no inheritance.
	Node(){}
public:
	
	static inline ting::Ptr<Node> New(){
		return ting::Ptr<Node>(new Node());
	}
	
	inline const char* Value()const throw(){
		return this->value.operator->();
	}
	
	inline void SetValue(ting::Ptr<const char> value)throw(){
		this->value = value;
	}
	
	//TODO: as int, as uint, as float, as double, as boolean
	
	//TODO: get children
	
	//TODO: insert after/before
	
	//TODO: pull out (remove)
	
	void Write(ting::fs::File& fi);
};



ting::Ptr<Node> Load(ting::fs::File& fi);



}//~namespace
