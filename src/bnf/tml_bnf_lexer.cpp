#include "tml_bnf_lexer.hpp"

#include <stdexcept>

#include <utki/string.hpp>
#include <utki/unicode.hpp>

namespace tml_bnf {

namespace {

std::string line_prefix(std::size_t line)
{
	return "tml bnf: " + std::string(line < 10 ? " " : "") + std::to_string(line) + ": ";
}

} // namespace

char lexer::peek() const noexcept
{
	return this->pos_ < this->input_.size() ? this->input_[this->pos_] : '\0';
}

char lexer::consume() noexcept
{
	char c = this->peek();
	if (this->pos_ < this->input_.size()) {
		if (c == '\n') {
			++this->line_;
		}
		++this->pos_;
	}
	return c;
}

bool lexer::at_end() const noexcept
{
	return this->pos_ >= this->input_.size();
}

bool lexer::is_hex(char c) const noexcept
{
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

bool lexer::is_unquoted_escape_char(char c) const noexcept
{
	return c == ' ' || c == '\\' || c == '"' || c == '{' || c == '}' || c == 'n' || c == 't' || c == 'r' || c == 'u' ||
		c == 'U';
}

bool lexer::is_quoted_escape_char(char c) const noexcept
{
	return c == 'n' || c == '"' || c == '\\' || c == 't' || c == 'r' || c == 'u' || c == 'U';
}

void lexer::process(std::string_view input)
{
	this->input_ = input;
	this->pos_ = 0;
	this->line_ = 1;
	this->tokens_.clear();
	this->nul_terminated_ = false;

	for (;;) {
		this->skip_whitespace_and_comments();

		if (this->at_end()) {
			break;
		}
		char c = this->peek();
		if (c == '\0') {
			// A NUL byte in the input terminates the document: stop parsing and
			// ignore any remaining input.
			this->nul_terminated_ = true;
			break;
		}
		if (c == '{') {
			this->consume();
			this->emit_lbrace();
			continue;
		}
		if (c == '}') {
			this->consume();
			this->emit_rbrace();
			continue;
		}
		if (c == '"') {
			std::size_t quotes = 0;
			while (this->peek() == '"') {
				this->consume();
				++quotes;
			}
			try {
				if (quotes == 1) {
					this->emit_value(this->parse_quoted());
				} else if (quotes == 2) {
					this->emit_value(std::string{}); // empty quoted string
				} else {
					// a raw string is opened by a run of N >= 3 quotes followed by '('
					if (this->peek() != '(') {
						throw std::invalid_argument(line_prefix(this->line_) + "raw string must be opened by '('");
					}
					this->consume();
					this->emit_value(this->parse_raw(quotes));
				}
			} catch (nul_byte&) {
				// A NUL byte was encountered inside the string: stop.
				break;
			}
			continue;
		}
		// anything else begins an unquoted string
		this->emit_value(this->parse_unquoted());
		if (this->nul_terminated_) {
			// A NUL byte was encountered inside the unquoted string: the value
			// has been emitted; stop parsing.
			break;
		}
	}
}

void lexer::skip_whitespace_and_comments()
{
	for (;;) {
		char c = this->peek();
		if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
			this->consume();
			continue;
		}
		if (c == '#') {
			// A comment runs from "#" to the next newline (or end of input). A NUL
			// byte inside the comment is consumed as part of the comment (it does
			// not terminate the comment).
			while (!this->at_end() && this->peek() != '\n') {
				this->consume();
			}
			// the terminating '\n' (if any) is skipped by the next iteration
			continue;
		}
		break;
	}
}

void lexer::emit_value(std::string value)
{
	this->tokens_.push_back(lex_token{token_kind::value, std::move(value)});
}

void lexer::emit_lbrace()
{
	this->tokens_.push_back(lex_token{token_kind::lbrace, {}});
}

void lexer::emit_rbrace()
{
	this->tokens_.push_back(lex_token{token_kind::rbrace, {}});
}

std::string lexer::parse_unquoted()
{
	std::string value;
	for (;;) {
		if (this->at_end()) {
			break; // end of input: the string is complete
		}
		char c = this->peek();
		if (c == '\0') {
			// A NUL byte terminates the document. The current (unquoted) value is
			// complete up to the NUL byte, so it is returned (and emitted by the
			// caller); the caller then stops parsing.
			this->nul_terminated_ = true;
			break;
		}
		if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '{' || c == '}' || c == '"' || c == '#') {
			break;
		}
		if (c == '\\') {
			this->consume();
			char next = this->peek();
			if (is_unquoted_escape_char(next)) {
				value += this->parse_unquoted_escape();
			} else {
				// Unlisted escape: the backslash is kept literally and the
				// following character is processed as usual (it is not consumed
				// here).
				value += '\\';
			}
		} else {
			this->consume();
			value += c;
		}
	}
	return value;
}

