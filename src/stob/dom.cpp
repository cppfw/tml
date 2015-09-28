#include "dom.hpp"

#include <vector>
#include <tuple>
#include <cstdint>
//#include <iosfwd>


#include <utki/debug.hpp>
#include <papki/BufferFile.hpp>
#include <papki/MemoryFile.hpp>

#include "parser.hpp"



using namespace stob;



namespace{
utki::MemoryPool<sizeof(Node), 1024> memoryPool;
}



void* Node::operator new(size_t size){
	ASSERT(size == sizeof(Node))

	return memoryPool.Alloc_ts();
}



void Node::operator delete(void* p)noexcept{
	memoryPool.Free_ts(p);
}



stob::Node::NodeAndPrev Node::Next(const char* value)noexcept{
	Node* prev = this;
	for(Node* n = this->Next(); n; prev = n, n = n->Next()){
		if(n->operator==(value)){
			return NodeAndPrev(prev, n);
		}
	}
	return NodeAndPrev(prev, 0);
}



stob::Node::NodeAndPrev Node::Child(const char* value)noexcept{
	if(!this->children){
		return NodeAndPrev(0, 0);
	}

	return this->children->ThisOrNext(value);
}



stob::Node::NodeAndPrev Node::ChildNonProperty()noexcept{
	if(!this->children){
		return NodeAndPrev(0, 0);
	}

	if(!this->children->IsProperty()){
		return NodeAndPrev(0, this->children.operator->());
	}

	return this->children->NextNonProperty();
}



stob::Node::NodeAndPrev Node::ChildProperty()noexcept{
	if(!this->children){
		return NodeAndPrev(0, 0);
	}

	if(this->children->IsProperty()){
		return NodeAndPrev(0, this->children.operator->());
	}

	return this->children->NextProperty();
}



stob::Node::NodeAndPrev Node::NextNonProperty()noexcept{
	Node* prev = this;
	for(Node* n = this->Next(); n; prev = n, n = n->Next()){
		if(!n->IsProperty()){
			return NodeAndPrev(prev, n);
		}
	}
	return NodeAndPrev(prev, 0);
}



stob::Node::NodeAndPrev Node::NextProperty()noexcept{
	Node* prev = this;
	for(Node* n = this->Next(); n; prev = n, n = n->Next()){
		if(n->IsProperty()){
			return NodeAndPrev(prev, n);
		}
	}
	return NodeAndPrev(prev, 0);
}



Node* Node::AddProperty(const char* propName){
	auto p = utki::makeUnique<Node>();
	p->SetValue(propName);
	p->SetNext(this->removeChildren());
	this->SetChildren(std::move(p));

	this->Child()->SetChildren(utki::makeUnique<Node>());

	return this->Child()->Child();
}



