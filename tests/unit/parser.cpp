#include <tst/set.hpp>
#include <tst/check.hpp>

#include "../../src/treeml/parser.hpp"

namespace{
tst::set set("parser", [](auto& suite){
	suite.template add<const char*>(
			"flag_space_should_be_false",
			{
				"hello",
				"hello post",
				"pre\"hello\"",
				"\"pre\"hello",
				"{}hello",
				"pre{child1 child2}hello post",
				"R\"raw string\"hello",
				"pre{hello child2}"
			},
			[](const auto& p){
				treeml::parser parser;

				struct listener : public treeml::listener{
					bool string_parsed = false;
					void on_string_parsed(std::string_view str, utki::flags<treeml::flags> flags)override{
						if(str == "hello"){
							string_parsed = true;
							tst::check(!flags.get(treeml::flags::space), SL);
						}
					}

					void on_children_parse_started()override{}
					void on_children_parse_finished()override{}
				} l;

				parser.parse_data_chunk(p, l);
				parser.end_of_data(l);

				tst::check(l.string_parsed, SL);
			}
		);
});
}
