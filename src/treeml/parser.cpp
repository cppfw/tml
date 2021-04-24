#include "parser.hpp"

#include <sstream>
#include <cstring>

#include <utki/debug.hpp>

using namespace treeml;

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
			this->cur_state = state::idle;
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
			this->cur_state = state::quoted_string;
			break;
		default:
			this->stringBuf.push_back(c);
			this->cur_state = state::unquoted_string;
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
				this->stateAfterComment = state::string_parsed;
				this->cur_state = state::single_line_comment;
			}else{
				this->stringBuf.push_back(c);
			}
			break;
		case '*':
			if(this->stringBuf.size() != 0){
				ASSERT(this->stringBuf.size() == 1)
				ASSERT(this->stringBuf[0] == '/')
				this->stringBuf.clear();
				this->stateAfterComment = state::string_parsed;
				this->cur_state = state::multiline_comment;
			}else{
				this->cur_state = state::idle;
				this->processCharInIdle(c, listener);
			}
			break;
		case '{':
			listener.on_children_parse_started();
			this->cur_state = state::idle;
			++this->nestingLevel;
			break;
		default:
			this->cur_state = state::idle;
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
					this->stateAfterComment = state::string_parsed;
				}else{
					this->stateAfterComment = state::idle;
				}
				this->cur_state = state::single_line_comment;
			}else{
				this->stringBuf.push_back(c);
			}
			break;
		case '*':
			if(this->stringBuf.size() != 0 && this->stringBuf.back() == '/'){
				this->stringBuf.pop_back();
				if(this->stringBuf.size() != 0){
					this->handleStringParsed(listener);
					this->stateAfterComment = state::string_parsed;
				}else{
					this->stateAfterComment = state::idle;
				}
				this->cur_state = state::multiline_comment;
			} else {
				this->stringBuf.push_back(c);
			}
			break;
		case '"':
			ASSERT(this->stringBuf.size() != 0)
			if(this->stringBuf.size() == 1 && this->stringBuf.back() == 'R'){
				this->stringBuf.clear();
				this->cur_state = state::raw_string_opening_delimeter;
			}else{
				this->handleStringParsed(listener);
				this->cur_state = state::quoted_string;
			}
			break;
		case ' ':
		case '\r':
		case '\n':
		case '\t':
			ASSERT(this->stringBuf.size() != 0)
			this->handleStringParsed(listener);
			this->cur_state = state::string_parsed;
			break;
		case '\0':
			ASSERT(this->stringBuf.size() != 0)
			this->handleStringParsed(listener);
			this->cur_state = state::idle;
			break;
		case '{':
			ASSERT(this->stringBuf.size() != 0)
			this->handleStringParsed(listener);
			this->cur_state = state::idle;
			listener.on_children_parse_started();
			++this->nestingLevel;
			break;
		case '}':
			ASSERT(this->stringBuf.size() != 0)
			this->handleStringParsed(listener);
			this->cur_state = state::idle;
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
			this->cur_state = state::string_parsed;
			break;
		case '\\':
			this->cur_state = state::escape_sequence;
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
	this->cur_state = state::quoted_string;
}

void parser::processCharInSingleLineComment(char c, listener& listener){
	switch(c){
		case '\0':
		case '\n':
			this->cur_state = this->stateAfterComment;
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
				this->cur_state = this->stateAfterComment;
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
			this->cur_state = state::string_parsed;
			break;
		case '(':
			this->rawStringDelimeter.assign(&*this->stringBuf.begin(), this->stringBuf.size());
			this->stringBuf.clear();
			this->cur_state = state::raw_string;
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
			this->cur_state = state::raw_string_closing_delimeter;
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
				this->cur_state = state::raw_string;
			}else{
				this->handleStringParsed(listener);
				this->rawStringDelimeter.clear();
				this->cur_state = state::string_parsed;
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
				this->cur_state = state::raw_string;
			}else{
				++this->rawStringDelimeterIndex;
			}
			break;
	}
}

void parser::processChar(char c, listener& listener){
	switch(this->cur_state){
		case state::idle:
			this->processCharInIdle(c, listener);
			break;
		case state::string_parsed:
			this->processCharInStringParsed(c, listener);
			break;
		case state::unquoted_string:
			this->processCharInUnquotedString(c, listener);
			break;
		case state::quoted_string:
			this->processCharInQuotedString(c, listener);
			break;
		case state::escape_sequence:
			this->processCharInEscapeSequence(c, listener);
			break;
		case state::single_line_comment:
			this->processCharInSingleLineComment(c, listener);
			break;
		case state::multiline_comment:
			this->processCharInMultiLineComment(c, listener);
			break;
		case state::raw_string_opening_delimeter:
			this->processCharInRawStringOpeningDelimeter(c, listener);
			break;
		case state::raw_string:
			this->processCharInRawString(c, listener);
			break;
		case state::raw_string_closing_delimeter:
			this->processCharInRawStringClosingDelimeter(c, listener);
			break;
		default:
			ASSERT(false)
			break;
	}
}

void parser::parse_data_chunk(utki::span<const uint8_t> chunk, listener& listener){
	for(auto c : chunk){
		this->processChar(c, listener);
	}
}

void parser::end_of_data(listener& listener){
	this->processChar('\0', listener);

	if(this->nestingLevel != 0 || this->cur_state != state::idle){
		throw std::logic_error("Malformed treeml document fed. After parsing all the data, the parser remained in the middle of some parsing task.");
	}

	this->reset();
}

void treeml::parse(const papki::file& fi, listener& listener){
	papki::file::guard file_guard(fi);

	treeml::parser parser;

	std::array<std::uint8_t, fileReadChinkSize_c> buf; // 2kb read buffer.

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
	this->cur_state = state::idle;
}
