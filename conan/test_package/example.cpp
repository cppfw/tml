#include <tml/tree.hpp>

#include <iostream>

using namespace std::string_literals;

int main(int argc, const char** argv){
	tml::tree t("hello"s, {tml::tree("tml!"s)});

	std::cout << "tml = " << tml::to_string(t) << std::endl;

	return 0;
}
