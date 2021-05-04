#pragma once

#include "tree.hpp"
#include "extra_info.hpp"

namespace treeml{

class leaf_ext : public leaf{
	extra_info info;
public:
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

	const extra_info& get_info()const noexcept{
		return this->info;
	}
};

typedef utki::tree<leaf_ext> tree_ext;
typedef tree_ext::container_type forest_ext;

forest_ext read_ext(const papki::file& fi);
forest_ext read_ext(const std::string& str);

tree to_non_ext(const tree_ext& t);
forest to_non_ext(const forest_ext& f);

}