namespace{

bool CanStringBeUnquoted(const char* s, size_t& out_length, unsigned& out_numEscapes){
//	TRACE(<< "CanStringBeUnquoted(): enter" << std::endl)

	out_numEscapes = 0;
	out_length = 0;

	if(s == 0){//empty string is can be unquoted when it has children, so return true.
		return true;
	}

	bool ret = true;
	for(; *s != 0; ++s, ++out_length){
//		TRACE(<< "CanStringBeUnquoted(): c = " << (*c) << std::endl)
		switch(*s){
			case '\t':
			case '\n':
			case '\r':
			case '\\':
			case '"':
				++out_numEscapes;
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


void MakeEscapedString(const char* str, utki::Buf<std::uint8_t> out){
	std::uint8_t *p = out.begin();
	for(const char* c = str; *c != 0; ++c){
		ASSERT(p != out.end())

		switch(*c){
			case '\t':
				*p = '\\';
				++p;
				ASSERT(p != out.end())
				*p = 't';
				break;
			case '\n':
				*p = '\\';
				++p;
				ASSERT(p != out.end())
				*p = 'n';
				break;
			case '\r':
				*p = '\\';
				++p;
				ASSERT(p != out.end())
				*p = 'r';
				break;
			case '\\':
				*p = '\\';
				++p;
				ASSERT(p != out.end())
				*p = '\\';
				break;
			case '"':
				*p = '\\';
				++p;
				ASSERT(p != out.end())
				*p = '"';
				break;
			default:
				*p = *c;
				break;
		}
		++p;
	}
}


void writeChainInternal(const stob::Node* chain, papki::File& fi, bool formatted, unsigned indentation){
	ASSERT(chain)

	std::array<std::uint8_t, 1> quote = {{'"'}};

	std::array<std::uint8_t, 1> lcurly = {{'{'}};

	std::array<std::uint8_t, 1> rcurly = {{'}'}};

	std::array<std::uint8_t, 1> space = {{' '}};

	std::array<std::uint8_t, 1> tab = {{'\t'}};

	std::array<std::uint8_t, 1> newLine = {{'\n'}};

	//used to detect case of two adjacent unquoted strings without children, need to insert space between them
	bool prevWasUnquotedWithoutChildren = false;
	
	bool prevHadChildren = true;

	for(auto n = chain; n; n = n->Next()){
		//indent
		if(formatted){
			for(unsigned i = 0; i != indentation; ++i){
				fi.write(tab);
			}
		}

		//write node value

		unsigned numEscapes;
		size_t length;
		bool unqouted = CanStringBeUnquoted(n->Value(), length, numEscapes);

		if(!unqouted){
			fi.write(quote);

			if(numEscapes == 0){
				fi.write(utki::Buf<std::uint8_t>(
						const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(n->Value())),
						length
					));
			}else{
				std::vector<std::uint8_t> buf(length + numEscapes);

				MakeEscapedString(n->Value(), buf);

				fi.write(buf);
			}

			fi.write(quote);
		}else{
			bool isQuotedEmptyString = false;
			if(n->ValueLength() == 0){//if empty string
				if(!n->Child() || !prevHadChildren){
					isQuotedEmptyString = true;
				}
			}
			
			//unquoted string
			if(!formatted && prevWasUnquotedWithoutChildren && !isQuotedEmptyString){
				fi.write(space);
			}

			if(n->ValueLength() == 0){//if empty string
				if(isQuotedEmptyString){
					fi.write(quote);
					fi.write(quote);
				}
			}else{
				ASSERT(numEscapes == 0)
				fi.write(utki::Buf<std::uint8_t>(
						const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(n->Value())),
						length
					));
			}
		}

		prevHadChildren = (n->Child() != 0);
		if(n->Child() == 0){
			
			if(formatted){
				fi.write(newLine);
			}
			prevWasUnquotedWithoutChildren = (unqouted && n->ValueLength() != 0);
			continue;
		}else{
			prevWasUnquotedWithoutChildren = false;
		}

		if(!formatted){
			fi.write(lcurly);

			writeChainInternal(n->Child(), fi, false, 0);

			fi.write(rcurly);
		}else{
			if(n->Child()->Next() == 0 && n->Child()->Child() == 0){
				//if only one child and that child has no children

				fi.write(lcurly);
				writeChainInternal(n->Child(), fi, false, 0);
				fi.write(rcurly);
				fi.write(newLine);
			}else{
				fi.write(lcurly);
				fi.write(newLine);
				writeChainInternal(n->Child(), fi, true, indentation + 1);

				//indent
				for(unsigned i = 0; i != indentation; ++i){
					fi.write(tab);
				}
				fi.write(rcurly);
				fi.write(newLine);
			}
		}
	}//~for()
}
}//~namespace



void Node::WriteChain(papki::File& fi, bool formatted)const{
	papki::File::Guard fileGuard(fi, papki::File::E_Mode::CREATE);

	writeChainInternal(this, fi, formatted, 0);
}



std::unique_ptr<stob::Node> stob::Load(const papki::File& fi){
	class Listener : public stob::ParseListener{
		typedef std::pair<std::unique_ptr<Node>, Node*> T_Pair; //NOTE: use pair, because tuple does not work on iOS when adding it to vector, for some reason.
		std::vector<T_Pair> stack;

	public:
		std::unique_ptr<Node> chains;
		Node* lastChain;
		
		void OnChildrenParseFinished() override{
			std::get<1>(this->stack.back())->SetChildren(std::move(this->chains));
			this->chains = std::move(std::get<0>(this->stack.back()));
			this->lastChain = std::get<1>(this->stack.back());
			this->stack.pop_back();
		}

		void OnChildrenParseStarted() override{
			this->stack.emplace_back(
					std::make_pair(std::move(this->chains), this->lastChain)
				);
		}

		void OnStringParsed(const utki::Buf<char> str)override{
			auto node = utki::makeUnique<Node>(str);

			if(!this->chains){
				this->chains = std::move(node);
				this->lastChain = this->chains.operator->();
			}else{
				this->lastChain->InsertNext(std::move(node));
				this->lastChain = this->lastChain->Next();
			}
		}

		~Listener()noexcept{}
	} listener;

	stob::Parse(fi, listener);

	return std::move(listener.chains);
}



std::unique_ptr<stob::Node> Node::Clone()const{
	auto ret = utki::makeUnique<Node>(this->Value());

	if(!this->Child()){
		return ret;
	}

	auto c = this->Child()->Clone();

	{
		auto curChild = c.operator->();
		for(auto p = this->Child()->Next(); p; p = p->Next(), curChild = curChild->Next()){
			ASSERT(curChild)
			curChild->InsertNext(p->Clone());
		}
	}

	ret->SetChildren(std::move(c));
	return ret;
}


std::unique_ptr<Node> Node::CloneChain() const{
	auto ret = this->Clone();
	
	if(this->Next()){
		ret->SetNext(this->Next()->CloneChain());
	}
	
	return ret;
}


bool Node::operator==(const Node& n)const noexcept{
	if(!this->operator==(n.Value())){
		return false;
	}
	
	const stob::Node* c = this->Child();
	const stob::Node* cn = n.Child();
	for(; c && cn; c = c->Next(), cn = cn->Next()){
		if(!c->operator==(cn->Value())){
			return false;
		}
	}
	
	//if not equal number of children
	if((c && !cn) || (!c && cn)){
		return false;
	}
	
	return true;
}



std::unique_ptr<Node> stob::Parse(const char *str){
	if(!str){
		return nullptr;
	}
	
	size_t len = strlen(str);
	
	//TODO: make const Buffer file
	papki::BufferFile fi(utki::Buf<std::uint8_t>(reinterpret_cast<std::uint8_t*>(const_cast<char*>(str)), len));
	
	return Load(fi);
}



std::string Node::ChainToString(bool formatted)const{
	papki::MemoryFile fi;
	
	this->WriteChain(fi, formatted);
	
	auto data = fi.resetData();
	
	return std::string(reinterpret_cast<char*>(&*data.begin()), data.size());
}


size_t Node::count() const noexcept{
	size_t ret = 0;
	
	for(auto c = this->children.get(); c; c = c->Next()){
		++ret;
	}
	
	return ret;
}


Node::NodeAndPrev Node::child(size_t index)noexcept{
	auto c = this->Child();
	decltype(c) prev = nullptr;
	
	for(; c && index != 0; prev = c, c = c->Next(), --index){}
	
	if(index == 0){
		return NodeAndPrev(prev, c);
	}
	
	return NodeAndPrev(nullptr, nullptr);
}

std::unique_ptr<Node> Node::removeChild(const stob::Node* c)noexcept{
	auto ch = this->Child();
	if(ch == c){
		return this->RemoveFirstChild();
	}
	auto prev = ch;
	ch = ch->Next();
	
	for(; ch; prev = ch, ch = ch->Next()){
		if(ch == c){
			return prev->RemoveNext();
		}
	}
	
	return nullptr;
}
