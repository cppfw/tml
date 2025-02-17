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

#include <utki/flags.hpp>

namespace tml {

/**
 * @brief Additional information flags.
 * These are the flags indicating additional formatting information of the parsed tml node.
 */
enum class flag {
	/**
	 * @brief Space between previous node and this node.
	 * Indicates that in the original text there is a whitespace between this node and previous node.
	 * Absence of whitespace is possible in case of very first document's node or previous node had
	 * curly braces or previous or this node is a quoted string.
	 * For the very first node of the document this flag is always false, since there is no previous node.
	 * Same for the very first node in the children list (in curly braces).
	 */
	space,

	/**
	 * @brief The node was specified as quoted string.
	 */
	quoted,

	/**
	 * @brief The node was specified as raw string.
	 */
	raw,

	/**
	 * @brief The node was specified as quotes raw string.
	 * The quotes style raw string is the one which has 3 or more double quotes as its opening
	 * and closing sequence.
	 * In case this flag is not set for the raw string, then it is specified
	 * as C++ style syntax.
	 */
	raw_quotes_style,

	/**
	 * @brief The node is first on the line.
	 * Indicates that in the original text this node is first on the line, i.e.
	 * before the node on the line are only whitespace characters or nothing.
	 */
	first_on_line,

	/**
	 * @brief The node has children list.
	 * Indicates that in the original text the node is specified with children list, either empty or not.
	 */
	curly_braces,

	enum_size
};

struct location {
	size_t line = 0;
	size_t offset = 0;
};

struct extra_info {
	tml::location location;
	utki::flags<tml::flag> flags;
};

} // namespace tml
