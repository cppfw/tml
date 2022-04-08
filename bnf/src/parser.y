%class-name parser
%namespace bnf

// ====================================================
// = this block defines filenames for generated files =
%baseclass-header parserBase.hpp
%class-header parser.hpp
%implementation-header parser.cxx
%parsefun-source parser.cpp
// ====================================================

%%

whitespace :
	' ' | '\t' | '\n'
	{
		std::cout << "whitespace ";
	}
;
