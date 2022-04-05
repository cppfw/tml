#include <regex>

#include <tst/set.hpp>
#include <tst/check.hpp>

#include <papki/fs_file.hpp>

#include <treeml/tree.hpp>

namespace{
const std::string data_dir = "samples_data/";
}

namespace{
std::string print(const tml::forest& f){
	struct printer{
		std::stringstream ss;

		unsigned indentation = 0;

		void print(std::string_view sv){
			if(this->indentation != 0){
				for(unsigned i = 0; i != this->indentation; ++i){
					ss << "-";
				}
				ss << " ";
			}

			// escape new lines and back slashes
			for(auto c : sv){
				switch(c){
					case '\n':
						ss << "\\n";
						break;
					case '\\':
						ss << '\\';
						break;
					default:
						ss << c;
						break;
				}
			}

			ss << std::endl;
		}

		void print(const tml::forest& f){
			for(const auto& t : f){
				this->print(t.value.to_string());

				++this->indentation;
				this->print(t.children);
				--this->indentation;
			}
		}
	} p;

	p.print(f);

	return p.ss.str();
}
}

namespace{
tst::set set("samples", [](tst::suite& suite){
	std::vector<std::string> files;

    {
		const std::regex suffix_regex("^.*\\.tml$");
		auto all_files = papki::fs_file(data_dir).list_dir();

		std::copy_if(
				all_files.begin(),
				all_files.end(),
				std::back_inserter(files),
				[&suffix_regex](auto& f){
					return std::regex_match(f, suffix_regex);
				}
			);
	}

	suite.add<std::string>(
		"sample",
		std::move(files),
		[](const auto& p){
			papki::fs_file in_file(data_dir + p);

			papki::fs_file cmp_file(data_dir + papki::not_suffix(in_file.not_dir()) + ".cmp");

			// std::cout << in_file.path() << std::endl;

			auto printed = print(tml::read(in_file));

			if(!cmp_file.exists()){
				papki::file::guard file_guard(cmp_file, papki::file::mode::create);

				auto num_bytes_written = cmp_file.write(printed);

				if(num_bytes_written != printed.size()){
					tst::check(false, SL)
						<< "file '" << cmp_file.path() << "' does not exist"
						<< ", could not create one from parsed sample file";
				}

				tst::check(false, SL)
						<< "file '" << cmp_file.path() << "' does not exist"
						<< ", creating one from parsed sample file";
			}

			auto cmp = utki::make_string(cmp_file.load());

			if(cmp != printed){
				std::cout << "printed = " << std::endl;
				std::cout << printed << std::endl;
				tst::check(false, SL) << "parsing file '" << in_file.path() << "' is not as expected";
			}
		}
	);
});
}
