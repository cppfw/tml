# tml BNF

This file is a human-readable formal description of the tml language grammar
(EBNF-like notation). It is **not** used by bison as input; the actual bison
grammar is in [`tml.y`](../src/bnf/tml.y). This file is the *source of truth*
for the language: an implementation conforms to tml if and only if it accepts
exactly the inputs this grammar accepts and rejects exactly the inputs this
grammar rejects. It is derived from the prose spec in
[spec.md](spec.md).

**Notation (EBNF-like):**

| Notation | Meaning |
|---|---|
| `X*` | zero or more X |
| `X+` | one or more X |
| `[X]` | optional X |
| `a / b` | either a or b |
| `{n}` | exactly n repetitions |
| `{n,}` | n or more repetitions |
| `{n,m}` | between n and m repetitions |
| `"c"` | the literal character c |
| `'c'` | the literal escape sequence c (backslash + c) |

---

## Document structure

A document is a (possibly empty) sequence of nodes.

```
document   = node* .
```

A node is a string, optionally followed by a children block.

```
node       = string [ "{" document "}" ] .
```

A string is unquoted, quoted, or raw.

```
string     = unquoted-string
           / quoted-string
           / raw-string
           .
```

**Whitespace** (space, tab, carriage return, line feed) separates tokens in the
unquoted context and carries no meaning of its own. It is never part of an
unquoted string value. Inside quoted and raw strings whitespace is literal
(byte-exact) content.

A `#` (number sign) opens a line comment in the unquoted context: the comment
runs from the `#` to (and including) the next line feed, or to end of data if
none. A `#` inside a quoted or raw string is literal content, not a comment.
Comments may appear anywhere a token may appear.

```
comment    = "#" non-lf* .
```

---

## Unquoted string

An unquoted string is a non-empty sequence of atoms. It may not contain an
unescaped space, tab, line feed, carriage return, `{`, `}`, `"`, or `#`, and a
backslash always begins an escape sequence (it is never a literal atom).

```
unquoted-string   = unquoted-atom+ .
unquoted-atom     = unquoted-char / unquoted-escape .
```

Any character except: space, tab, LF, CR, `{`, `"`, `}`, `#`.

```
unquoted-char     = any-char-but { " " #tab #lf #cr "{" "\"" "}" "#" } .
```

```
unquoted-escape   = ' '          ; an escaped space (backslash + space)
                 / "\\"         ; backslash
                 / "\""        ; double quote
                 / "\{"        ; left curly brace
                 / "\}"        ; right curly brace
                 / '\n'        ; line feed
                 / '\t'        ; tab
                 / '\r'        ; carriage return
                 / "\u" HEXDIGIT{4}
                 / "\U" HEXDIGIT{8}
                 / "\\" unlisted-escape-char   ; unlisted escape (kept literally)
                 .
```

An **unlisted escape** is a backslash followed by a character that is not one
of the listed escape characters. Unlisted escapes are kept literally: both the
backslash and the following character appear in the value. `unlisted-escape-char`
is any character except: space, backslash, double quote, `{`, `}`, `n`, `t`,
`r`, `u`, `U`.

```
unlisted-escape-char = any-char-but { " " "\\" "\"" "{" "}" "n" "t" "r" "u" "U" } .
```

---

## Quoted string

A quoted string is enclosed in double quotes. Its value is byte-exact: it is
exactly the bytes between the quotes, with the listed escape sequences
processed. A literal LF / tab / CR typed between the quotes is kept verbatim.

```
quoted-string    = "\"" quoted-content "\"" .
quoted-content   = quoted-atom* .
quoted-atom      = quoted-char / quoted-escape .
```

Any character except the closing `"` and the escape introducer `\`. (In
particular LF, tab, CR and `#` are all allowed here, byte-exact.)

```
quoted-char      = any-char-but { "\"" "\" } .
```

