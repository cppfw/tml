#pragma once

#include <utki/tree.hpp>

#include <papki/file.hpp>

#include <string>

//TODO: doxygen
namespace treeml{

enum class base{
    dec,
    oct,
    hex
};

class leaf{
    std::string string;
public:
    leaf() = default;
    leaf(const leaf&) = default;
    leaf(leaf&&) = default;
    leaf& operator=(const leaf&) = default;

    leaf(const char* str) :
            string(str)
    {}

    leaf(const std::string& str) :
            string(str)
    {}

    leaf(std::string&& str) :
            string(std::move(str))
    {}

    bool operator==(const char* str)const{
        return this->string == str;
    }

    bool operator!=(const char* str)const{
        return this->string != str;
    }

    bool operator==(const std::string& str)const{
        return this->string == str;
    }

    bool operator!=(const std::string& str)const{
        return this->string != str;
    }

    bool operator==(const leaf& l)const{
        return this->string == l.string;
    }

    bool operator!=(const leaf& l)const{
        return this->string != l.string;
    }

    const char* c_str()const noexcept{
        return this->string.c_str();
    }

    size_t length()const noexcept{
        return this->string.length();
    }

    bool empty()const noexcept{
        return this->string.empty();
    }

    char operator[](size_t i)const noexcept{
        return this->string[i];
    }

    const std::string& to_string()const noexcept{
        return this->string;
    }

    explicit leaf(bool value);

    explicit leaf(unsigned char value, base conversion_base = base::dec);
    explicit leaf(unsigned short int value, base conversion_base = base::dec);

    explicit leaf(signed int value);
    explicit leaf(unsigned int value, base conversion_base = base::dec);

    explicit leaf(signed long int value);
    explicit leaf(unsigned long int value, base conversion_base = base::dec);

    explicit leaf(signed long long int value);
    explicit leaf(unsigned long long int value, base conversion_base = base::dec);

    explicit leaf(float value);
    explicit leaf(double value);
    explicit leaf(long double value);

    bool to_bool()const;

    int32_t to_int32()const{
        return int32_t(std::stoi(this->string, nullptr, 0));
    }
    uint32_t to_uint32()const{
        return uint32_t(std::stoul(this->string, nullptr, 0));
    }

    int64_t to_int64()const{
        return int64_t(std::stoll(this->string, nullptr, 0));
    }
    uint64_t to_uint64()const{
        return uint64_t(std::stoull(this->string, nullptr, 0));
    }

    float to_float()const{
        return std::stof(this->string);
    }
    double to_double()const{
        return std::stod(this->string);
    }
    long double to_long_double()const{
        return std::stold(this->string);
    }

	friend std::ostream& operator<<(std::ostream& o, const leaf& l){
		o << l.to_string();
		return o;
	}
};

typedef utki::tree<leaf> tree;
typedef tree::container_type forest;

forest read(const papki::file& fi);
forest read(const std::string& str);

enum class formatting{
    normal,
    minimal
};

void write(const forest& wood, papki::file& fi, formatting fmt = formatting::normal);

std::string to_string(const forest& f);

inline std::string to_string(const tree& t){
    return to_string(treeml::forest({t}));
}

inline std::ostream& operator<<(std::ostream& o, const forest& f){
	o << to_string(f);
	return o;
}

}
