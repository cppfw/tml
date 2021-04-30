#pragma once

#include <utki/flags.hpp>

namespace treeml{

/**
 * @brief Additional information flags.
 * These are the flags indicating additional formatting information of the parsed treeml node.
 */
enum class flag{
	/**
	 * @brief Space between previous node and this node.
	 * Indicates that in the original text there was a space between this node and previous node.
	 * Absence of space is possible in case of very first document's node or previous node had
	 * curly braces or previous or this node is a quoted string.
	 * For the very first node of the document this flag is laways false, since there is no previous node.
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
	 * @brief The node was specified as Python style raw string.
	 * In case this flag is not set for the raw string, then it is specified
	 * as C++ style syntax.
	 */
	raw_python_style,

	/**
	 * @brief The node has children list.
	 * Indicates that the node had children list specified, possibly empty.
	 * In other words, this indicates that there is an opening curly brace following the string.
	 */
	children,

	enum_size
};

struct location{
	size_t line;
	size_t offset;
};

struct extra_info{
	treeml::location location;
	utki::flags<treeml::flag> flags;
};

}