```
quoted-escape    = '\n'        ; line feed
                 / "\""        ; double quote
                 / "\\"        ; backslash
                 / '\t'        ; tab
                 / '\r'        ; carriage return
                 / "\u" HEXDIGIT{4}
                 / "\U" HEXDIGIT{8}
                 / "\\" quoted-unlisted-char   ; unlisted escape (kept literally)
                 .
```

A **reserved escape** inside a quoted string is a backslash followed by a
character that is not one of the listed escape characters. Reserved escapes
are ignored: both the backslash and the following character are discarded from
the value. `quoted-reserved-char` is any character except `n`, `"`, `\`, `t`,
`r`, `u`, `U`.

```
quoted-unlisted-char = any-char-but { "n" "\"" "\\" "t" "r" "u" "U" } .
```

An empty quoted string is written as two consecutive double quotes (`""`),
which is a quoted string whose content is empty.

---

## Raw string

A raw string is opened by a run of N consecutive double quotes (N >= 3)
immediately followed by an opening parenthesis `(`, and closed by a closing
parenthesis `)` immediately followed by a run of exactly N consecutive double
quotes. N is fixed by the opening run. The parentheses are delimiters, not
content. Its value is byte-exact: exactly the bytes between the opening `(`
and the closing `)`, with nothing stripped and no escape processing.

Inside the raw string body:

- a double quote is literal content; quote runs of any length are allowed as
  long as they do not immediately follow a `)`;
- a `)` immediately followed by a run of fewer than N double quotes is literal
  content (the parenthesis and the quotes are all part of the value);
- a `)` immediately followed by a run of exactly N double quotes closes the
  string;
- a `)` immediately followed by a run of more than N double quotes is a syntax
  error.

The closing delimiter is lexical (it depends on the maximal run of consecutive
quotes following a `)`, a lookahead concern), so the raw string is defined as
a lexical rule rather than a pure BNF production.

```
raw-string(N)    = "\"" {N} "(" raw-body(N) .            N >= 3
raw-body(N)      = ( raw-char / ")" "\"" {0,N-1} )* ")" "\"" {N} .
raw-char         = any character other than ")" .
```

A `)` followed by a run of more than N consecutive quotes is a syntax error.
A quote run of 3 or more at the start of a token that is not immediately
followed by `(` is a syntax error.

---

## Terminals

```
HEXDIGIT = "0".."9" / "a".."f" / "A".."F" .
```

**Unicode escapes:** `"\uXXXX"` and `"\UXXXXXXXX"` encode a Unicode code point
as a 4- or 8-digit hexadecimal number. All digits must be present and hex; a
trailing non-hex character is a syntax error. The code point is encoded into
the string value as UTF-8.

> **NOTE (spec gap):** the prose spec does not state how surrogates
> (U+D800..U+DFFF) or values above U+10FFFF are handled. This BNF leaves that
> to the implementation's UTF-8 encoder.

---

## NUL byte

A NUL byte (`\0`) is the end-of-document marker: parsing stops at the NUL
byte, the current value (if any and complete) is emitted, and any remaining
input is ignored. An incomplete value (e.g. an unterminated quoted or raw
string) is discarded.

- In an unquoted context, a NUL byte terminates the current (unquoted) value
  and the value is emitted. Example: `abc\0def` parses to the value `abc` (the
  `def` is ignored).
- Inside a quoted or raw string, a NUL byte terminates the document and the
  incomplete string is discarded. Example: `"ab\0c"` parses to an empty forest
  (the unterminated quoted string is discarded).

Real `.tml` files do not contain NUL bytes; this is a defined edge behavior.

---

## Top-level token stream

The token stream is a sequence of tokens. A token is one of:

- a string (unquoted, quoted, or raw) → a **VALUE** token
- `{` → **LBRACE**
- `}` → **RBRACE**

Whitespace and comments are skipped between tokens.

The token stream of a document must reduce to `document`. In particular a
LBRACE token may only appear immediately after a VALUE token, and every RBRACE
must match a preceding LBRACE (and vice versa); a LBRACE with no preceding
VALUE, or a RBRACE with no matching LBRACE, is a syntax error.
