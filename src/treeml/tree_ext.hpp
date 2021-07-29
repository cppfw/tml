/*
The MIT License (MIT)

Copyright (c) 2012-2021 Ivan Gagis <igagis@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/* ================ LICENSE END ================ */

#pragma once

#include "tree.hpp"
#include "extra_info.hpp"

namespace treeml{

class leaf_ext : public leaf{
public:
	extra_info info;

	leaf_ext(const char* str) :
			leaf(str)
	{}

	leaf_ext(const std::string& str) :
			leaf(str)
	{}

	leaf_ext(std::string&& str) :
			leaf(std::move(str))
	{}

	leaf_ext(const char* str, const extra_info& info) :
			leaf(str),
			info(info)
	{}

	leaf_ext(const std::string& str, const extra_info& info) :
			leaf(str),
			info(info)
	{}

	leaf_ext(std::string&& str, const extra_info& info) :
			leaf(std::move(str)),
			info(info)
	{}
};

typedef utki::tree<leaf_ext> tree_ext;
typedef tree_ext::container_type forest_ext;

forest_ext read_ext(const papki::file& fi);
forest_ext read_ext(const std::string& str);

tree to_non_ext(const tree_ext& t);
forest to_non_ext(const forest_ext& f);

}
