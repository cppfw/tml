#include "dom.hpp"

#include <vector>

#include <ting/debug.hpp>
#include <ting/Array.hpp>

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



std::pair<Node*, Node*> Node::Next(const std::string& value)throw(){
	Node* prev = this;
	for(Node* n = this->Next(); n; prev = n, n = n->Next()){
		if(n->Value() == value){
			return std::pair<Node*, Node*>(prev, n);
		}
	}
	return std::pair<Node*, Node*>(0, 0);
}



std::pair<const Node*, const Node*> Node::Next(const std::string& value)const throw(){
	return const_cast<Node const*>(this)->Next(value);
}



std::pair<Node*, Node*> Node::Child(const std::string& value)throw(){
	if(this->children.IsNotValid()){
		return std::pair<Node*, Node*>(0, 0);
	}
	
	if(this->children->Value() == value){
		return std::pair<Node*, Node*>(0, this->children.operator->());
	}
	
	return this->children->Next(value);
}



std::pair<const Node*, const Node*> Node::Child(const std::string& value)const throw(){
	return const_cast<Node const*>(this)->Child(value);
}



namespace{

bool CanStringBeUnquoted(const std::string& s, unsigned& numEscapes){
//	TRACE(<< "CanStringBeUnquoted(): enter" << std::endl)
	
	numEscapes = 0;
	
	if(s.size() == 0){//empty string is always quoted
		return false;
	}
	
	bool ret = true;
	for(const char* c = s.c_str(); *c != 0; ++c){
//		TRACE(<< "CanStringBeUnquoted(): c = " << (*c) << std::endl)
		switch(*c){
			case '\t':
			case '\n':
			case '\r':
			case '"':
				++numEscapes;
			case '{':
			case '}':
			case ' ':
				ret = false;
				break;
			default:
				break;
		}
	}
	return ret;
}


void MakeEscapedString(const std::string& str, ting::Buffer<ting::u8>& out){
	ting::u8 *p = out.Begin();
	for(const char* c = str.c_str(); *c != 0; ++c){
		ASSERT(p != out.End())
		
		switch(*c){
			case '\t':
				*p = '\\';
				++p;
				ASSERT(p != out.End())
				*p = 't';
				break;
			case '\n':
				*p = '\\';
				++p;
				ASSERT(p != out.End())
				*p = 'n';
				break;
			case '\r':
				*p = '\\';
				++p;
				ASSERT(p != out.End())
				*p = 'r';
				break;
			case '"':
				*p = '\\';
				++p;
				ASSERT(p != out.End())
				*p = '"';
				break;
			default:
				*p = *c;
				break;
		}
		++p;
	}
}


void WriteNode(const stob::Node* node, ting::fs::File& fi, bool formatted, unsigned indentation){
	ASSERT(node)
	
	ting::StaticBuffer<ting::u8, 1> quote;
	quote[0] = '"';
	
	ting::StaticBuffer<ting::u8, 1> lcurly;
	lcurly[0] = '{';
	
	ting::StaticBuffer<ting::u8, 1> rcurly;
	rcurly[0] = '}';
	
	ting::StaticBuffer<ting::u8, 1> space;
	space[0] = ' ';
	
	ting::StaticBuffer<ting::u8, 1> tab;
	tab[0] = '\t';
	
	ting::StaticBuffer<ting::u8, 1> newLine;
	newLine[0] = '\n';
	
	//used to detect case of two adjacent unquoted strings without children, need to insert space between them
	bool prevWasUnquotedWithoutChildren = false;
	
	for(const Node* n = node->Child(); n; n = n->Next()){
		//indent
		if(formatted){
			for(unsigned i = 0; i != indentation; ++i){
				fi.Write(tab);
			}
		}
		
		
		//write node value
		
		unsigned numEscapes;
		bool unqouted = CanStringBeUnquoted(n->Value(), numEscapes);
		
		if(!unqouted){
			fi.Write(quote);
			
			if(numEscapes == 0){
				fi.Write(ting::Buffer<ting::u8>(
						const_cast<ting::u8*>(reinterpret_cast<const ting::u8*>(n->Value().c_str())),
						n->Value().size()
					));
			}else{
				ting::Array<ting::u8> buf(n->Value().size() + numEscapes);
				
				MakeEscapedString(n->Value(), buf);
				
				fi.Write(buf);
			}
			
			fi.Write(quote);
		}else{
			//unquoted string
			if(!formatted && prevWasUnquotedWithoutChildren){
				fi.Write(space);
			}
			
			ASSERT(numEscapes == 0)
			fi.Write(ting::Buffer<ting::u8>(
					const_cast<ting::u8*>(reinterpret_cast<const ting::u8*>(n->Value().c_str())),
					n->Value().size()
				));
		}
		
		if(n->Child() == 0){
			if(formatted){
				fi.Write(newLine);
			}
			prevWasUnquotedWithoutChildren = unqouted;
			continue;
		}else{
			prevWasUnquotedWithoutChildren = false;
		}
		
		if(!formatted){
			fi.Write(lcurly);
			
			WriteNode(n, fi, false, 0);
			
			fi.Write(rcurly);
		}else{
			if(n->Child()->Next() == 0 && n->Child()->Child() == 0){
				//if only one child and that child has no children
				fi.Write(space);
				fi.Write(lcurly);
				WriteNode(n, fi, false, 0);
				fi.Write(rcurly);
				fi.Write(newLine);
			}else{
				fi.Write(lcurly);
				fi.Write(newLine);
				WriteNode(n, fi, true, indentation + 1);
				
				//indent
				for(unsigned i = 0; i != indentation; ++i){
					fi.Write(tab);
				}
				fi.Write(rcurly);
				fi.Write(newLine);
			}
		}
	}//~for()
}
}//~namespace



void Node::Write(ting::fs::File& fi, bool formatted){
	ting::fs::File::Guard fileGuard(fi, ting::fs::File::CREATE);
	
	WriteNode(this, fi, formatted, 0);
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
		
		~Listener()throw(){}
	} listener;
	
	listener.OnChildrenParseStarted();
	stob::Parse(fi, listener);
	listener.OnChildrenParseFinished();
	
	return listener.chain;
}
