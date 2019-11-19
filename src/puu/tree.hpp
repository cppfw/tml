#pragma once

#include <utki/tree.hpp>

#include <papki/File.hpp>

#include <string>

#include "exception.hpp"

//TODO: doxygen
namespace puu{

class leaf : public std::string{
public:
    leaf(const std::string& str) :
            std::string(str)
    {}

    leaf(std::string&& str) :
            std::string(std::move(str))
    {}

    leaf& operator=(const std::string& str){
        this->std::string::operator=(str);
        return *this;
    }

    // TODO: add methods to get as int etc.

    // TODO: add set methods
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
