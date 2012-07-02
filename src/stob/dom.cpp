#include "dom.hpp"

#include <ting/debug.hpp>

#include "parser.hpp"



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



ting::Ptr<stob::Node> stob::Load(ting::fs::File& fi){
	class Listener : public stob::ParseListener{
		//override
		void OnChildrenParseFinished(){
			
		}

		//override
		void OnChildrenParseStarted(){
			
		}

		//override
		void OnStringParsed(const char* s, ting::u32 size){
			ting::Ptr<Node> node = Node::New();
			node->SetValue(std::string(s, size));
		}
		
		Node* curParent;
		
	public:
		ting::Ptr<Node> root;
		
		Listener() :
				root(Node::New())
		{
			this->curParent = this->root.operator->();
		}
	};
	
	return ting::Ptr<stob::Node>();
}
