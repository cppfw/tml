#include "parser.hpp"

#include <sstream>
#include <cstring>

#include <utki/debug.hpp>

using namespace treeml;

namespace{
const size_t file_read_chunk_size = 0x4ff;
}

void parser::next_line(){
	++this->cur_loc.line;
	this->cur_loc.offset = 0;
}

void parser::handle_string_parsed(listener& listener){
	auto data = this->buf.data();
	auto size = this->buf.size();
	if(this->info.flags.get(flag::raw)){
		if(size >= 2 && data[0] == '\r' && data[1] == '\n'){
			data += 2;
			size -= 2;
		}else if(size >= 1 && data[0] == '\n'){
			++data;
			--size;
		}

		if(size >= 1 && data[size - 1] == '\n'){
			--size;
		}

		if(size >= 1 && data[size - 1] == '\r'){
			--size;
		}
	}

	listener.on_string_parsed(std::string_view(data, size), this->info);
	this->buf.clear();
	this->info.flags.clear();
}

void parser::set_string_start_pos(){
	this->info.location = this->cur_loc;
}

void parser::process_char_in_initial(char c, listener& listener){
	ASSERT(this->cur_state == state::initial)
	switch(c){
		case '\n':
		case ' ':
		case '\t':
		case '\r':
			break;
		case '/':
			this->set_string_start_pos();
			this->state_after_comment = state::initial;
			this->cur_state = state::comment_seqence;
			break;
		case '\0':
			this->cur_state = state::idle; // parser should remain in idle state after data end
			break;
		default:
			this->process_char_in_idle(c, listener);
			break;
	}
}

void parser::process_char_in_idle(char c, listener& listener){
	switch(c){
		case '\n':
		case ' ':
		case '\t':
		case '\r':
			this->info.flags.set(treeml::flag::space);
			break;
		case '\0':
			break;
		case '{':
			ASSERT(this->buf.empty())
			this->set_string_start_pos();
			this->handle_string_parsed(listener); // report empty string
			listener.on_children_parse_started(this->cur_loc);
			++this->nesting_level;
			this->cur_state = state::initial;
			break;
		case '}':
			listener.on_children_parse_finished(this->cur_loc);
			--this->nesting_level;
			this->cur_state = state::idle; // this is needed because some other states forward processing to 'process_char_in_idle()'
			this->info.flags.clear(flag::space);
			break;
		case '"':
			this->set_string_start_pos();
			this->cur_state = state::raw_python_string_opening_sequence;
			this->sequence_index = 1;
			break;
		case '/':
			this->set_string_start_pos();
			this->state_after_comment = state::idle;
			this->cur_state = state::comment_seqence;
			break;
		default:
			this->buf.push_back(c);
			this->set_string_start_pos();
			this->cur_state = state::unquoted_string;
			break;
	}
}

void parser::process_char_in_string_parsed(char c, listener& listener){
	ASSERT(this->cur_state == state::string_parsed)
	switch (c) {
		case '\n':
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
			listener.on_children_parse_started(this->cur_loc);
			this->cur_state = state::initial;
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
			ASSERT(this->buf.size() != 0)
			if(this->buf.size() == 1 && this->buf.back() == 'R'){
				this->buf.clear();
				this->cur_state = state::raw_cpp_string_opening_sequence;
			}else{
				this->handle_string_parsed(listener);
				this->cur_state = state::raw_python_string_opening_sequence;
				this->sequence_index = 1;
				this->set_string_start_pos();
			}
			break;
		case '\n':
		case ' ':
		case '\r':
		case '\t':
			ASSERT(this->buf.size() != 0)
			this->handle_string_parsed(listener);
			this->cur_state = state::string_parsed;
			this->info.flags.set(treeml::flag::space);
			break;
		case '\0':
			ASSERT(this->buf.size() != 0)
			this->handle_string_parsed(listener);
			this->cur_state = state::idle;
			break;
		case '{':
			ASSERT(this->buf.size() != 0)
			this->handle_string_parsed(listener);
			this->cur_state = state::initial;
			listener.on_children_parse_started(this->cur_loc);
			++this->nesting_level;
			break;
		case '}':
			ASSERT(this->buf.size() != 0)
			this->handle_string_parsed(listener);
			this->cur_state = state::idle;
			listener.on_children_parse_finished(this->cur_loc);
			--this->nesting_level;
			break;
		default:
			this->buf.push_back(c);
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
		case '\r':
		case '\t':
			break;
		default:
			this->buf.push_back(c);
			break;
	}
}

