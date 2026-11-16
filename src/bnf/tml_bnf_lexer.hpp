#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace tml_bnf {

// The token kinds produced by the lexer. The structural part (how VALUE /
// LBRACE / RBRACE combine into a document) is enforced by the bison grammar.
enum class token_kind {
	value,
	lbrace,
	rbrace,
};

struct lex_token {
	token_kind kind;
	std::string str; // payload for value tokens
};

// Thrown internally by the lexer when a NUL byte (\0) is encountered. A NUL
// byte terminates the document; an incomplete string is discarded. The
// process() function catches this and stops.
class nul_byte : public std::exception
{
public:
	const char* what() const noexcept override
	{
		return "nul byte terminates the document";
	}
};

// A hand-written lexer that tokenizes tml input according to the tml BNF
// (see doc/bnf.md). It resolves unquoted / quoted / raw string values (with
// escape processing), skips whitespace and "#" line comments, and emits VALUE,
// LBRACE and RBRACE tokens.
//
// The lexer is strict with respect to the BNF: it rejects malformed unicode
// escapes, unterminated quoted/raw strings and raw strings whose body
// contains a run of more than the delimiter length consecutive quotes. An
// unlisted escape (a backslash followed by a character that is not one of the
// listed escape characters) is kept literally: the backslash is preserved and
// the following character is processed as usual.
class lexer
{
public:
	// Process the whole input and produce the token stream.
	// Throws std::invalid_argument on a lexical error.
	void process(std::string_view input);

	const std::vector<lex_token>& tokens() const noexcept
	{
		return this->tokens_;
	}

	// True if a NUL byte (\0) was encountered in the input, which terminates
	// the document (parsing stops and any remaining input is ignored).
	bool nul_terminated() const noexcept
	{
		return this->nul_terminated_;
	}

private:
	std::string_view input_;
	std::size_t pos_ = 0;
	std::size_t line_ = 1;
	std::vector<lex_token> tokens_;
	bool nul_terminated_ = false;

	char peek() const noexcept;
	char consume() noexcept;
	bool at_end() const noexcept;
	bool is_hex(char c) const noexcept;
	bool is_unquoted_escape_char(char c) const noexcept;
	bool is_quoted_escape_char(char c) const noexcept;

	void skip_whitespace_and_comments();
	void emit_value(std::string value);
	void emit_lbrace();
	void emit_rbrace();

	std::string parse_unquoted();
	std::string parse_quoted();
	std::string parse_raw(std::size_t n);
	std::string parse_unquoted_escape();
	std::string parse_quoted_escape();
	std::string parse_unicode(std::size_t n);
};

} // namespace tml_bnf
