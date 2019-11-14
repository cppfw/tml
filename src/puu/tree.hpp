#pragma once

#include <utki/tree.hpp>

#include <papki/File.hpp>

#include <string>

#include "exception.hpp"

//TODO: doxygen
namespace puu{

typedef utki::tree<std::string> branch;
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
    {}

    branch& operator*()noexcept{
        return *this->i;
    }

    branches::iterator operator->()noexcept{
        return this->i;
    }

    crawler find(const std::string& str);
};

}
