# tml syntax specification

tml (text markup language) describes a tree of text strings. The syntax is
deliberately minimal: there is only one kind of element — a string — and the
only structure is parent/child nesting via curly braces.

This document is the prose specification. The formal BNF specification of the
language is in [bnf.md](bnf.md).

## The basic idea

A **document** is a (possibly empty) sequence of **nodes**. A **node** is a
string, optionally followed by a children block in curly braces:

```
my_string
another_string {
    child_1
    child_2
}
```

The document above has two top-level nodes: `my_string` (no children) and
`another_string` (two children). If a string has no children, the curly braces
are omitted.

## Strings

A string can be **unquoted**, **quoted**, or **raw**. All three produce the
same kind of value (a sequence of bytes); they differ only in what characters
you can write and how the parser interprets them.

### Unquoted strings

An unquoted string is the simplest form. It runs until the parser hits a
character that cannot be part of an unquoted value:

- whitespace (space, tab, newline, carriage return)
- `{` or `}`
- `"`
- `#` (opens a comment)
- a NUL byte (terminates the document)

Everything else is a literal character in the value. A backslash introduces an
escape sequence (see below).

```
hello world        # two strings: "hello" and "world"
a{b}              # string "a" with one child "b"
\{hello\}         # one string: {hello}
```

### Quoted strings

A quoted string is enclosed in double quotes. Inside the quotes, whitespace,
`{`, `}`, `#`, newlines, and most other characters are **literal** (byte-exact
content). The only special characters are the closing `"` and the backslash
(which introduces an escape).

```
"hello world"       # one string: hello world
"it's fine"         # one string: it's fine
"a # b"             # one string: a # b
"line1\nline2"      # one string: line1<newline>line2 (the \n escape produces a newline)
"line1
line2"             # one string: line1<newline>line2 (a literal newline is kept verbatim)
```

An empty quoted string is written as `""`.

### Raw strings

A raw string is opened by a run of **N ≥ 3** consecutive double quotes and
closed by a run of exactly N consecutive double quotes. Inside the raw string,
**nothing is processed** — no escapes, no stripping. The value is exactly the
bytes between the delimiters.

```
"""hello "world" """     # one string: hello "world" 
"""a{}b"""               # one string: a{}b
```

Rules for the content:

- A run of **fewer** than N quotes is literal content.
- A run of **exactly** N quotes closes the string.
- A run of **more** than N quotes is a syntax error.

A longer opening run lets you embed shorter quote runs in the content:

```
"""""it has "" quotes"""""
```

This is a raw string with N=5; the `""` in the middle is just two literal
quotes.

### Comparing the three kinds

| | Unquoted | Quoted | Raw |
|---|---|---|---|
| Whitespace in value | No (use `\ ` or escape) | Yes (literal) | Yes (literal) |
| `{` / `}` in value | No (use `\{` `\}`) | Yes (literal) | Yes (literal) |
| `#` in value | No (starts comment) | Yes (literal) | Yes (literal) |
| Newline in value | No | Yes (literal) | Yes (literal) |
| Escape sequences | Yes | Yes | No |
| Delimiter | none | `"` | `"""` or longer |

## Escape sequences

