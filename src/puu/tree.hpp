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

    branch& get()noexcept{
        ASSERT(this->i != this->b.end())
        return *this->i;
    }

    crawler& to(const std::string& str);

    crawler& next();

    crawler up();
};

}
