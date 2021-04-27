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

	enum_size
};

struct extra_info{
	size_t line;
	size_t line_offset;
	utki::flags<treeml::flag> flags;
};

}
