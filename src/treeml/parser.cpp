#include "parser.hpp"

#include <sstream>
#include <cstring>

#include <utki/debug.hpp>

using namespace treeml;

namespace{
const size_t fileReadChinkSize_c = 0x4ff;
}

void parser::handle_string_parsed(listener& listener, utki::flags<treeml::flags> flags){
	listener.on_string_parsed(std::string_view(this->string_buf.data(), this->string_buf.size()), flags);
	this->string_buf.clear();
}

void parser::process_char_in_idle(char c, listener& listener){
	switch(c){
		case ' ':
		case '\n':
			++this->cur_line;
			this->cur_line_offset = 0;
		case '\r':
		case '\t':
			break;
		case '\0':
			this->cur_state = state::idle;
			break;
		case '{':
			listener.on_string_parsed(std::string_view(nullptr, 0), {}); // TODO: correct flags?
			listener.on_children_parse_started();
			++this->nesting_level;
			break;
		case '}':
			listener.on_children_parse_finished();
			--this->nesting_level;
			break;
		case '"':
			this->cur_state = state::quoted_string;
			break;
		default:
			this->string_buf.push_back(c);
			this->cur_state = state::unquoted_string;
			break;
	}
}

void parser::process_char_in_string_parsed(char c, listener& listener){
	switch (c) {
		case ' ':
		case '\n':
			++this->cur_line;
			this->cur_line_offset = 0;
		case '\r':
		case '\t':
			break;
		case '/':
			if(this->string_buf.size() != 0){
				ASSERT(this->string_buf.size() == 1)
				ASSERT(this->string_buf[0] == '/')
				this->string_buf.clear();
				this->state_after_comment = state::string_parsed;
				this->cur_state = state::single_line_comment;
			}else{
				this->string_buf.push_back(c);
			}
			break;
		case '*':
			if(this->string_buf.size() != 0){
				ASSERT(this->string_buf.size() == 1)
				ASSERT(this->string_buf[0] == '/')
				this->string_buf.clear();
				this->state_after_comment = state::string_parsed;
				this->cur_state = state::multiline_comment;
			}else{
				this->cur_state = state::idle;
				this->process_char_in_idle(c, listener);
			}
			break;
		case '{':
			listener.on_children_parse_started();
			this->cur_state = state::idle;
			++this->nesting_level;
			break;
		default:
			this->cur_state = state::idle;
			this->process_char_in_idle(c, listener);
			break;
	}
}

void parser::process_char_in_unquoted_string(char c, listener& listener){
	switch(c){
		case '/':
			if(this->string_buf.size() != 0 && this->string_buf.back() == '/'){
				this->string_buf.pop_back();
				if(this->string_buf.size() != 0){
					this->handle_string_parsed(listener, {}); // TODO: correct flags?
					this->state_after_comment = state::string_parsed;
				}else{
					this->state_after_comment = state::idle;
				}
				this->cur_state = state::single_line_comment;
			}else{
				this->string_buf.push_back(c);
			}
			break;
		case '*':
			if(this->string_buf.size() != 0 && this->string_buf.back() == '/'){
				this->string_buf.pop_back();
				if(this->string_buf.size() != 0){
					this->handle_string_parsed(listener, {}); // TODO: correct flags?
					this->state_after_comment = state::string_parsed;
				}else{
					this->state_after_comment = state::idle;
				}
				this->cur_state = state::multiline_comment;
			} else {
				this->string_buf.push_back(c);
			}
			break;
		case '"':
			ASSERT(this->string_buf.size() != 0)
			if(this->string_buf.size() == 1 && this->string_buf.back() == 'R'){
				this->string_buf.clear();
				this->cur_state = state::raw_string_opening_delimeter;
			}else{
				this->handle_string_parsed(listener, {}); // TODO: correct flags?
				this->cur_state = state::quoted_string;
			}
			break;
		case ' ':
		case '\r':
		case '\n':
			++this->cur_line;
			this->cur_line_offset = 0;
		case '\t':
			ASSERT(this->string_buf.size() != 0)
			this->handle_string_parsed(listener, {}); // TODO: correct flags?
			this->cur_state = state::string_parsed;
			break;
		case '\0':
			ASSERT(this->string_buf.size() != 0)
			this->handle_string_parsed(listener, {}); // TODO: correct flags?
			this->cur_state = state::idle;
			break;
		case '{':
			ASSERT(this->string_buf.size() != 0)
			this->handle_string_parsed(listener, {}); // TODO: correct flags?
			this->cur_state = state::idle;
			listener.on_children_parse_started();
			++this->nesting_level;
			break;
		case '}':
			ASSERT(this->string_buf.size() != 0)
			this->handle_string_parsed(listener, {}); // TODO: correct flags?
			this->cur_state = state::idle;
			listener.on_children_parse_finished();
			--this->nesting_level;
			break;
		default:
			this->string_buf.push_back(c);
			break;
	}
}

void parser::process_char_in_quoted_string(char c, listener& listener){
	switch (c) {
		case '"':
			this->handle_string_parsed(listener, {}); // TODO: correct flags?
			this->cur_state = state::string_parsed;
			break;
		case '\\':
			this->cur_state = state::escape_sequence;
			break;
		case '\r':
		case '\n':
			++this->cur_line;
			this->cur_line_offset = 0;
		case '\t':
			break;
		default:
			this->string_buf.push_back(c);
			break;
	}
}

