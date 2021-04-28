#include "parser.hpp"

#include <sstream>
#include <cstring>

#include <utki/debug.hpp>

using namespace treeml;

namespace{
const size_t fileReadChinkSize_c = 0x4ff;
}

void parser::next_line(){
	++this->cur_loc.line;
	this->cur_loc.offset = 0;
}

void parser::handle_string_parsed(listener& listener){
	listener.on_string_parsed(std::string_view(this->string_buf.data(), this->string_buf.size()), this->info);
	this->string_buf.clear();
	this->info.flags.clear();
}

void parser::set_string_start_pos(){
	this->info.location = this->cur_loc;
	this->info.location.offset -= 1; // current line offset numbering starts from 1
}

void parser::process_char_in_initial(char c, listener& listener){
	ASSERT(this->cur_state == state::initial)
	switch(c){
		case '\n':
			this->next_line();
		case ' ':
		case '\t':
		case '\r':
			break;
		case '/':
			this->set_string_start_pos();
			this->state_after_comment = state::initial;
			this->cur_state = state::comment_seqence;
			break;
		default:
			this->process_char_in_idle(c, listener);
			break;
	}
}

void parser::process_char_in_idle(char c, listener& listener){
	switch(c){
		case '\n':
			this->next_line();
		case ' ':
		case '\t':
		case '\r':
			this->info.flags.set(treeml::flag::space);
			break;
		case '\0':
			break;
		case '{':
			ASSERT(this->string_buf.empty())
			this->handle_string_parsed(listener); // report empty string
			listener.on_children_parse_started();
			++this->nesting_level;
			break;
		case '}':
			listener.on_children_parse_finished();
			--this->nesting_level;
			this->cur_state = state::idle; // this is needed because some other states forward processing to 'process_char_in_idle()'
			this->info.flags.clear(flag::space);
			break;
		case '"':
			this->set_string_start_pos();
			this->cur_state = state::quoted_string;
			this->info.flags.set(flag::quoted);
			break;
		case '/':
			this->set_string_start_pos();
			this->state_after_comment = state::idle;
			this->cur_state = state::comment_seqence;
			break;
		default:
			this->string_buf.push_back(c);
			this->set_string_start_pos();
			this->cur_state = state::unquoted_string;
			break;
	}
}

void parser::process_char_in_string_parsed(char c, listener& listener){
	ASSERT(this->cur_state == state::string_parsed)
	switch (c) {
		case '\n':
			this->next_line();
		case ' ':
		case '\r':
		case '\t':
			this->info.flags.set(treeml::flag::space);
			break;
		case '/':
			this->set_string_start_pos();
			this->state_after_comment = state::string_parsed;
			this->cur_state = state::comment_seqence;
			break;
		case '{':
			listener.on_children_parse_started();
			this->cur_state = state::idle;
			++this->nesting_level;
			this->info.flags.clear(treeml::flag::space);
			break;
		default:
			this->cur_state = state::idle;
			this->process_char_in_idle(c, listener);
			break;
	}
}

