#include "../../src/stob/dom.hpp"

#include <ting/debug.hpp>
#include <ting/fs/FSFile.hpp>



namespace TestBasicParsing{

void Run(){
	ting::fs::FSFile fi("test.stob");
	
	ting::Ptr<stob::Node> root = stob::Load(fi);
	
	
}

}




int main(int argc, char** argv){

	TestBasicParsing::Run();
	
	
	return 0;
}
