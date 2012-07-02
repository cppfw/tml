#include "dom.hpp"

#include <vector>

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



Node* Node::Next(const std::string& value)throw(){
	for(Node* n = this->Next(); n; n = n->Next()){
		if(n->Value() == value){
			return n;
		}
	}
	return 0;
}



Node* Node::Child(const std::string& value)throw(){
	if(this->children.IsNotValid()){
		return 0;
	}
	
	if(this->children->Value() == value){
		return this->children.operator->();
	}
	
	return this->children->Next(value);
}



void Node::Write(ting::fs::File& fi){
	//TODO:
}



ting::Ptr<stob::Node> stob::Load(ting::fs::File& fi){
	class Listener : public stob::ParseListener{
		typedef std::pair<ting::Ptr<Node>, Node*> T_Pair;
		std::vector<T_Pair> stack;
		
	public:
		//override
		void OnChildrenParseFinished(){
			this->stack.back().second->SetChildren(this->chain);
			this->chain = this->stack.back().first;
			this->lastInChain = this->stack.back().second;
			this->stack.pop_back();
		}

		//override
		void OnChildrenParseStarted(){
			this->stack.push_back(
					T_Pair(this->chain, this->lastInChain)
				);
		}

		//override
		void OnStringParsed(const char* s, ting::u32 size){
			ting::Ptr<Node> node = Node::New();
			node->SetValue(std::string(s, size));
			
			if(this->chain.IsNotValid()){
				this->chain = node;
				this->lastInChain = this->chain.operator->();
			}else{
				this->lastInChain->InsertNext(node);
				this->lastInChain = this->lastInChain->Next();
			}
		}
		
		ting::Ptr<Node> chain;
		Node* lastInChain;
		
		Listener() :
				chain(Node::New()),//create root node
				lastInChain(this->chain.operator->())
		{}
	} listener;
	
	listener.OnChildrenParseStarted();
	stob::Parse(fi, listener);
	listener.OnChildrenParseFinished();
	
	return listener.chain;
}
