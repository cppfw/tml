#include <iostream>
#include <tml/tree.hpp>

int main(int argc, const char** argv){
    auto t = tml::read("hello{world}");

    std::cout << "tml = " << tml::to_string(t) << std::endl;

    return 0;
}
