#include "tree.hpp"

#include "parser.hpp"

#include <stack>

using namespace puu;


branches grow(const papki::File& fi){
	class the_listener : public puu::listener{
		std::stack<branches> stack;

	public:
		branches cur_branches;

		void on_children_parse_started()override{
			this->stack.push(std::move(this->cur_branches));
            ASSERT(this->cur_branches.size() == 0)
		}

        void on_children_parse_finished()override{
            ASSERT(this->stack.size() != 0)
            this->stack.top().back().branches() = std::move(this->cur_branches);
            this->cur_branches = std::move(this->stack.top());
            this->stack.pop();
		}

		void on_string_parsed(const utki::Buf<char> str)override{
			this->cur_branches.push_back(std::string(str.begin(), str.size()));
		}
	} listener;

	puu::parse(fi, listener);

	return std::move(listener.cur_branches);
}
