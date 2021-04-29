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
	 * @brief The node was specified as C++ style raw string.
	 */
	raw_cpp,

	/**
	 * @brief The node was specified as Python style raw string.
	 */
	raw_python,

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
