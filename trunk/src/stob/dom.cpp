#include "dom.hpp"

#include <vector>

#include <ting/debug.hpp>
#include <ting/Array.hpp>

#include "parser.hpp"



using namespace stob;



namespace{
ting::MemoryPool<sizeof(Node), 100> memoryPool;
}



void* Node::operator new(size_t size){
	ASSERT(size == sizeof(Node))

	return memoryPool.Alloc_ts();
}



void Node::operator delete(void* p)throw(){
	memoryPool.Free_ts(p);
}



stob::Node::NodeAndPrev Node::Next(const char* value)throw(){
	Node* prev = this;
	for(Node* n = this->Next(); n; prev = n, n = n->Next()){
		if(n->operator ==(value)){
			return NodeAndPrev(prev, n);
		}
	}
	return NodeAndPrev(prev, 0);
}



stob::Node::NodeAndPrev Node::Child(const char* value)throw(){
	if(this->children.IsNotValid()){
		return NodeAndPrev(0, 0);
	}

	if(this->children->operator==(value)){
		return NodeAndPrev(0, this->children.operator->());
	}

	return this->children->Next(value);
}



stob::Node::NodeAndPrev Node::ChildNonProperty()throw(){
	if(this->children.IsNotValid()){
		return NodeAndPrev(0, 0);
	}

	if(this->children->IsProperty()){
		return NodeAndPrev(0, this->children.operator->());
	}

	return this->children->NextNonProperty();
}



stob::Node::NodeAndPrev Node::ChildProperty()throw(){
	if(this->children.IsNotValid()){
		return NodeAndPrev(0, 0);
	}

	if(!this->children->IsProperty()){
		return NodeAndPrev(0, this->children.operator->());
	}

	return this->children->NextProperty();
}



stob::Node::NodeAndPrev Node::NextNonProperty()throw(){
	Node* prev = this;
	for(Node* n = this->Next(); n; prev = n, n = n->Next()){
		if(n->IsProperty()){
			return NodeAndPrev(prev, n);
		}
	}
	return NodeAndPrev(prev, 0);
}



stob::Node::NodeAndPrev Node::NextProperty()throw(){
	Node* prev = this;
	for(Node* n = this->Next(); n; prev = n, n = n->Next()){
		if(!n->IsProperty()){
			return NodeAndPrev(prev, n);
		}
	}
	return NodeAndPrev(prev, 0);
}



Node* Node::AddProperty(const char* propName){
	ting::Ptr<Node> p = Node::New();
	p->SetValue(propName);
	p->SetNext(this->RemoveChildren());
	this->SetChildren(p);

	this->Child()->SetChildren(Node::New());

	return this->Child()->Child();
}



namespace{

bool CanStringBeUnquoted(const char* s, size_t& length, unsigned& numEscapes){
//	TRACE(<< "CanStringBeUnquoted(): enter" << std::endl)

	numEscapes = 0;
	length = 0;

	if(s == 0){//empty string is always quoted
		return false;
	}

	bool ret = true;
	for(; *s != 0; ++s, ++length){
//		TRACE(<< "CanStringBeUnquoted(): c = " << (*c) << std::endl)
		switch(*s){
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


void MakeEscapedString(const char* str, ting::Buffer<ting::u8>& out){
	ting::u8 *p = out.Begin();
	for(const char* c = str; *c != 0; ++c){
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
		size_t length;
		bool unqouted = CanStringBeUnquoted(n->Value(), length, numEscapes);

		if(!unqouted){
			fi.Write(quote);

			if(numEscapes == 0){
				fi.Write(ting::Buffer<ting::u8>(
						const_cast<ting::u8*>(reinterpret_cast<const ting::u8*>(n->Value())),
						length
					));
			}else{
				ting::Array<ting::u8> buf(length + numEscapes);

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
					const_cast<ting::u8*>(reinterpret_cast<const ting::u8*>(n->Value())),
					length
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
			ting::Ptr<Node> node = Node::New(s, size);

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



ting::Ptr<stob::Node> Node::Clone()const{
	ting::Ptr<Node> ret = Node::New(this->Value());

	if(!this->Child()){
		return ret;
	}

	ting::Ptr<stob::Node> c = this->Child()->Clone();

	{
		stob::Node* curChild = c.operator->();
		for(const stob::Node *p = this->Child()->Next(); p; p = p->Next(), curChild = curChild->Next()){
			ASSERT(curChild)
			curChild->InsertNext(p->Clone());
		}
	}

	ret->SetChildren(c);
	return ret;
}