std::string lexer::parse_quoted()
{
	std::string value;
	for (;;) {
		char c = this->peek();
		if (this->at_end()) {
			throw std::invalid_argument(line_prefix(this->line_) + "unterminated quoted string");
		}
		if (c == '\0') {
			// A NUL byte terminates the document.
			throw nul_byte{};
		}
		if (c == '"') {
			this->consume(); // closing quote
			break;
		}
		if (c == '\\') {
			this->consume();
			char next = this->peek();
			if (is_quoted_escape_char(next)) {
				value += this->parse_quoted_escape();
			} else {
				// Unlisted escape: the backslash is kept literally and the
				// following character is processed as usual (it is not consumed
				// here).
				value += '\\';
			}
		} else {
			this->consume();
			value += c; // byte-exact
		}
	}
	return value;
}

std::string lexer::parse_raw(std::size_t n)
{
	std::string value;
	for (;;) {
		char c = this->peek();
		if (this->at_end()) {
			throw std::invalid_argument(line_prefix(this->line_) + "unterminated raw string");
		}
		if (c == '\0') {
			// A NUL byte terminates the document.
			throw nul_byte{};
		}
		if (c == ')') {
			this->consume();
			std::size_t run = 0;
			while (this->peek() == '"') {
				this->consume();
				++run;
			}
			if (run > n) {
				throw std::invalid_argument(
					line_prefix(this->line_) + "a run of more than " + std::to_string(n) +
					" consecutive quotes after ')' in a raw string"
				);
			}
			if (run == n) {
				break; // the closing delimiter: ')' followed by exactly n quotes
			}
			// run of < n quotes: the ')' and the quotes are literal content
			value += ')';
			value.append(run, '"');
		} else if (c == '"') {
			// quotes not preceded by ')' are literal content, in runs of any length
			std::size_t run = 0;
			while (this->peek() == '"') {
				this->consume();
				++run;
			}
			value.append(run, '"');
		} else {
			this->consume();
			value += c;
		}
	}
	return value;
}

std::string lexer::parse_unquoted_escape()
{
	char c = this->consume();
	switch (c) {
		case ' ':
			return " ";
		case '\\':
			return "\\";
		case '"':
			return "\"";
		case '{':
			return "{";
		case '}':
			return "}";
		case 'n':
			return "\n";
		case 't':
			return "\t";
		case 'r':
			return "\r";
		case 'u':
			return this->parse_unicode(4);
		case 'U':
			return this->parse_unicode(8);
		default:
			throw std::invalid_argument(line_prefix(this->line_) + "unlisted escape sequence '\\'" + std::string(1, c));
	}
}

std::string lexer::parse_quoted_escape()
{
	char c = this->consume();
	switch (c) {
		case 'n':
			return "\n";
		case '"':
			return "\"";
		case '\\':
			return "\\";
		case 't':
			return "\t";
		case 'r':
			return "\r";
		case 'u':
			return this->parse_unicode(4);
		case 'U':
			return this->parse_unicode(8);
		default:
			throw std::invalid_argument(
				line_prefix(this->line_) + "unlisted escape sequence in quoted string '\\'" + std::string(1, c)
			);
	}
}

std::string lexer::parse_unicode(std::size_t n)
{
	std::string hex;
	for (std::size_t i = 0; i < n; i++) {
		char c = this->peek();
		if (!this->is_hex(c)) {
			throw std::invalid_argument(line_prefix(this->line_) + "malformed unicode escape");
		}
		this->consume();
		hex += c;
	}

	uint32_t cp = 0;
	for (char h : hex) {
		cp <<= 4;
		if (h >= '0' && h <= '9') {
			cp |= (uint32_t)(h - '0');
		} else if (h >= 'a' && h <= 'f') {
			cp |= (uint32_t)(h - 'a' + 10);
		} else if (h >= 'A' && h <= 'F') {
			cp |= (uint32_t)(h - 'A' + 10);
		}
	}

	auto bytes = utki::to_utf8(char32_t(cp));
	std::string result;
	for (char b : bytes) {
		if (b == '\0') {
			break;
		}
		result += b;
	}
	return result;
}

} // namespace tml_bnf
