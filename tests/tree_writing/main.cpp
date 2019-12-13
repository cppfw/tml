#include "../../src/puu/tree.hpp"

#include <utki/config.hpp>
#include <utki/debug.hpp>

#include <papki/FSFile.hpp>
#include <papki/MemoryFile.hpp>

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

	bool formatted = false;

	args.add('f', "--formatted", "format output to be human friendly", [&formatted](){formatted = true;});

	auto in_files = args.parse(argc, argv);

//	TRACE(<< "num infiles = " << in_files.size() << std::endl)

	for(auto& f : in_files){
		papki::FSFile fi(f);

		auto roots = puu::read(fi);

//		TRACE(<< "num roots read = " << roots.size() << std::endl)

		papki::MemoryFile outfi;

		puu::write(roots, outfi, formatted);

		auto data = outfi.resetData();

		for(auto b : data){
			ASSERT_INFO_ALWAYS(b != '\r', "output contains '\r' character")
			std::cout << char(b);
		}
	}

	return 0;
}
