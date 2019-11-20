#include "tree.hpp"

#include "parser.hpp"

#include <stack>
#include <cinttypes>

using namespace puu;


branches puu::read(const papki::File& fi){
	class the_listener : public puu::listener{
		std::stack<branches> stack;

	public:
		branches cur_branches;

		void on_children_parse_started()override{
			this->stack.push(std::move(this->cur_branches));
            ASSERT(this->cur_branches.size() == 0)
		}

        void on_children_parse_finished()override{
            ASSERT(this->stack.size() != 0)
            this->stack.top().back().children = std::move(this->cur_branches);
            this->cur_branches = std::move(this->stack.top());
            this->stack.pop();
		}

		void on_string_parsed(const utki::Buf<char> str)override{
			this->cur_branches.emplace_back(std::string(str.begin(), str.size()));
		}
	} listener;

	puu::parse(fi, listener);

	return std::move(listener.cur_branches);
}

namespace{
bool can_string_be_unquoted(const char* s, size_t& out_length, unsigned& out_num_escapes){
//	TRACE(<< "CanStringBeUnquoted(): enter" << std::endl)

	out_num_escapes = 0;
	out_length = 0;

	if(s == 0){
		// empty string can be unquoted when it has children, so return true.
		return true;
	}

	bool ret = true;
	for(; *s != 0; ++s, ++out_length){
//		TRACE(<< "CanStringBeUnquoted(): c = " << (*c) << std::endl)
		switch(*s){
			case '\t':
			case '\n':
			case '\r':
			case '\\':
			case '"':
				++out_num_escapes;
			case '{':
			case '}':
			case ' ':
				ret = false;
				break;
			default:
				break;
		}
	}
	return ret;
}

void make_escaped_string(const char* str, utki::Buf<std::uint8_t> out){
	std::uint8_t *p = out.begin();
	for(const char* c = str; *c != 0; ++c){
		ASSERT(p != out.end())

		switch(*c){
			case '\t':
				*p = '\\';
				++p;
				ASSERT(p != out.end())
				*p = 't';
				break;
			case '\n':
				*p = '\\';
				++p;
				ASSERT(p != out.end())
				*p = 'n';
				break;
			case '\r':
				*p = '\\';
				++p;
				ASSERT(p != out.end())
				*p = 'r';
				break;
			case '\\':
				*p = '\\';
				++p;
				ASSERT(p != out.end())
				*p = '\\';
				break;
			case '"':
				*p = '\\';
				++p;
				ASSERT(p != out.end())
				*p = '"';
				break;
			default:
				*p = *c;
				break;
		}
		++p;
	}
}

void write_internal(const puu::branches& roots, papki::File& fi, bool formatted, unsigned indentation){
    const std::array<std::uint8_t, 1> quote = {{'"'}};
	const std::array<std::uint8_t, 1> lcurly = {{'{'}};
	const std::array<std::uint8_t, 1> rcurly = {{'}'}};
	const std::array<std::uint8_t, 1> space = {{' '}};
	const std::array<std::uint8_t, 1> tab = {{'\t'}};
	const std::array<std::uint8_t, 1> newLine = {{'\n'}};

	//used to detect case of two adjacent unquoted strings without children, need to insert space between them
	bool prev_was_unquoted_without_children = false;

	bool prev_had_children = true;

	for(auto& n : roots){
		//indent
		if(formatted){
			for(unsigned i = 0; i != indentation; ++i){
				fi.write(utki::wrapBuf(tab));
			}
		}

		//write node value

		unsigned num_escapes;
		size_t length;
		bool unqouted = can_string_be_unquoted(n.value.str().c_str(), length, num_escapes);

		if(!unqouted){
			fi.write(utki::wrapBuf(quote));

			if(num_escapes == 0){
				fi.write(utki::Buf<uint8_t>(
						const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(n.value.str().c_str())),
						length
					));
			}else{
				std::vector<uint8_t> buf(length + num_escapes);

				make_escaped_string(n.value.str().c_str(), utki::wrapBuf(buf));

				fi.write(utki::wrapBuf(buf));
			}

			fi.write(utki::wrapBuf(quote));
		}else{
			bool is_quoted_empty_string = false;

			if(length == 0){
				if(n.children.size() == 0 || !prev_had_children){
					is_quoted_empty_string = true;
				}
			}

			// if the string is unquoted then write space in case the output is unformatted
			if(!formatted && prev_was_unquoted_without_children && !is_quoted_empty_string){
				fi.write(utki::wrapBuf(space));
			}

			if(length == 0){
				if(is_quoted_empty_string){
					fi.write(utki::wrapBuf(quote));
					fi.write(utki::wrapBuf(quote));
				}
			}else{
				ASSERT(num_escapes == 0)
				fi.write(utki::Buf<std::uint8_t>(
						const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(n.value.str().c_str())),
						length
					));
                ASSERT(n.value.length() != 0)
				if(n.children.size() == 0 && length == 1 && n.value.str()[0] == 'R'){
					fi.write(utki::wrapBuf(space));
				}
			}
		}

