/*
The MIT License (MIT)

Copyright (c) 2012-2025 Ivan Gagis <igagis@gmail.com>

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

#include "tree_ext.hpp"

#include <stack>

#include <papki/span_file.hpp>

#include "parser.hpp"

using namespace tml;

forest_ext tml::read_ext(const papki::file& fi)
{
	class the_listener : public tml::listener
	{
		std::stack<forest_ext> stack;

	public:
		forest_ext cur_forest;

		void on_children_parse_started(location loc) override
		{
			this->stack.push(std::move(this->cur_forest));
			ASSERT(this->cur_forest.size() == 0)
		}

		void on_children_parse_finished(location loc) override
		{
			if (this->stack.size() == 0) {
				std::stringstream ss;
				ss << "malformed tml: unopened curly brace encountered at ";
				ss << loc.line << ":" << loc.offset;
				throw std::invalid_argument(ss.str());
			}
			this->stack.top().back().children = std::move(this->cur_forest);
			this->cur_forest = std::move(this->stack.top());
			this->stack.pop();
		}

		void on_string_parsed(std::string_view str, const extra_info& info) override
		{
			this->cur_forest.emplace_back(leaf_ext(std::string(str.data(), str.size()), info));
		}
	} listener;

	tml::parse(fi, listener);

	return std::move(listener.cur_forest);
}

forest_ext tml::read_ext(const std::string& str)
{
	const papki::span_file fi(to_uint8_t(utki::make_span(str)));

	return read_ext(fi);
}

tree tml::to_non_ext(const tree_ext& t)
{
	tree ret;

	ret.value = static_cast<const leaf&>(t.value);
	ret.children = to_non_ext(t.children);

	return ret;
}

forest tml::to_non_ext(const forest_ext& f)
{
	forest ret;

	for (const auto& c : f) {
		ret.push_back(to_non_ext(c));
	}

	return ret;
}