void parser::process_char_in_escape_sequence(char c, listener& listener){
	ASSERT(this->cur_state == state::escape_sequence)
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
	this->cur_state = state::quoted_string;
}

void parser::process_char_in_comment_sequence(char c, listener& listener){
	ASSERT(this->cur_state == state::comment_seqence)
	switch(c){
		case '/':
			if(!this->buf.empty()){
				this->handle_string_parsed(listener);
			}
			this->cur_state = state::single_line_comment;
			break;
		case '*':
			if(!this->buf.empty()){
				this->handle_string_parsed(listener);
			}
			this->cur_state = state::multiline_comment;
			break;
		case '{':
			if(this->buf.empty()){
				this->set_string_start_pos();
				--this->info.location.offset;
			}
			this->buf.push_back('/');
			this->handle_string_parsed(listener);
			listener.on_children_parse_started(this->cur_loc);
			++this->nesting_level;
			this->cur_state = state::initial;
			break;
		case '\n':
		case '\r':
		case '\t':
		case ' ':
			this->buf.push_back('/');
			this->handle_string_parsed(listener);
			this->cur_state = state::string_parsed;
			break;
		default:
			this->buf.push_back('/');
			this->buf.push_back(c);
			this->cur_state = state::unquoted_string;
			break;
	}
}

void parser::process_char_in_single_line_comment(char c, listener& listener){
	ASSERT(this->cur_state == state::single_line_comment)
	this->info.flags.set(treeml::flag::space);
	switch(c){
		case '\0':
			this->cur_state = this->state_after_comment;
			this->process_char('\0', listener);
			break;
		case '\n':
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
			ASSERT(this->buf.size() == 0)
			this->buf.push_back('*');
			break;
		case '/':
			if(this->buf.size() != 0){
				ASSERT(this->buf.size() == 1)
				ASSERT(this->buf.back() == '*')
				this->buf.clear();
				this->cur_state = this->state_after_comment;
			}
			break;
		default:
			this->buf.clear();
			break;
	}
}

void parser::process_char_in_raw_cpp_string_opening_sequence(char c, listener& listener){
	ASSERT(this->cur_state == state::raw_cpp_string_opening_sequence)
	switch(c){
		case '"':
			// not a C++ style raw string, report 'R' string and a quoted string
			{
				char r = 'R';
				listener.on_string_parsed(std::string_view(&r, 1), this->info);
				this->info.flags.clear(treeml::flag::space);
			}
			++this->info.location.offset;

			if(this->buf.empty()){
				this->cur_state = state::raw_python_string_opening_sequence;
				this->sequence_index = 2;
			}else{
				this->info.flags.set(treeml::flag::quoted);
				this->handle_string_parsed(listener);
				this->cur_state = state::string_parsed;
			}
			break;
		case '(':
			this->sequenece.assign(&*this->buf.begin(), this->buf.size());
			this->buf.clear();
			this->cur_state = state::raw_cpp_string;
			this->info.flags.set(flag::raw);
			break;
		default:
			this->buf.push_back(c);
			break;
	}
}

void parser::process_char_in_raw_cpp_string(char c, listener& listener){
	ASSERT(this->cur_state == state::raw_cpp_string)
	switch(c){
		case ')':
			this->sequence_index = 0;
			this->cur_state = state::raw_cpp_string_closing_sequence;
			break;
		default:
			this->buf.push_back(c);
			break;
	}
}

void parser::process_char_in_raw_cpp_string_closing_sequence(char c, listener& listener){
	ASSERT(this->cur_state == state::raw_cpp_string_closing_sequence)
	switch(c){
		case '"':
			ASSERT(this->sequence_index <= this->sequenece.size())
			if(this->sequence_index != this->sequenece.size()){
				this->buf.push_back(')');
				for(size_t i = 0; i != this->sequence_index; ++i){
					this->buf.push_back(this->sequenece[i]);
				}
				this->cur_state = state::raw_cpp_string;
			}else{
				this->handle_string_parsed(listener);
				this->sequenece.clear();
				this->cur_state = state::string_parsed;
			}
			break;
		default:
			ASSERT(this->sequence_index <= this->sequenece.size())
			if(this->sequence_index == this->sequenece.size()
					|| c != this->sequenece[this->sequence_index])
			{
				this->buf.push_back(')');
				for(size_t i = 0; i != this->sequence_index; ++i){
					this->buf.push_back(this->sequenece[i]);
				}
				this->cur_state = state::raw_cpp_string;
			}else{
				++this->sequence_index;
			}
			break;
	}
}