void parser::process_char_in_escape_sequence(char c, listener& listener){
	switch (c) {
		case '"':
			this->string_buf.push_back('"');
			break;
		case '\\':
			this->string_buf.push_back('\\');
			break;
		case 'r':
			this->string_buf.push_back('\r');
			break;
		case 'n':
			this->string_buf.push_back('\n');
			break;
		case 't':
			this->string_buf.push_back('\t');
			break;
		default:
			this->string_buf.push_back('\\');
			this->string_buf.push_back(c);
			break;
	}
	this->cur_state = state::quoted_string;
}

void parser::process_char_in_single_line_comment(char c, listener& listener){
	switch(c){
		case '\0':
		case '\n':
			++this->cur_line;
			this->cur_line_offset = 0;
			this->cur_state = this->state_after_comment;
			break;
		default:
			break;
	}
}

void parser::process_char_in_multiline_comment(char c, listener& listener){
	switch(c){
		case '*':
			ASSERT(this->string_buf.size() == 0)
			this->string_buf.push_back('*');
			break;
		case '/':
			if(this->string_buf.size() != 0){
				ASSERT(this->string_buf.size() == 1)
				ASSERT(this->string_buf.back() == '*')
				this->string_buf.clear();
				this->cur_state = this->state_after_comment;
			}
			break;
		default:
			this->string_buf.clear();
			break;
	}
}

void parser::process_char_in_raw_string_opening_delimeter(char c, listener& listener) {
	switch(c){
		case '"':
			{
				char r = 'R';
				listener.on_string_parsed(std::string_view(&r, 1), {}); // TODO: correct flags?
			}
			this->handle_string_parsed(listener, {}); // TODO: correct flags?
			this->cur_state = state::string_parsed;
			break;
		case '(':
			this->raw_string_delimeter.assign(&*this->string_buf.begin(), this->string_buf.size());
			this->string_buf.clear();
			this->cur_state = state::raw_string;
			break;
		default:
			this->string_buf.push_back(c);
			break;
	}
}

void parser::process_char_in_raw_string(char c, listener& listener) {
	switch(c){
		case ')':
			this->raw_string_delimeter_index = 0;
			this->cur_state = state::raw_string_closing_delimeter;
			break;
		default:
			this->string_buf.push_back(c);
			break;
	}
}

void parser::process_char_in_raw_string_closing_delimeter(char c, listener& listener) {
	switch(c){
		case '"':
			ASSERT(this->raw_string_delimeter_index <= this->raw_string_delimeter.size())
			if(this->raw_string_delimeter_index != this->raw_string_delimeter.size()){
				this->string_buf.push_back(')');
				for(size_t i = 0; i != this->raw_string_delimeter_index; ++i){
					this->string_buf.push_back(this->raw_string_delimeter[i]);
				}
				this->cur_state = state::raw_string;
			}else{
				this->handle_string_parsed(listener, {}); // TODO: correct flags?
				this->raw_string_delimeter.clear();
				this->cur_state = state::string_parsed;
			}
			break;
		default:
			ASSERT(this->raw_string_delimeter_index <= this->raw_string_delimeter.size())
			if(this->raw_string_delimeter_index == this->raw_string_delimeter.size()
					|| c != this->raw_string_delimeter[this->raw_string_delimeter_index])
			{
				this->string_buf.push_back(')');
				for(size_t i = 0; i != this->raw_string_delimeter_index; ++i){
					this->string_buf.push_back(this->raw_string_delimeter[i]);
				}
				this->cur_state = state::raw_string;
			}else{
				++this->raw_string_delimeter_index;
			}
			break;
	}
}

void parser::process_char(char c, listener& listener){
	switch(this->cur_state){
		case state::idle:
			this->process_char_in_idle(c, listener);
			break;
		case state::string_parsed:
			this->process_char_in_string_parsed(c, listener);
			break;
		case state::unquoted_string:
			this->process_char_in_unquoted_string(c, listener);
			break;
		case state::quoted_string:
			this->process_char_in_quoted_string(c, listener);
			break;
		case state::escape_sequence:
			this->process_char_in_escape_sequence(c, listener);
			break;
		case state::single_line_comment:
			this->process_char_in_single_line_comment(c, listener);
			break;
		case state::multiline_comment:
			this->process_char_in_multiline_comment(c, listener);
			break;
		case state::raw_string_opening_delimeter:
			this->process_char_in_raw_string_opening_delimeter(c, listener);
			break;
		case state::raw_string:
			this->process_char_in_raw_string(c, listener);
			break;
		case state::raw_string_closing_delimeter:
			this->process_char_in_raw_string_closing_delimeter(c, listener);
			break;
		default:
			ASSERT(false)
			break;
	}
}

void parser::parse_data_chunk(utki::span<const uint8_t> chunk, listener& listener){
	for(auto c : chunk){
		this->process_char(c, listener);
	}
}

void parser::end_of_data(listener& listener){
	this->process_char('\0', listener);

	if(this->nesting_level != 0 || this->cur_state != state::idle){
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
	this->string_buf.clear();
	this->nesting_level = 0;
	this->cur_state = state::idle;
}
