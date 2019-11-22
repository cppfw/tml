#pragma once

#include <utki/tree.hpp>

#include <papki/File.hpp>

#include <string>

#include "exception.hpp"

//TODO: doxygen
namespace puu{

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

    bool operator==(const std::string& str)const{
        return this->string == str;
    }

    bool operator==(const leaf& l)const{
        return this->string == l.string;
    }

    const char* c_str()const noexcept{
        return this->string.c_str();
    }

    const std::string& to_string()const noexcept{
        return this->string;
    }

    explicit leaf(bool value);

    // explicit leaf(signed char value);
    // explicit leaf(unsigned char value);
    // explicit leaf(signed short value);
    // explicit leaf(unsigned short value);
    explicit leaf(signed int value);
    explicit leaf(unsigned int value, std::ios_base&(*base)(std::ios_base&) = std::dec);
    explicit leaf(signed long int value);
    explicit leaf(unsigned long int value, std::ios_base&(*base)(std::ios_base&) = std::dec);
    explicit leaf(signed long long int value);
    explicit leaf(unsigned long long int value, std::ios_base&(*base)(std::ios_base&) = std::dec);
    explicit leaf(float value);
    explicit leaf(double value);
    explicit leaf(long double value);

    bool to_bool()const;
    int32_t to_int32()const;
    uint32_t to_uint32()const;
    int64_t to_int64()const;
    uint64_t to_uint64()const;
    float to_float()const;
    double to_double()const;
    long double to_long_double()const;
};

typedef utki::tree<leaf> branch;
typedef branch::container_type branches;

branches read(const papki::File& fi);

void write(const branches& roots, papki::File& fi, bool formatted);

class not_found_exception : public puu::exception{
public:
    not_found_exception(const std::string& message) :
            exception(message)
    {}
};

class crawler{
    branches& b;
    branches::iterator i;
public:
    crawler(branches& b) :
            b(b),
            i(b.begin())
    {
        if(b.size() == 0){
            throw puu::not_found_exception("crawler::crawler() failed, reached end of node list");
        }
    }

    crawler(const branches& b) :
            crawler(const_cast<branches&>(b))
    {}

    branch& get()noexcept{
        ASSERT(this->i != this->b.end())
        return *this->i;
    }

    const branch& get()const noexcept{
        ASSERT(this->i != this->b.end())
        return *this->i;
    }

    crawler& to(const std::string& str);

    const crawler& to(const std::string& str)const{
        return const_cast<crawler*>(this)->to(str);
    }

    template <class Predicate> crawler& to_if(Predicate p){
        this->i = std::find_if(this->i, this->b.end(), p);
        if(this->i != this->b.end()){
            return *this;
        }
        throw puu::not_found_exception("crawler::to() failed, reached end of node list");
    }

    template <class Predicate> const crawler& to_if(Predicate p)const{
        return const_cast<crawler*>(this)->to_if(p);
    }

    crawler& next();

    const crawler& next()const{
        return const_cast<crawler*>(this)->next();
    }

    crawler up();

    const crawler up()const{
        return const_cast<crawler*>(this)->up();
    }
};

typedef const crawler const_crawler;

}
