#include "tml_bnf_grammar.hpp"

#include <stdexcept>

namespace yy {

thread_local tml_bnf::tml_bnf_grammar* current_grammar = nullptr;

int yylex(void* yylval)
{
	auto* g = current_grammar;
	if (!g) {
		return tml_bnf_parser::token::YYEOF;
	}
	const auto& tokens = g->tokens();
	auto& idx = g->token_index();
	if (idx >= tokens.size()) {
		return tml_bnf_parser::token::YYEOF;
	}
	const tml_bnf::lex_token& t = tokens[idx++];
	auto* val = static_cast<tml_bnf_parser::value_type*>(yylval);
	switch (t.kind) {
		case tml_bnf::token_kind::value:
			val->emplace<std::string>(t.str);
			return tml_bnf_parser::token::VALUE;
		case tml_bnf::token_kind::lbrace:
			return tml_bnf_parser::token::LBRACE;
		case tml_bnf::token_kind::rbrace:
			return tml_bnf_parser::token::RBRACE;
		default:
			return tml_bnf_parser::token::YYEOF;
	}
}

} // namespace yy

// The bison-generated base class declares error(const std::string&) as a
// non-pure virtual but does not define it. We define it here (it throws), so
// that the base class's vtable has a valid target.
namespace yy {

void tml_bnf_parser::error(const std::string& message)
{
	throw std::invalid_argument(message);
}

} // namespace yy

namespace tml_bnf {

// Build a forest from a (possibly incomplete) token stream. This is used when
// a NUL byte terminates the document: the token stream may be incomplete (e.g.
// an unclosed LBRACE), so the bison parser cannot be used (it would reject the
// incomplete stream). Instead we build the forest directly, attaching any
// incomplete trailing structure as best we can.
tml::forest build_forest_from_tokens(const std::vector<lex_token>& tokens)
{
	tml::forest root;
	// A stack of forests. The bottom is the root; each LBRACE pushes a new
	// forest (the children of the last node); each RBRACE pops it and attaches
	// it to the last node's children.
	std::vector<tml::forest> stack;
	stack.push_back(std::move(root));

	for (const auto& token : tokens) {
		auto& cur = stack.back();
		if (token.kind == token_kind::value) {
			auto node = tml::tree{};
			node.value.string = token.str;
			cur.push_back(std::move(node));
		} else if (token.kind == token_kind::lbrace) {
			if (!cur.empty()) {
				stack.push_back(tml::forest{});
			}
		} else { // rbrace
			if (stack.size() > 1) {
				auto children = std::move(stack.back());
				stack.pop_back();
				if (!stack.empty() && !stack.back().empty()) {
					stack.back().back().children = std::move(children);
				}
			}
		}
	}

	return stack[0];
}

tml::forest tml_bnf_grammar::parse(std::string_view input)
{
	this->lexer_.process(input);
	if (this->lexer_.nul_terminated()) {
		// A NUL byte terminated the document: build the forest from the partial
		// token stream (without calling the bison parser).
		return build_forest_from_tokens(this->lexer_.tokens());
	}

	yy::current_grammar = this;
	try {
		this->token_index_ = 0;
		// the bison-generated parse() (no arguments); called via the base class
		// name because the derived parse(std::string_view) hides it
		yy::tml_bnf_parser::parse();
		return std::move(this->result);
	} catch (...) {
		yy::current_grammar = nullptr;
		throw;
	}
}

} // namespace tml_bnf