Escape sequences are introduced by a backslash (`\`). They are available in
unquoted and quoted strings (not in raw strings).

| Escape | Value | Unquoted | Quoted |
|---|---|---|---|
| `\ ` (backslash + space) | space | Yes | No (literal) |
| `\\` | backslash | Yes | Yes |
| `\"` | double quote | Yes | Yes |
| `\{` | left curly brace | Yes | No (literal) |
| `\}` | right curly brace | Yes | No (literal) |
| `\n` | newline (LF) | Yes | Yes |
| `\t` | tab | Yes | Yes |
| `\r` | carriage return | Yes | Yes |
| `\uXXXX` | Unicode code point (4 hex digits) | Yes | Yes |
| `\UXXXXXXXX` | Unicode code point (8 hex digits) | Yes | Yes |

The `\u` and `\U` escapes encode a Unicode code point as a hexadecimal number.
All digits must be present and valid hex; a trailing non-hex character is a
syntax error. The code point is encoded as UTF-8 in the value.

**Unlisted escapes:** a backslash followed by a character that is *not* in the
table above is kept **literally** — both the backslash and the following
character appear in the value. For example, `a\Xb` has the value `a\Xb`.
This applies to both unquoted and quoted strings.

Note that in an unquoted string, a bare newline normally terminates the value.
A backslash before a newline keeps the backslash literally, and the newline
still terminates the value. For example, `a\\<newline>b` produces two values:
`a\` and `b`.
## Comments

A `#` (number sign) opens a **line comment**. The comment runs from the `#` to
the end of the line (the newline is consumed as part of the comment). There are
no block comments.

```
# this is a comment
value1 # this is also a comment (value1 is the only token on this line)
"value # not a comment"   # the # inside quotes is literal
"""raw # also literal"""  # the # inside a raw string is literal
```

`#` is recognized **unconditionally** outside strings: it starts a comment
whether or not preceded by whitespace. This means `/` (and `//`) is an ordinary
character in unquoted strings, so URLs like `http://example.com/path` can be
written without quoting. However, a `#` in a URL fragment
(`http://example.com/page#section`) must be quoted, because `#` starts a
comment.

## NUL byte

A NUL byte (`\0`, byte value 0x00) acts as the **end-of-document** marker. When
the parser encounters a NUL byte, parsing stops immediately:

- The current value (if it is complete) is emitted.
- Any remaining input after the NUL byte is ignored.
- An incomplete value (e.g. an unterminated quoted or raw string) is discarded.

```
abc\0def    →  one value: "abc"  (the "def" is ignored)
\0abc       →  empty document
"ab\0c"     →  empty document (unterminated quoted string is discarded)
```

Real `.tml` files do not contain NUL bytes; this is a defined edge behavior.

## Extended information

In addition to the parsed value and child structure, the parser can record
**extended information** (also called *extra info*) for each node. This is
metadata about the node's appearance in the source text, used for round-tripping
(writing the document back with the original formatting preserved).

Each node has:

- **Location**: the 1-based line number and 0-based column offset where the
  node starts in the source.
- **Flags**:

| Flag | Meaning |
|---|---|
| `space` | There is whitespace between this node and the previous sibling. Always false for the first node in a document or the first child in a brace group. |
| `quoted` | The node was written as a quoted string (e.g. `"hello"`). Mutually exclusive with `raw`. |
| `raw` | The node was written as a raw string (e.g. `"""hello"""`). Mutually exclusive with `quoted`. |
| `first_on_line` | The node is at the beginning of the line (only whitespace before it on that line). |
| `curly_braces` | The node has a children list in the source (e.g. `hello{...}`), even if the list is empty. |

These flags are not part of the language syntax — they are implementation
metadata that the parser extracts while reading. The BNF conformance parser
does not track them; only the reference parser does. Applications may use
this metadata to control logic, for example a text editor can use `first_on_line`
and `space` to preserve indentation when reformatting a document.

## Full example

```
"quoted string"
unquoted_string
string_one string_two "string_three"
string_four"string five"
"string six"string_seven
"string with children"{
	"child 1"
	Child2
	"child three"{
		SubChild1
		"Subchild two"

		# properties can be expressed as a string with just one child
		Property1 {Value1}
		"Property two" {"Value 2"}

		# a comment

		# multi line
		   # comment

		# / is an ordinary character, so this URL needs no quoting:
		http://website.com/some/url/*

		# escape sequences inside a quoted string
		"Escape sequences: \" \n \t \\"

		# raw string: byte-exact, nothing is stripped
		"""raw string content " { } \ /"""
		"""bla bla {} "" and "quoted" bla bla"""

		# a raw string's value is exactly the bytes between the delimiters
"""
multiline
verbatim text
block
"""

		# a longer opening run allows shorter quote runs in the content
"""""it has "" quotes"""""
	}
}
```
