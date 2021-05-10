#include "tree.hpp"

#include "parser.hpp"

#include <stack>
#include <cinttypes>
#include <cstring>

#include <papki/span_file.hpp>
#include <papki/vector_file.hpp>

using namespace treeml;

forest treeml::read(const papki::file& fi){
	class the_listener : public treeml::listener{
		std::stack<forest> stack;

	public:
		forest cur_forest;

		void on_children_parse_started(location loc)override{
			this->stack.push(std::move(this->cur_forest));
            ASSERT(this->cur_forest.size() == 0)
		}

        void on_children_parse_finished(location loc)override{
            if(this->stack.size() == 0){
				std::stringstream ss;
				ss << "malformed treeml: unopened curly brace encountered at ";
				ss << loc.line << ":" << loc.offset;
				throw std::invalid_argument(ss.str());
			}
            this->stack.top().back().children = std::move(this->cur_forest);
            this->cur_forest = std::move(this->stack.top());
            this->stack.pop();
		}

		void on_string_parsed(std::string_view str, const extra_info& info)override{
			this->cur_forest.emplace_back(std::string(str.data(), str.size()));
		}
	} listener;

	treeml::parse(fi, listener);

	return std::move(listener.cur_forest);
}

forest treeml::read(const std::string& str){
	const papki::span_file fi(utki::make_span(reinterpret_cast<const std::uint8_t*>(str.data()), str.size()));

	return read(fi);
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

void make_escaped_string(const char* str, utki::span<std::uint8_t> out){
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

void write_internal(const treeml::forest& roots, papki::file& fi, formatting fmt, unsigned indentation){
    const std::array<std::uint8_t, 1> quote = {{'"'}};
	const std::array<std::uint8_t, 1> lcurly = {{'{'}};
	const std::array<std::uint8_t, 1> rcurly = {{'}'}};
	const std::array<std::uint8_t, 1> space = {{' '}};
	const std::array<std::uint8_t, 1> tab = {{'\t'}};
	const std::array<std::uint8_t, 1> newLine = {{'\n'}};

	// used to detect case of two adjacent unquoted strings without children, need to insert space between them
	bool prev_was_unquoted_without_children = false;

	bool prev_had_children = true;

	for(auto& n : roots){
		//indent
		if(fmt == formatting::normal){
			for(unsigned i = 0; i != indentation; ++i){
				fi.write(utki::make_span(tab));
			}
		}

		//write node value

//		TRACE(<< "writing node: " << n.value.c_str() << std::endl)

		unsigned num_escapes;
		size_t length;
		bool unqouted = can_string_be_unquoted(n.value.c_str(), length, num_escapes);

		if(!unqouted){
			fi.write(utki::make_span(quote));

			if(num_escapes == 0){
				fi.write(utki::make_span(
						reinterpret_cast<const uint8_t*>(n.value.c_str()),
						length
					));
			}else{
				std::vector<uint8_t> buf(length + num_escapes);

				make_escaped_string(n.value.c_str(), utki::make_span(buf));

				fi.write(utki::make_span(buf));
			}

			fi.write(utki::make_span(quote));
		}else{
			bool is_quoted_empty_string = false;

			if(length == 0){
				if(n.children.size() == 0 || !prev_had_children){
					is_quoted_empty_string = true;
				}
			}

			// if the string is unquoted then write space in case the output is unformatted
			if(fmt != formatting::normal && prev_was_unquoted_without_children && !is_quoted_empty_string){
				fi.write(utki::make_span(space));
			}

			if(length == 0){
				if(is_quoted_empty_string){
					fi.write(utki::make_span(quote));
					fi.write(utki::make_span(quote));
				}
			}else{
				ASSERT(num_escapes == 0)
				fi.write(utki::make_span(
						reinterpret_cast<const uint8_t*>(n.value.c_str()),
						length
					));
				ASSERT(n.value.to_string().length() != 0)
				if(n.children.size() == 0 && length == 1 && n.value.c_str()[0] == 'R'){
					fi.write(utki::make_span(space));
				}
			}
		}

		prev_had_children = (n.children.size() != 0);

		if(n.children.size() == 0){
			if(fmt == formatting::normal){
				fi.write(utki::make_span(newLine));
			}
			prev_was_unquoted_without_children = (unqouted && length != 0);
			continue;
		}else{
			prev_was_unquoted_without_children = false;
		}

		if(fmt != formatting::normal){
			fi.write(utki::make_span(lcurly));

			write_internal(n.children, fi, fmt, 0);

			fi.write(utki::make_span(rcurly));
		}else{
			fi.write(utki::make_span(lcurly));

			if(n.children.size() == 1 && n.children[0].children.size() == 0){
				// if only one child and that child has no children then write the only child on the same line
				write_internal(n.children, fi, formatting::minimal, 0);
			}else{
				fi.write(utki::make_span(newLine));
				write_internal(n.children, fi, fmt, indentation + 1);

				// indent
				for(unsigned i = 0; i != indentation; ++i){
					fi.write(utki::make_span(tab));
				}
			}
			fi.write(utki::make_span(rcurly));
			fi.write(utki::make_span(newLine));
		}
	}
}
}

void treeml::write(const treeml::forest& wood, papki::file& fi, formatting fmt){
    papki::file::guard file_guard(fi, papki::file::mode::create);

    write_internal(wood, fi, fmt, 0);
}

leaf::leaf(bool value) :
		string(value ? "true" : "false")
{}

leaf::leaf(int value) :
		string([](int value) -> std::string{
			// TRACE(<< "leaf::leaf(int): value = " << value << std::endl)
			char buf[64];

			int res = snprintf(buf, sizeof(buf), "%d", value);

			if(0 <= res && res <= int(sizeof(buf))){
				return std::string(buf, res);
			}
			return std::string();
		}(value))
{}

leaf::leaf(unsigned char value, base conversion_base) :
		leaf((unsigned short int)value, conversion_base)
{}

leaf::leaf(unsigned short int value, base conversion_base) :
		leaf((unsigned int)value, conversion_base)
{}

leaf::leaf(unsigned int value, base conversion_base) :
		string([](unsigned int value, base conversion_base) -> std::string{
			// TRACE(<< "leaf::leaf(uint): value = " << value <<", base = " << int(conversion_base) << std::endl)
			char buf[64];

			const char* format;
			switch(conversion_base){
				case base::oct:
					format = "0%o";
					break;
				case base::hex:
					format = "0x%x";
					break;
				default:
				case base::dec:
					format = "%u";
					break;
			}

			int res = snprintf(buf, sizeof(buf), format, value);

			if(0 <= res && res <= int(sizeof(buf))){
				return std::string(buf, res);
			}
			return std::string();
		}(value, conversion_base))
{}

leaf::leaf(signed long int value) :
		string([](long int value) -> std::string{
			// TRACE(<< "leaf::leaf(long int): value = " << value << std::endl)
			char buf[64];

			int res = snprintf(buf, sizeof(buf), "%ld", value);

			if(0 <= res && res <= int(sizeof(buf))){
				return std::string(buf, res);
			}
			return std::string();
		}(value))
{}

leaf::leaf(unsigned long int value, base conversion_base) :
		string([](unsigned long int value, base conversion_base) -> std::string{
			// TRACE(<< "leaf::leaf(ulong): value = " << value << ", base = " << int(conversion_base) << std::endl)
			char buf[64];

			const char* format;
			switch(conversion_base){
				case base::oct:
					format = "0%lo";
					break;
				case base::hex:
					format = "0x%lx";
					break;
				default:
				case base::dec:
					format = "%lu";
					break;
			}

			int res = snprintf(buf, sizeof(buf), format, value);

			if(0 <= res && res <= int(sizeof(buf))){
				return std::string(buf, res);
			}
			return std::string();
		}(value, conversion_base))
{}

leaf::leaf(signed long long int value) :
		string([](long long int value) -> std::string{
			// TRACE(<< "leaf::leaf(long long): value = " << value << std::endl)
			char buf[64];

			int res = snprintf(buf, sizeof(buf), "%lld", value);

			if(0 <= res && res <= int(sizeof(buf))){
				return std::string(buf, res);
			}
			return std::string();
		}(value))
{}

leaf::leaf(unsigned long long int value, base conversion_base) :
		string([](unsigned long long int value, base conversion_base) -> std::string{
			// TRACE(<< "leaf::leaf(u long long): value = " << value << ", base = " << int(conversion_base) << std::endl)
			char buf[64];

			const char* format;
			switch(conversion_base){
				case base::oct:
					format = "0%llo";
					break;
				case base::hex:
					format = "0x%llx";
					break;
				default:
				case base::dec:
					format = "%llu";
					break;
			}

			int res = snprintf(buf, sizeof(buf), format, value);

			if(0 <= res && res <= int(sizeof(buf))){
				return std::string(buf, res);
			}
			return std::string();
		}(value, conversion_base))
{}

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

leaf::leaf(double value) :
		string([](double value) -> std::string{
			char buf[64];

			int res = snprintf(buf, sizeof(buf), "%.17G", value);

			if(res < 0 || res > int(sizeof(buf))){
				return std::string();
			}else{
				return std::string(buf, res);
			}
		}(value))
{}

leaf::leaf(long double value) :
		string([](long double value) -> std::string{
			char buf[128];

			int res = snprintf(buf, sizeof(buf), "%.31LG", value);

			if(res < 0 || res > int(sizeof(buf))){
				return std::string();
			}else{
				return std::string(buf, res);
			}
		}(value))
{}

bool leaf::to_bool()const{
	return this->string == "true";
}

std::string treeml::to_string(const forest& f){
	papki::vector_file fi;
	treeml::write(f, fi, treeml::formatting::minimal);
	auto v = fi.reset_data();
	return std::string(reinterpret_cast<char*>(v.data()), v.size());
}