		prev_had_children = (n.children.size() != 0);

		if(n.children.size() == 0){
			if(formatted){
				fi.write(utki::wrapBuf(newLine));
			}
			prev_was_unquoted_without_children = (unqouted && length != 0);
			continue;
		}else{
			prev_was_unquoted_without_children = false;
		}

		if(!formatted){
			fi.write(utki::wrapBuf(lcurly));

			write_internal(n.children, fi, false, 0);

			fi.write(utki::wrapBuf(rcurly));
		}else{
			fi.write(utki::wrapBuf(lcurly));

			if(n.children.size() == 1 && n.children[0].children.size() == 0){
				// if only one child and that child has no children then write the only child on the same line
				write_internal(n.children, fi, false, 0);
			}else{
				fi.write(utki::wrapBuf(newLine));
				write_internal(n.children, fi, true, indentation + 1);

				//indent
				for(unsigned i = 0; i != indentation; ++i){
					fi.write(utki::wrapBuf(tab));
				}
			}
			fi.write(utki::wrapBuf(rcurly));
			fi.write(utki::wrapBuf(newLine));
		}
	}//~for
}
}

void puu::write(const puu::branches& roots, papki::File& fi, bool formatted){
    papki::File::Guard fileGuard(fi, papki::File::E_Mode::CREATE);

    write_internal(roots, fi, formatted, 0);
}

crawler crawler::up(){
	ASSERT(this->i != this->b.end())
	if(this->get().children.size() == 0){
		throw puu::not_found_exception("crawler::up() failed, node has no children");
	}
	return crawler(this->get().children);
}

crawler& crawler::next(){
	ASSERT(this->i != this->b.end())
	++this->i;
	if(this->i == this->b.end()){
		throw puu::not_found_exception("crawler::next() failed, reached end of node list");
	}
	return *this;
}

crawler& crawler::to(const std::string& str){
	this->i = std::find(this->i, this->b.end(), str);
	if(this->i != this->b.end()){
		return *this;
	}
	throw puu::not_found_exception("crawler::to() failed, reached end of node list");
}

leaf::leaf(bool value) :
		string(value ? "true" : "false")
{}

bool leaf::to_bool()const{
	return this->str() == "true";
}

leaf::leaf(int32_t value) :
		string([](int32_t value) -> std::string{
			char buf[64];

			int res = snprintf(buf, sizeof(buf), "%" PRIi32, value);

			if(0 <= res && res <= int(sizeof(buf))){
				return std::string(buf, res);
			}
			return std::string();
		}(value))
{}

int32_t leaf::to_int32()const{
	return int32_t(strtol(this->str().c_str(), nullptr, 0));
}

leaf::leaf(uint32_t value, std::ios_base&(*base)(std::ios_base&)) :
		string([](int32_t value, std::ios_base&(*base)(std::ios_base&)) -> std::string{
			char buf[64];

			const char* format;
			if(base == std::oct){
				format = "0%" PRIo32;
			}else if(base == std::hex){
				format = "0x%" PRIx32;
			}else{ // std::dec
				format = "%" PRIu32;
			}

			int res = snprintf(buf, sizeof(buf), format, value);

			if(0 <= res && res <= int(sizeof(buf))){
				return std::string(buf, res);
			}
			return std::string();
		}(value, base))
{}

uint32_t leaf::to_uint32()const{
	return uint32_t(strtoul(this->str().c_str(), nullptr, 0));
}


leaf::leaf(int64_t value) :
		string([](int64_t value) -> std::string{
			char buf[64];

			int res = snprintf(buf, sizeof(buf), "%" PRIi64, value);

			if(0 <= res && res <= int(sizeof(buf))){
				return std::string(buf, res);
			}
			return std::string();
		}(value))
{}

int64_t leaf::to_int64()const{
	return int64_t(strtoll(this->str().c_str(), nullptr, 0));
}

leaf::leaf(uint64_t value, std::ios_base&(*base)(std::ios_base&)) :
		string([](uint64_t value, std::ios_base&(*base)(std::ios_base&)) -> std::string{
			char buf[64];

			const char* format;
			if(base == std::oct){
				format = "0%" PRIo64;
			}else if(base == std::hex){
				format = "0x%" PRIx64;
			}else{ // std::dec
				format = "%" PRIu64;
			}

			int res = snprintf(buf, sizeof(buf), format, value);

			if(0 <= res && res <= int(sizeof(buf))){
				return std::string(buf, res);
			}
			return std::string();
		}(value, base))
{}

uint64_t leaf::to_uint64()const{
	return uint64_t(strtoull(this->str().c_str(), nullptr, 0));
}

leaf::leaf(float value) :
		string([](float value) -> std::string{
			char buf[64];

			int res = snprintf(buf, sizeof(buf), "%.8G", double(value));

			if(res < 0 || res > int(sizeof(buf))){
				return std::string();
			}else{
				return std::string(buf, res);
			}
		}(value))
{}

float leaf::to_float()const{
	return strtof(this->str().c_str(), nullptr);
}
