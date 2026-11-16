#pragma once

#include <string_view>

#include "../tml/tree.hpp"

namespace tml_bnf {

// Parse a tml document according to the tml BNF (see doc/bnf.md) and return
// the resulting forest. This parser is an independent, spec-driven re-implementation
// of the tml syntax; it is used to cross-check the reference parser in libtml.
// Throws std::invalid_argument on a lexical or structural error.
tml::forest parse(std::string_view input);

} // namespace tml_bnf
