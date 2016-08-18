#include "parser.hpp"

#include <sstream>
#include <cstring>

#include <utki/debug.hpp>

#include "Exc.hpp"



using namespace stob;


namespace{
const size_t bufReserve_d = 0xff;
const size_t fileReadChinkSize_c = 0x4ff;
}

void Parser::handleStringParsed(ParseListener& listener){
	listener.onStringParsed(std::string(&*this->buf.begin(), this->buf.size()));
	this->buf.clear();
	this->buf.reserve(bufReserve_d);
}

void Parser::processCharInIdle(char c, ParseListener& listener){
	switch (c) {
		case ' ':
		case '\n':
		case '\r':
		case '\t':
			break;
		case '\0':
			this->state = State_e::IDLE;
			break;
		case '{':
			listener.onStringParsed(std::string());
			listener.onChildrenParseStarted();
			++this->nestingLevel;
			break;
		case '}':
			listener.onChildrenParseFinished();
			--this->nestingLevel;
			break;
		case '"':
			this->state = State_e::QUOTED_STRING;
			break;
		default:
			this->buf.push_back(c);
			this->state = State_e::UNQUOTED_STRING;
			break;
	}
}

void Parser::processCharInStringParsed(char c, ParseListener& listener){
	switch (c) {
		case ' ':
		case '\n':
		case '\r':
		case '\t':
			break;
		case '{':
			listener.onChildrenParseStarted();
			this->state = State_e::IDLE;
			++this->nestingLevel;
			break;
		default:
			this->state = State_e::IDLE;
			this->processCharInIdle(c, listener);
			break;
	}
}

void Parser::processCharInUnquotedString(char c, ParseListener& listener){
	switch(c){
		case '/':
			if(this->buf.size() != 0 && this->buf.back() == '/'){
				this->buf.pop_back();
				if(this->buf.size() != 0){
					this->handleStringParsed(listener);
				}
				this->state = State_e::SINGLE_LINE_COMMENT;
			}else{
				this->buf.push_back(c);
			}
			break;
		case '*':
			if(this->buf.size() != 0 && this->buf.back() == '/'){
				this->buf.pop_back();
				if(this->buf.size() != 0){
					this->handleStringParsed(listener);
				}
				state = State_e::MULTILINE_COMMENT;
			} else {
				this->buf.push_back(c);
			}
			break;
		case '"':
			ASSERT(this->buf.size() != 0)
			this->handleStringParsed(listener);
			this->state = State_e::QUOTED_STRING;
			break;
		case ' ':
		case '\r':
		case '\n':
		case '\t':
			ASSERT(this->buf.size() != 0)
			this->handleStringParsed(listener);
			this->state = State_e::STRING_PARSED;
			break;
		case '\0':
			ASSERT(this->buf.size() != 0)
			this->handleStringParsed(listener);
			this->state = State_e::IDLE;
			break;
		case '{':
			ASSERT(this->buf.size() != 0)
			this->handleStringParsed(listener);
			this->state = State_e::IDLE;
			listener.onChildrenParseStarted();
			++this->nestingLevel;
			break;
		case '}':
			ASSERT(this->buf.size() != 0)
			this->handleStringParsed(listener);
			this->state = State_e::IDLE;
			listener.onChildrenParseFinished();
			--this->nestingLevel;
			break;
		default:
			this->buf.push_back(c);
			break;
	}
}

void Parser::processCharInQuotedString(char c, ParseListener& listener){
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
			this->buf.push_back(c);
			break;
	}
}

void Parser::processCharInEscapeSequence(char c, ParseListener& listener){
	switch (c) {
		case '"':
			this->buf.push_back('"');
			break;
		case '\\':
			this->buf.push_back('\\');
			break;
		case 'r':
			this->buf.push_back('\r');
			break;
		case 'n':
			this->buf.push_back('\n');
			break;
		case 't':
			this->buf.push_back('\t');
			break;
		default:
			this->buf.push_back('\\');
			this->buf.push_back(c);
			break;
	}
	this->state = State_e::QUOTED_STRING;
}

void Parser::processCharInSingleLineComment(char c, ParseListener& listener){
	if (c == '\n') {
		this->state = State_e::IDLE;
	}
}

void Parser::processCharInMultiLineComment(char c, ParseListener& listener){
	switch(c){
		case '*':
			ASSERT(this->buf.size() == 0)
			this->buf.push_back('*');
			break;
		case '/':
			if(this->buf.size() != 0){
				ASSERT(this->buf.size() == 1)
				ASSERT(this->buf.back() == '*')
				this->buf.clear();
				this->state = State_e::IDLE;
			}
			break;
		default:
			this->buf.clear();
			break;
	}
}

void Parser::processChar(char c, ParseListener& listener){
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
		default:
			ASSERT(false)
			break;
	}	
}



void Parser::parseDataChunk(const utki::Buf<std::uint8_t> chunk, ParseListener& listener){
	for(auto c : chunk){
		this->processChar(c, listener);
	}
}



void Parser::endOfData(ParseListener& listener){
	this->processChar('\0', listener);
	
	if(this->nestingLevel != 0 || this->state != State_e::IDLE){
		throw stob::Exc("Malformed stob document fed. After parsing all the data, the parser remained in the middle of some parsing task.");
	}
	
	this->reset();
}



void stob::parse(const papki::File& fi, ParseListener& listener){
	papki::File::Guard fileGuard(fi);
	
	stob::Parser parser;
	
	std::array<std::uint8_t, fileReadChinkSize_c> buf; //2kb read buffer.
	
	size_t bytesRead;
	
	do{
		bytesRead = fi.read(utki::wrapBuf(buf));
		
		parser.parseDataChunk(utki::Buf<std::uint8_t>(&*buf.begin(), bytesRead), listener);
	}while(bytesRead == buf.size());

	parser.endOfData(listener);
}



void Parser::reset(){
	this->buf.clear();
	this->buf.reserve(bufReserve_d);
	this->nestingLevel = 0;
	this->state = State_e::IDLE;
}
