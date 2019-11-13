#pragma once

#include <utki/tree.hpp>

#include <papki/File.hpp>

#include <string>

namespace puu{

typedef utki::tree<std::string>::container_type branches;

branches grow(const papki::File& fi);

void write(const branches& roots, papki::File& fi, bool formatted);

}
