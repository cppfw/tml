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
};

typedef utki::tree<leaf> tree;
typedef tree::container_type forest;

forest read(const papki::file& fi);

forest read(const char* str);

inline forest read(const std::string& str){
	return read(str.c_str());
}

enum class formatting{
    normal,
    minimal
};

void write(const forest& wood, papki::file& fi, formatting fmt = formatting::normal);

std::string to_string(const forest& f);

inline std::string to_string(const tree& t){
    return to_string(treeml::forest({t}));
}

class crawler{
    forest& b;
    forest::iterator i;
public:
    crawler(forest& b) :
            b(b),
            i(b.begin())
    {
        if(b.size() == 0){
            throw std::logic_error("crawler::crawler() failed, reached end of node list");
        }
    }

    crawler(const forest& b) :
            crawler(const_cast<forest&>(b))
    {}

    tree& get()noexcept{
        ASSERT(this->i != this->b.end())
        return *this->i;
    }

    const tree& get()const noexcept{
        ASSERT(this->i != this->b.end())
        return *this->i;
    }

    crawler& to(const std::string& str);

    const crawler& to(const std::string& str)const{
        return const_cast<crawler*>(this)->to(str);
    }

    template <class predicate_type> crawler& to_if(predicate_type p){
        this->i = std::find_if(this->i, this->b.end(), p);
        if(this->i != this->b.end()){
            return *this;
        }
        throw std::runtime_error("crawler::to_if() failed, reached end of node list");
    }

    template <class predicate_type> const crawler& to_if(predicate_type p)const{
        return const_cast<crawler*>(this)->to_if(p);
    }

    crawler& next();

    const crawler& next()const{
        return const_cast<crawler*>(this)->next();
    }

    crawler in();

    const crawler in()const{
        return const_cast<crawler*>(this)->in();
    }
};

typedef const crawler const_crawler;

}
