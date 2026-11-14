#include "tml_bnf.hpp"

#include "tml_bnf_grammar.hpp"

namespace tml_bnf {

tml::forest parse (std::string_view input)
{
	tml_bnf_grammar grammar;
	return grammar.parse (input);
}

}  // namespace tml_bnf
