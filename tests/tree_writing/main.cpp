#include "../../src/puu/tree.hpp"

#include <utki/debug.hpp>

#include <papki/FSFile.hpp>
#include <papki/MemoryFile.hpp>

#include <clargs/arguments.hpp>


int main(int argc, char** argv){
	clargs::arguments args;

	bool formatted = false;

	args.add('f', "--formatted", "format output to be human friendly", [&formatted](){formatted = true;});

	auto in_files = args.parse(argc, argv);

	for(auto& f : in_files){
		papki::FSFile fi(f);

		auto roots = puu::read(fi);

		papki::MemoryFile outfi;

		puu::write(roots, outfi, formatted);

		auto data = outfi.resetData();

		std::cout << reinterpret_cast<char*>(&*data.begin());
	}

	return 0;
}
