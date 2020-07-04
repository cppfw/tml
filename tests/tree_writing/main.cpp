#include "../../src/puu/tree.hpp"

#include <utki/config.hpp>
#include <utki/debug.hpp>

#include <papki/fs_file.hpp>
#include <papki/vector_file.hpp>

#include <clargs/parser.hpp>

#if M_OS == M_OS_WINDOWS
#	include <io.h>
#	include <fcntl.h>
#endif


int main(int argc, char** argv){

	// Set stdout to binary mode to prevent convertion of \n to \r\n by std::cout.
#if M_OS == M_OS_WINDOWS
	setmode(fileno(stdout), O_BINARY);
#endif

	clargs::parser args;

	puu::formatting formatted = puu::formatting::minimal;

	args.add('f', "--formatted", "format output to be human friendly", [&formatted](){formatted = puu::formatting::normal;});

	auto in_files = args.parse(argc, argv);

//	TRACE(<< "num infiles = " << in_files.size() << std::endl)

	for(auto& f : in_files){
		papki::fs_file fi(f);

		auto roots = puu::read(fi);

//		TRACE(<< "num roots read = " << roots.size() << std::endl)

		papki::vector_file outfi;

		puu::write(roots, outfi, formatted);

		auto data = outfi.reset_data();

		for(auto b : data){
			ASSERT_INFO_ALWAYS(b != '\r', "output contains '\r' character")
			std::cout << char(b);
		}
	}

	return 0;
}
