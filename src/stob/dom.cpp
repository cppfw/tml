#include "dom.hpp"

#include <ting/debug.hpp>



using namespace stob;



namespace{
ting::MemoryPool<sizeof(Node), 4096 / sizeof(Node)> memoryPool;
}



void* Node::operator new(size_t size){
	ASSERT(size == sizeof(Node))

	return memoryPool.Alloc_ts();
}



void Node::operator delete(void* p)throw(){
	memoryPool.Free_ts(p);
}



ting::Ptr<stob::Node> Load(ting::fs::File& fi){
	//TODO:
	
	return ting::Ptr<stob::Node>();
}
