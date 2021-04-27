#pragma once

#include "tree.hpp"
namespace treeml{

class leaf_ext : public leaf{
	extra_info info;
public:
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

}
