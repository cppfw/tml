# tml

[![releases](https://img.shields.io/github/tag/cppfw/tml.svg)](https://github.com/cppfw/tml/releases) [![ci status](https://github.com/cppfw/tml/workflows/ci/badge.svg)](https://github.com/cppfw/tml/actions)

**T**ree **M**arkup **L**anguage.
A lightweight markup language C++ library.

The idea is that the document is just a tree of strings. Each string can have a number of children strings or no children at all, and so on.

document example:

```tml
"quoted string"
unquoted_string
"string with children"{
	"child 1"
	Child2
	"child three"{
		SubChild1
		"Subchild two"

		# properties can be expressed as a string with just one child
		Property1 {Value1}
		"Property two" {"Value 2"}

		# a # opens a line comment, it runs to the end of the line

		# / is an ordinary character, so URLs can be written without quotes:
		http://website.com/some/url/*

		# escape sequences inside a quoted string
		"Escape sequences: \" \n \t \\"

		# a raw string is delimited by 3 or more double quotes,
		# its content is byte-exact and no escape is processed,
		# a run of fewer than 3 quotes in the content is literal
		"""raw string content " "" { } \ /"""

		# a longer opening run allows shorter quote runs in the content,
		# up to N-1 consecutive quotes for an N-quote delimiter
		"""""it has """" quotes"""""
	}
}
```

The full language specification is in [doc/spec.md](doc/spec.md), the formal
BNF grammar in [doc/bnf.md](doc/bnf.md).

## Usage

```cpp
#include <tml/tree.hpp>

auto doc = tml::read(
R"(
root{
	name{value}
	"quoted child"
}
)"
);

for (const auto& node : doc)
{
	const auto& value = node.value.string;  // the node's string
	for (const auto& child : node.children)
		// ... visit children ...
	{};
};

std::string text = tml::to_string(doc);  // serialize back to tml
```

## Installation and Documentation

See [Wiki](wiki/main.adoc).
