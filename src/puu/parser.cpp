#include "parser.hpp"

#include <sstream>
#include <cstring>

#include <utki/debug.hpp>

#include "exception.hpp"



using namespace puu;


namespace{
const size_t fileReadChinkSize_c = 0x4ff;
}

void parser::handleStringParsed(listener& listener){
	listener.on_string_parsed(utki::make_span(this->stringBuf));
	this->stringBuf.clear();
}

void parser::processCharInIdle(char c, listener& listener){
	switch(c){
		case ' ':
		case '\n':
		case '\r':
		case '\t':
			break;
		case '\0':
			this->state = State_e::IDLE;
			break;
		case '{':
			listener.on_string_parsed(utki::span<char>());
			listener.on_children_parse_started();
			++this->nestingLevel;
			break;
		case '}':
			listener.on_children_parse_finished();
			--this->nestingLevel;
			break;
		case '"':
			this->state = State_e::QUOTED_STRING;
			break;
		default:
			this->stringBuf.push_back(c);
			this->state = State_e::UNQUOTED_STRING;
			break;
	}
}

void parser::processCharInStringParsed(char c, listener& listener){
	switch (c) {
		case ' ':
		case '\n':
		case '\r':
		case '\t':
			break;
		case '/':
			if(this->stringBuf.size() != 0){
				ASSERT(this->stringBuf.size() == 1)
				ASSERT(this->stringBuf[0] == '/')
				this->stringBuf.clear();
				this->stateAfterComment = State_e::STRING_PARSED;
				this->state = State_e::SINGLE_LINE_COMMENT;
			}else{
				this->stringBuf.push_back(c);
			}
			break;
		case '*':
			if(this->stringBuf.size() != 0){
				ASSERT(this->stringBuf.size() == 1)
				ASSERT(this->stringBuf[0] == '/')
				this->stringBuf.clear();
				this->stateAfterComment = State_e::STRING_PARSED;
				this->state = State_e::MULTILINE_COMMENT;
			}else{
				this->state = State_e::IDLE;
				this->processCharInIdle(c, listener);
			}
			break;
		case '{':
			listener.on_children_parse_started();
			this->state = State_e::IDLE;
			++this->nestingLevel;
			break;
		default:
			this->state = State_e::IDLE;
			this->processCharInIdle(c, listener);
			break;
	}
}

void parser::processCharInUnquotedString(char c, listener& listener){
	switch(c){
		case '/':
			if(this->stringBuf.size() != 0 && this->stringBuf.back() == '/'){
				this->stringBuf.pop_back();
				if(this->stringBuf.size() != 0){
					this->handleStringParsed(listener);
					this->stateAfterComment = State_e::STRING_PARSED;
				}else{
					this->stateAfterComment = State_e::IDLE;
				}
				this->state = State_e::SINGLE_LINE_COMMENT;
			}else{
				this->stringBuf.push_back(c);
			}
			break;
		case '*':
			if(this->stringBuf.size() != 0 && this->stringBuf.back() == '/'){
				this->stringBuf.pop_back();
				if(this->stringBuf.size() != 0){
					this->handleStringParsed(listener);
					this->stateAfterComment = State_e::STRING_PARSED;
				}else{
					this->stateAfterComment = State_e::IDLE;
				}
				this->state = State_e::MULTILINE_COMMENT;
			} else {
				this->stringBuf.push_back(c);
			}
			break;
		case '"':
			ASSERT(this->stringBuf.size() != 0)
			if(this->stringBuf.size() == 1 && this->stringBuf.back() == 'R'){
				this->stringBuf.clear();
				this->state = State_e::RAW_STRING_OPENING_DELIMETER;
			}else{
				this->handleStringParsed(listener);
				this->state = State_e::QUOTED_STRING;
			}
			break;
		case ' ':
		case '\r':
		case '\n':
		case '\t':
			ASSERT(this->stringBuf.size() != 0)
			this->handleStringParsed(listener);
			this->state = State_e::STRING_PARSED;
			break;
		case '\0':
			ASSERT(this->stringBuf.size() != 0)
			this->handleStringParsed(listener);
			this->state = State_e::IDLE;
			break;
		case '{':
			ASSERT(this->stringBuf.size() != 0)
			this->handleStringParsed(listener);
			this->state = State_e::IDLE;
			listener.on_children_parse_started();
			++this->nestingLevel;
			break;
		case '}':
			ASSERT(this->stringBuf.size() != 0)
			this->handleStringParsed(listener);
			this->state = State_e::IDLE;
			listener.on_children_parse_finished();
			--this->nestingLevel;
			break;
		default:
			this->stringBuf.push_back(c);
			break;
	}
}

void parser::processCharInQuotedString(char c, listener& listener){
	switch (c) {
		case '"':
			this->handleStringParsed(listener);
			this->state = State_e::STRING_PARSED;
			break;
		case '\\':
			this->state = State_e::ESCAPE_SEQUENCE;
			break;
		case '\r':
		case '\n':
		case '\t':
			break;
		default:
			this->stringBuf.push_back(c);
			break;
	}
}

void parser::processCharInEscapeSequence(char c, listener& listener){
	switch (c) {
		case '"':
			this->stringBuf.push_back('"');
			break;
		case '\\':
			this->stringBuf.push_back('\\');
			break;
		case 'r':
			this->stringBuf.push_back('\r');
			break;
		case 'n':
			this->stringBuf.push_back('\n');
			break;
		case 't':
			this->stringBuf.push_back('\t');
			break;
		default:
			this->stringBuf.push_back('\\');
			this->stringBuf.push_back(c);
			break;
	}
	this->state = State_e::QUOTED_STRING;
}

