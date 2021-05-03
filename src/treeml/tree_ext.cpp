#include "tree_ext.hpp"

#include <stack>
#include <papki/span_file.hpp>

#include "parser.hpp"

using namespace treeml;

forest_ext treeml::read_ext(const papki::file& fi){
	class the_listener : public treeml::listener{
		std::stack<forest_ext> stack;

	public:
		forest_ext cur_forest;

		void on_children_parse_started()override{
			this->stack.push(std::move(this->cur_forest));
            ASSERT(this->cur_forest.size() == 0)
		}

        void on_children_parse_finished()override{
            ASSERT(this->stack.size() != 0)
            this->stack.top().back().children = std::move(this->cur_forest);
            this->cur_forest = std::move(this->stack.top());
            this->stack.pop();
		}

		void on_string_parsed(std::string_view str, const extra_info& info)override{
			this->cur_forest.emplace_back(leaf_ext(std::string(str.data(), str.size()), info));
		}
	} listener;

	treeml::parse(fi, listener);

	return std::move(listener.cur_forest);
}

forest_ext treeml::read_ext(const std::string& str){
	const papki::span_file fi(utki::make_span(reinterpret_cast<const std::uint8_t*>(str.data()), str.size()));

	return read_ext(fi);
}

tree treeml::to_non_ext(const tree_ext& t){
	tree ret;

	ret.value = t.value;
	ret.children = to_non_ext(t.children);

	return ret;
}

forest treeml::to_non_ext(const forest_ext& f){
	forest ret;

	for(const auto& c : f){
		ret.push_back(to_non_ext(c));
	}

	return ret;
}
