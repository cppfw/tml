// ============================================================================
// tml BNF parser grammar (bison).
//
// This implements the *structural* part of the tml BNF (see doc/bnf.md).
// The terminal (lexical) part -- unquoted / quoted / raw strings, comments and
// whitespace -- is implemented by the hand-written lexer (tml_bnf_lexer).
//
// The grammar builds a tml::forest (a forest of tml::tree<tml::leaf>), exactly
// the same document type that the reference parser in libtml produces.
//
// Tokens:
//   VALUE    a string value (unquoted, quoted or raw); semantic type std::string
//   LBRACE   "{"
//   RBRACE   "}"
//
// Grammar (from tml.bnf):
//   document  -> node*
//   node      -> VALUE [ LBRACE document RBRACE ]
// ============================================================================

%language "c++"
%define api.value.type variant
%define api.parser.class { tml_bnf_parser }
%define parse.error verbose

%code requires {
	#include <string>
	#include "../../../tml/tree.hpp"
	namespace tml_bnf {
		class tml_bnf_grammar;
	}
	namespace yy {
		// The free yylex() below is found by the generated parse() via name
		// lookup (it is not a member of the generated parser class).
		// current_grammar points at the active driver instance (defined in
		// tml_bnf_grammar.cpp).
		extern thread_local tml_bnf::tml_bnf_grammar* current_grammar;
		int yylex (void* yylval);
	}
}

%code {
	#include "tml_bnf_grammar.hpp"
}

%token <std::string> VALUE
%token LBRACE
%token RBRACE

%type <tml::forest> document
%type <tml::forest> forest
%type <tml::tree> node

%start document

%%

document
	: forest
		{
			if (yy::current_grammar) {
				yy::current_grammar->result = std::move($1);
			}
			$$ = std::move($1);
		}
	;

forest
	: /* empty */
		{
			$$ = tml::forest{};
		}
	| forest node
		{
			$1.push_back(std::move($2));
			$$ = std::move($1);
		}
	;

node
	: VALUE
		{
			$$ = tml::tree(tml::leaf(std::move($1)));
		}
	| VALUE LBRACE forest RBRACE
		{
			tml::tree t (tml::leaf(std::move($1)));
			t.children = std::move($3);
			$$ = std::move(t);
		}
	;

%%
