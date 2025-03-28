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

#pragma once

#include "tree.hpp"

namespace tml {

class crawler
{
	forest& b;
	forest::iterator i;

public:
	crawler(forest& b) :
		b(b),
		i(b.begin())
	{
		if (b.size() == 0) {
			throw std::logic_error("crawler::crawler() failed, reached end of node list");
		}
	}

	crawler(const forest& b) :
		// NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
		crawler(const_cast<forest&>(b))
	{}

	tree& get() noexcept
	{
		ASSERT(this->i != this->b.end())
		return *this->i;
	}

	const tree& get() const noexcept
	{
		ASSERT(this->i != this->b.end())
		return *this->i;
	}

	crawler& to(const std::string& str);

	const crawler& to(const std::string& str) const
	{
		// NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
		return const_cast<crawler*>(this)->to(str);
	}

	template <class predicate_type>
	crawler& to_if(predicate_type p)
	{
		this->i = std::find_if(this->i, this->b.end(), p);
		if (this->i != this->b.end()) {
			return *this;
		}
		throw std::runtime_error("crawler::to_if() failed, reached end of node list");
	}

	template <class predicate_type>
	const crawler& to_if(predicate_type p) const
	{
		// NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
		return const_cast<crawler*>(this)->to_if(p);
	}

	crawler& next();

	const crawler& next() const
	{
		// NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
		return const_cast<crawler*>(this)->next();
	}

	crawler in();

	const crawler in() const
	{
		// NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
		return const_cast<crawler*>(this)->in();
	}
};

using const_crawler = const crawler;

} // namespace tml