void parser::processCharInSingleLineComment(char c, listener& listener){
	switch(c){
		case '\0':
		case '\n':
			this->state = this->stateAfterComment;
			break;
		default:
			break;
	}
}

void parser::processCharInMultiLineComment(char c, listener& listener){
	switch(c){
		case '*':
			ASSERT(this->stringBuf.size() == 0)
			this->stringBuf.push_back('*');
			break;
		case '/':
			if(this->stringBuf.size() != 0){
				ASSERT(this->stringBuf.size() == 1)
				ASSERT(this->stringBuf.back() == '*')
				this->stringBuf.clear();
				this->state = this->stateAfterComment;
			}
			break;
		default:
			this->stringBuf.clear();
			break;
	}
}

void parser::processCharInRawStringOpeningDelimeter(char c, listener& listener) {
	switch(c){
		case '"':
			{
				char r = 'R';
				listener.on_string_parsed(utki::make_span(&r, 1));
			}
			this->handleStringParsed(listener);
			this->state = State_e::STRING_PARSED;
			break;
		case '(':
			this->rawStringDelimeter.assign(&*this->stringBuf.begin(), this->stringBuf.size());
			this->stringBuf.clear();
			this->state = State_e::RAW_STRING;
			break;
		default:
			this->stringBuf.push_back(c);
			break;
	}
}

void parser::processCharInRawString(char c, listener& listener) {
	switch(c){
		case ')':
			this->rawStringDelimeterIndex = 0;
			this->state = State_e::RAW_STRING_CLOSING_DELIMETER;
			break;
		default:
			this->stringBuf.push_back(c);
			break;
	}
}

void parser::processCharInRawStringClosingDelimeter(char c, listener& listener) {
	switch(c){
		case '"':
			ASSERT(this->rawStringDelimeterIndex <= this->rawStringDelimeter.size())
			if(this->rawStringDelimeterIndex != this->rawStringDelimeter.size()){
				this->stringBuf.push_back(')');
				for(size_t i = 0; i != this->rawStringDelimeterIndex; ++i){
					this->stringBuf.push_back(this->rawStringDelimeter[i]);
				}
				this->state = State_e::RAW_STRING;
			}else{
				this->handleStringParsed(listener);
				this->rawStringDelimeter.clear();
				this->state = State_e::STRING_PARSED;
			}
			break;
		default:
			ASSERT(this->rawStringDelimeterIndex <= this->rawStringDelimeter.size())
			if(this->rawStringDelimeterIndex == this->rawStringDelimeter.size()
					|| c != this->rawStringDelimeter[this->rawStringDelimeterIndex])
			{
				this->stringBuf.push_back(')');
				for(size_t i = 0; i != this->rawStringDelimeterIndex; ++i){
					this->stringBuf.push_back(this->rawStringDelimeter[i]);
				}
				this->state = State_e::RAW_STRING;
			}else{
				++this->rawStringDelimeterIndex;
			}
			break;
	}
}


void parser::processChar(char c, listener& listener){
	switch(this->state){
		case State_e::IDLE:
			this->processCharInIdle(c, listener);
			break;
		case State_e::STRING_PARSED:
			this->processCharInStringParsed(c, listener);
			break;
		case State_e::UNQUOTED_STRING:
			this->processCharInUnquotedString(c, listener);
			break;
		case State_e::QUOTED_STRING:
			this->processCharInQuotedString(c, listener);
			break;
		case State_e::ESCAPE_SEQUENCE:
			this->processCharInEscapeSequence(c, listener);
			break;
		case State_e::SINGLE_LINE_COMMENT:
			this->processCharInSingleLineComment(c, listener);
			break;
		case State_e::MULTILINE_COMMENT:
			this->processCharInMultiLineComment(c, listener);
			break;
		case State_e::RAW_STRING_OPENING_DELIMETER:
			this->processCharInRawStringOpeningDelimeter(c, listener);
			break;
		case State_e::RAW_STRING:
			this->processCharInRawString(c, listener);
			break;
		case State_e::RAW_STRING_CLOSING_DELIMETER:
			this->processCharInRawStringClosingDelimeter(c, listener);
			break;
		default:
			ASSERT(false)
			break;
	}
}



void parser::parse_data_chunk(const utki::span<uint8_t> chunk, listener& listener){
	for(auto c : chunk){
		this->processChar(c, listener);
	}
}



void parser::end_of_data(listener& listener){
	this->processChar('\0', listener);

	if(this->nestingLevel != 0 || this->state != State_e::IDLE){
		throw puu::exception("Malformed puu document fed. After parsing all the data, the parser remained in the middle of some parsing task.");
	}

	this->reset();
}



void puu::parse(const papki::File& fi, listener& listener){
	papki::File::Guard fileGuard(fi);

	puu::parser parser;

	std::array<std::uint8_t, fileReadChinkSize_c> buf; //2kb read buffer.

	size_t bytesRead;

	do{
		bytesRead = fi.read(utki::make_span(buf));

		parser.parse_data_chunk(utki::make_span(&*buf.begin(), bytesRead), listener);
	}while(bytesRead == buf.size());

	parser.end_of_data(listener);
}



void parser::reset(){
	this->stringBuf.clear();
	this->nestingLevel = 0;
	this->state = State_e::IDLE;
}
