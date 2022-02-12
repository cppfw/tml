#include <tst/set.hpp>
#include <tst/check.hpp>

#include <regex>

#include <papki/fs_file.hpp>
#include <papki/vector_file.hpp>

#include "../../../src/treeml/tree.hpp"

namespace{
const std::string data_dir = "tree_writing_data/";
}

namespace{
template <bool formatted>
std::function<void(const std::string&)> make_test_proc(){
	return [](const std::string& p){
		papki::fs_file fi(data_dir + p);

		auto roots = tml::read(fi);

		papki::vector_file outfi;

		tml::write(roots, outfi, formatted ? tml::formatting::normal : tml::formatting::minimal);

		auto data = outfi.reset_data();

		papki::fs_file expected_fi(fi.path() + "." + (formatted ? "" : "un") + "formatted");

		auto expected_data = expected_fi.load();

		tst::check(data == expected_data, SL);
	};
}
}

namespace{
tst::set set("tree_writing", [](tst::suite& suite){
	std::vector<std::string> files;

	{
		const std::regex tml_regex("^.*\\.tml$");
		auto all_files = papki::fs_file(data_dir).list_dir();

		std::copy_if(
				all_files.begin(),
				all_files.end(),
				std::back_inserter(files),
				[&tml_regex](auto& f){
					return std::regex_match(f, tml_regex);
				}
			);
	}

	suite.add<std::string>(
		"write_unformatted",
		decltype(files)(files),
		make_test_proc<false>()
	);

	suite.add<std::string>(
		"write_formatted",
		decltype(files)(files),
		make_test_proc<true>()
	);
});
}