void parser::process_char_in_raw_python_string_opening_sequence(char c, listener& listener){
	ASSERT(this->cur_state == state::raw_python_string_opening_sequence)
	ASSERT(this->buf.empty())
	switch(c){
		case '"':
			++this->sequence_index;
			if(this->sequence_index == 3){
				this->cur_state = state::raw_python_string;
				this->info.flags.set(flag::raw);
				this->info.flags.set(flag::raw_python_style);
			}
			break;
		default:
			this->info.flags.set(flag::quoted);
			switch(this->sequence_index){
				default:
					ASSERT(false)
				case 1:
					this->cur_state = state::quoted_string;
					this->process_char_in_quoted_string(c, listener);
					break;
				case 2:
					// empty quoted string
					this->handle_string_parsed(listener);
					this->cur_state = state::string_parsed;
					this->process_char_in_string_parsed(c, listener);
					break;
			}
			break;
	}
}

void parser::process_char_in_raw_python_string(char c, listener& listener){
	ASSERT(this->cur_state == state::raw_python_string)
	switch(c){
		case '"':
			this->cur_state = state::raw_python_string_closing_sequence;
			this->sequence_index = 1;
			break;
		default:
			this->buf.push_back(c);
			break;
	}
}

void parser::process_char_in_raw_python_string_closing_sequence(char c, listener& listener){
	ASSERT(this->cur_state == state::raw_python_string_closing_sequence)
	switch(c){
		case '"':
			++this->sequence_index;
			if(this->sequence_index == 3){
				this->handle_string_parsed(listener);
				this->cur_state = state::string_parsed;
			}
			break;
		default:
			this->buf.insert(this->buf.end(), this->sequence_index, '"');
			this->buf.push_back(c);
			this->cur_state = state::raw_python_string;
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
		case state::raw_cpp_string_opening_sequence:
			this->process_char_in_raw_cpp_string_opening_sequence(c, listener);
			break;
		case state::raw_cpp_string:
			this->process_char_in_raw_cpp_string(c, listener);
			break;
		case state::raw_cpp_string_closing_sequence:
			this->process_char_in_raw_cpp_string_closing_sequence(c, listener);
			break;
		case state::raw_python_string_opening_sequence:
			this->process_char_in_raw_python_string_opening_sequence(c, listener);
			break;
		case state::raw_python_string:
			this->process_char_in_raw_python_string(c, listener);
			break;
		case state::raw_python_string_closing_sequence:
			this->process_char_in_raw_python_string_closing_sequence(c, listener);
			break;
		default:
			ASSERT(false, [&](auto&o){o << "this->cur_state = " << unsigned(this->cur_state);})
			break;
	}
}

void parser::parse_data_chunk(utki::span<const uint8_t> chunk, listener& listener){
	for(auto c : chunk){
		if(c == '\n'){
			this->next_line();
		}
		this->process_char(c, listener);
		++this->cur_loc.offset;
	}
}

void parser::end_of_data(listener& listener){
	this->process_char('\0', listener);

	if(this->nesting_level != 0){
		throw std::invalid_argument("Malformed treeml document fed. Document end reached while parsing children block.");
	}

	if(this->cur_state != state::idle){
		throw std::invalid_argument("Malformed treeml document fed. After parsing all the data, the parser remained in the middle of some parsing task.");
	}

	this->reset();
}

void treeml::parse(const papki::file& fi, listener& listener){
	papki::file::guard file_guard(fi);

	treeml::parser parser;

	std::array<std::uint8_t, file_read_chunk_size> buf; // 2kb read buffer.

	size_t bytesRead;

	do{
		bytesRead = fi.read(utki::make_span(buf));

		parser.parse_data_chunk(utki::make_span(&*buf.begin(), bytesRead), listener);
	}while(bytesRead == buf.size());

	parser.end_of_data(listener);
}

void parser::reset(){
	this->buf.clear();
	this->nesting_level = 0;
	this->cur_state = state::initial;
}
