#pragma once

#include <utki/tree.hpp>

#include <papki/File.hpp>

#include <string>

//TODO: doxygen
namespace puu{

typedef utki::tree<std::string> branch;
typedef branch::container_type branches;

branches read(const papki::File& fi);

void write(const branches& roots, papki::File& fi, bool formatted);

}