void parser::process_char_in_unquoted_string(char c, listener& listener){
	ASSERT(this->cur_state == state::unquoted_string)
	switch(c){
		case '/':
			this->state_after_comment = state::string_parsed;
			this->cur_state = state::comment_seqence;
			break;
		case '"':
			ASSERT(this->string_buf.size() != 0)
			if(this->string_buf.size() == 1 && this->string_buf.back() == 'R'){
				this->string_buf.clear();
				this->cur_state = state::raw_string_opening_delimeter;
				this->info.flags.set(flag::raw_cpp);
			}else{
				this->handle_string_parsed(listener);
				this->cur_state = state::quoted_string;
				this->info.flags.set(flag::quoted);
				this->set_string_start_pos();
			}
			break;
		case '\n':
			this->next_line();
		case ' ':
		case '\r':
		case '\t':
			ASSERT(this->string_buf.size() != 0)
			this->handle_string_parsed(listener);
			this->cur_state = state::string_parsed;
			this->info.flags.set(treeml::flag::space);
			break;
		case '\0':
			ASSERT(this->string_buf.size() != 0)
			this->handle_string_parsed(listener);
			this->cur_state = state::idle;
			break;
		case '{':
			ASSERT(this->string_buf.size() != 0)
			this->handle_string_parsed(listener);
			this->cur_state = state::idle;
			listener.on_children_parse_started();
			++this->nesting_level;
			break;
		case '}':
			ASSERT(this->string_buf.size() != 0)
			this->handle_string_parsed(listener);
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
	ASSERT(this->cur_state == state::quoted_string)
	switch(c){
		case '"':
			this->handle_string_parsed(listener);
			this->cur_state = state::string_parsed;
			break;
		case '\\':
			this->cur_state = state::escape_sequence;
			break;
		case '\n':
			this->next_line();
		case '\r':
		case '\t':
			break;
		default:
			this->string_buf.push_back(c);
			break;
	}
}

void parser::process_char_in_escape_sequence(char c, listener& listener){
	ASSERT(this->cur_state == state::escape_sequence)
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

void parser::process_char_in_comment_sequence(char c, listener& listener){
	ASSERT(this->cur_state == state::comment_seqence)
	switch(c){
		case '/':
			if(!this->string_buf.empty()){
				this->handle_string_parsed(listener);
			}
			this->cur_state = state::single_line_comment;
			break;
		case '*':
			if(!this->string_buf.empty()){
				this->handle_string_parsed(listener);
			}
			this->cur_state = state::multiline_comment;
			break;
		case '{':
			this->string_buf.push_back('/');
			this->handle_string_parsed(listener);
			listener.on_children_parse_started();
			++this->nesting_level;
			this->cur_state = state::idle;
			break;
		case '\n':
			this->next_line();
		case '\r':
		case '\t':
		case ' ':
			this->string_buf.push_back('/');
			this->handle_string_parsed(listener);
			this->cur_state = state::string_parsed;
			break;
		default:
			this->string_buf.push_back('/');
			this->string_buf.push_back(c);
			this->cur_state = state::unquoted_string;
			break;
	}
}

void parser::process_char_in_single_line_comment(char c, listener& listener){
	ASSERT(this->cur_state == state::single_line_comment)
	this->info.flags.set(treeml::flag::space);
	switch(c){
		case '\n':
			this->next_line();
		case '\0':
			this->cur_state = this->state_after_comment;
			break;
		default:
			break;
	}
}

void parser::process_char_in_multiline_comment(char c, listener& listener){
	ASSERT(this->cur_state == state::multiline_comment)
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

void parser::process_char_in_raw_string_opening_delimeter(char c, listener& listener){
	ASSERT(this->cur_state == state::raw_string_opening_delimeter)
	switch(c){
		case '"':
			// not a C++ style raw string, report 'R' string and a quoted string
			this->info.flags.clear(treeml::flag::raw_cpp);
			{
				char r = 'R';
				listener.on_string_parsed(std::string_view(&r, 1), this->info);
				this->info.flags.clear(treeml::flag::space);
			}
			++this->info.location.offset;
			this->info.flags.set(treeml::flag::quoted);
			this->handle_string_parsed(listener);
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

void parser::process_char_in_raw_string(char c, listener& listener){
	ASSERT(this->cur_state == state::raw_string)
	switch(c){
		case ')':
			this->raw_string_delimeter_index = 0;
			this->cur_state = state::raw_string_closing_delimeter;
			break;
		case '\n':
			this->next_line();
		default:
			this->string_buf.push_back(c);
			break;
	}
}

void parser::process_char_in_raw_string_closing_delimeter(char c, listener& listener){
	ASSERT(this->cur_state == state::raw_string_closing_delimeter)
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
				this->handle_string_parsed(listener);
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
		case state::initial:
			this->process_char_in_initial(c, listener);
			break;
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
		case state::comment_seqence:
			this->process_char_in_comment_sequence(c, listener);
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
			ASSERT(false, [&](auto&o){o << "this->cur_state = " << unsigned(this->cur_state);})
			break;
	}
}

void parser::parse_data_chunk(utki::span<const uint8_t> chunk, listener& listener){
	for(auto c : chunk){
		this->process_char(c, listener);
		++this->cur_loc.offset;
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
	this->cur_state = state::initial;
}
