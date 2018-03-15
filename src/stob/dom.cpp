#include "dom.hpp"

#include <vector>
#include <tuple>
#include <cstdint>

#include <utki/debug.hpp>
#include <papki/BufferFile.hpp>
#include <papki/MemoryFile.hpp>

#include "parser.hpp"



using namespace stob;



stob::Node::NodeAndPrev Node::next(const char* value)noexcept{
	Node* prev = this;
	for(Node* n = this->next(); n; prev = n, n = n->next()){
		if(n->operator==(value)){
			return NodeAndPrev(prev, n);
		}
	}
	return NodeAndPrev(prev, 0);
}



stob::Node::NodeAndPrev Node::child(const char* value)noexcept{
	if(!this->children){
		return NodeAndPrev(0, 0);
	}

	return this->children->thisOrNext(value);
}



stob::Node::NodeAndPrev Node::childNonProperty()noexcept{
	if(!this->children){
		return NodeAndPrev(0, 0);
	}

	if(!this->children->isProperty()){
		return NodeAndPrev(0, this->children.operator->());
	}

	return this->children->nextNonProperty();
}



stob::Node::NodeAndPrev Node::childProperty()noexcept{
	if(!this->children){
		return NodeAndPrev(0, 0);
	}

	if(this->children->isProperty()){
		return NodeAndPrev(0, this->children.operator->());
	}

	return this->children->nextProperty();
}



stob::Node::NodeAndPrev Node::nextNonProperty()noexcept{
	Node* prev = this;
	for(Node* n = this->next(); n; prev = n, n = n->next()){
		if(!n->isProperty()){
			return NodeAndPrev(prev, n);
		}
	}
	return NodeAndPrev(prev, 0);
}



stob::Node::NodeAndPrev Node::nextProperty()noexcept{
	Node* prev = this;
	for(Node* n = this->next(); n; prev = n, n = n->next()){
		if(n->isProperty()){
			return NodeAndPrev(prev, n);
		}
	}
	return NodeAndPrev(prev, 0);
}



Node* Node::addProperty(const char* propName){
	auto p = utki::makeUnique<Node>();
	p->setValue(propName);
	p->setNext(this->removeChildren());
	this->setChildren(std::move(p));

	this->child()->setChildren(utki::makeUnique<Node>());

	return this->child()->child();
}



namespace{

bool canStringBeUnquoted(const char* s, size_t& out_length, unsigned& out_numEscapes){
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

	for(auto n = chain; n; n = n->next()){
		//indent
		if(formatted){
			for(unsigned i = 0; i != indentation; ++i){
				fi.write(utki::wrapBuf(tab));
			}
		}

		//write node value

		unsigned numEscapes;
		size_t length;
		bool unqouted = canStringBeUnquoted(n->value(), length, numEscapes);

		if(!unqouted){
			fi.write(utki::wrapBuf(quote));

			if(numEscapes == 0){
				fi.write(utki::Buf<std::uint8_t>(
						const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(n->value())),
						length
					));
			}else{
				std::vector<std::uint8_t> buf(length + numEscapes);

				MakeEscapedString(n->value(), utki::wrapBuf(buf));

				fi.write(utki::wrapBuf(buf));
			}

			fi.write(utki::wrapBuf(quote));
		}else{
			bool isQuotedEmptyString = false;
			if(n->length() == 0){//if empty string
				if(!n->child() || !prevHadChildren){
					isQuotedEmptyString = true;
				}
			}
			
			//unquoted string
			if(!formatted && prevWasUnquotedWithoutChildren && !isQuotedEmptyString){
				fi.write(utki::wrapBuf(space));
			}

			if(n->length() == 0){//if empty string
				if(isQuotedEmptyString){
					fi.write(utki::wrapBuf(quote));
					fi.write(utki::wrapBuf(quote));
				}
			}else{
				ASSERT(numEscapes == 0)
				fi.write(utki::Buf<std::uint8_t>(
						const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(n->value())),
						length
					));
				if(!n->child() && length == 1 && n->value()[0] == 'R'){
					fi.write(utki::wrapBuf(space));
				}
			}
		}

		prevHadChildren = (n->child() != nullptr);
		if(!n->child()){
			if(formatted){
				fi.write(utki::wrapBuf(newLine));
			}
			prevWasUnquotedWithoutChildren = (unqouted && n->length() != 0);
			continue;
		}else{
			prevWasUnquotedWithoutChildren = false;
		}

		if(!formatted){
			fi.write(utki::wrapBuf(lcurly));

			writeChainInternal(n->child(), fi, false, 0);

			fi.write(utki::wrapBuf(rcurly));
		}else{
			if(n->child()->next() == 0 && n->child()->child() == 0){
				//if only one child and that child has no children

				fi.write(utki::wrapBuf(lcurly));
				writeChainInternal(n->child(), fi, false, 0);
				fi.write(utki::wrapBuf(rcurly));
				fi.write(utki::wrapBuf(newLine));
			}else{
				fi.write(utki::wrapBuf(lcurly));
				fi.write(utki::wrapBuf(newLine));
				writeChainInternal(n->child(), fi, true, indentation + 1);

				//indent
				for(unsigned i = 0; i != indentation; ++i){
					fi.write(utki::wrapBuf(tab));
				}
				fi.write(utki::wrapBuf(rcurly));
				fi.write(utki::wrapBuf(newLine));
			}
		}
	}//~for()
}
}//~namespace



void Node::writeChain(papki::File& fi, bool formatted)const{
	papki::File::Guard fileGuard(fi, papki::File::E_Mode::CREATE);

	writeChainInternal(this, fi, formatted, 0);
}



std::unique_ptr<stob::Node> stob::load(const papki::File& fi){
	class Listener : public stob::ParseListener{
		typedef std::pair<std::unique_ptr<Node>, Node*> T_Pair; //NOTE: use pair, because tuple does not work on iOS when adding it to vector, for some reason.
		std::vector<T_Pair> stack;

	public:
		std::unique_ptr<Node> chains;
		Node* lastChain;
		
		void onChildrenParseFinished() override{
			std::get<1>(this->stack.back())->setChildren(std::move(this->chains));
			this->chains = std::move(std::get<0>(this->stack.back()));
			this->lastChain = std::get<1>(this->stack.back());
			this->stack.pop_back();
		}

		void onChildrenParseStarted() override{
			this->stack.emplace_back(
					std::make_pair(std::move(this->chains), this->lastChain)
				);
		}

		void onStringParsed(const utki::Buf<char> str)override{
			auto node = utki::makeUnique<Node>(str);

			if(!this->chains){
				this->chains = std::move(node);
				this->lastChain = this->chains.operator->();
			}else{
				this->lastChain->insertNext(std::move(node));
				this->lastChain = this->lastChain->next();
			}
		}

		~Listener()noexcept{}
	} listener;

	stob::parse(fi, listener);

	return std::move(listener.chains);
}



std::unique_ptr<stob::Node> Node::clone()const{
	auto ret = utki::makeUnique<Node>(this->value());

	if(!this->child()){
		return ret;
	}

	auto c = this->child()->clone();

	{
		auto curChild = c.operator->();
		for(auto p = this->child()->next(); p; p = p->next(), curChild = curChild->next()){
			ASSERT(curChild)
			curChild->insertNext(p->clone());
		}
	}

	ret->setChildren(std::move(c));
	return ret;
}


std::unique_ptr<Node> Node::cloneChain() const{
	auto ret = this->clone();
	
	if(this->next()){
		ret->setNext(this->next()->cloneChain());
	}
	
	return ret;
}


bool Node::operator==(const Node& n)const noexcept{
	if(!this->operator==(n.value())){
		return false;
	}
	
	const stob::Node* c = this->child();
	const stob::Node* cn = n.child();
	for(; c && cn; c = c->next(), cn = cn->next()){
		if(!c->operator==(cn->value())){
			return false;
		}
	}
	
	//if not equal number of children
	if((c && !cn) || (!c && cn)){
		return false;
	}
	
	return true;
}



std::unique_ptr<Node> stob::parse(const char *str){
	if(!str){
		return nullptr;
	}
	
	size_t len = strlen(str);
	
	//TODO: make const Buffer file
	papki::BufferFile fi(utki::Buf<std::uint8_t>(reinterpret_cast<std::uint8_t*>(const_cast<char*>(str)), len));
	
	return load(fi);
}



std::string Node::chainToString(bool formatted)const{
	papki::MemoryFile fi;
	
	this->writeChain(fi, formatted);
	
	auto data = fi.resetData();
	
	return std::string(reinterpret_cast<char*>(&*data.begin()), data.size());
}


size_t Node::count() const noexcept{
	size_t ret = 0;
	
	for(auto c = this->children.get(); c; c = c->next()){
		++ret;
	}
	
	return ret;
}


Node::NodeAndPrev Node::child(size_t index)noexcept{
	auto c = this->child();
	decltype(c) prev = nullptr;
	
	for(; c && index != 0; prev = c, c = c->next(), --index){}
	
	if(index == 0){
		return NodeAndPrev(prev, c);
	}
	
	return NodeAndPrev(nullptr, nullptr);
}

std::unique_ptr<Node> Node::removeChild(const stob::Node* c)noexcept{
	auto ch = this->child();
	if(ch == c){
		return this->removeFirstChild();
	}
	auto prev = ch;
	ch = ch->next();
	
	for(; ch; prev = ch, ch = ch->next()){
		if(ch == c){
			return prev->removeNext();
		}
	}
	
	return nullptr;
}

void Node::setValueInternal(const utki::Buf<char> str) {
	if (str.size() == 0) {
		this->value_v = nullptr;
		return;
	}

	this->value_v = decltype(this->value_v)(new char[str.size() + 1]);
	memcpy(this->value_v.get(), str.begin(), str.size());
	this->value_v[str.size()] = 0; //null-terminate
}


std::u32string Node::asU32String() const noexcept{
	std::vector<char32_t> v;
	for(auto i = this->asUTF8(); !i.isEnd(); ++i){
		v.push_back(i.character());
	}
	
	return std::u32string(&*v.begin(), v.size());
}


std::unique_ptr<Node> Node::cloneChildren() const {
	if(!this->child()){
		return nullptr;
	}
	return this->child()->cloneChain();
}

size_t Node::countChain() const noexcept{
	size_t ret = 0;
	
	auto n = this;
	for(; n; n = n->next()){
		++ret;
	}
	
	return ret;
}



std::unique_ptr<Node> Node::replace(std::unique_ptr<Node> chain) {
	if(!chain){
		throw stob::Exc("could not replace node by null chain");
	}

	//find chain's last node
	Node* last = chain.get();
	for(; last->next(); ){
		last = last->next();
	}
	
	if(chain->next()){
		last->setNext(this->chopNext());
		this->setNext(chain->chopNext());
	}
	
	swap(*this, *chain);
	
	return chain;
}


std::unique_ptr<Node> Node::replace(const Node& chain){
	//clone chain
	
	auto head = chain.clone();

	auto last = head.get();
	
	for(auto curNode = chain.next(); curNode; curNode = curNode->next()){
		last->setNext(curNode->clone());
		last = last->next();
	}
	
	if(head->next()){
		last->setNext(this->chopNext());
		this->setNext(head->chopNext());
	}
	
	swap(*this, *head);
	
	return head;
}

std::uint32_t Node::asUint32() const noexcept{
	if(!this->value()){
		return 0;
	}
	return std::uint32_t(strtoul(this->value(), nullptr, 0));
}

bool Node::asBool() const noexcept{
	return strcmp(this->value(), "true") == 0;
}

double Node::asDouble() const noexcept{
	if(!this->value()){
		return 0;
	}
	return strtod(this->value(), nullptr);
}

float Node::asFloat() const noexcept{
	if(!this->value()){
		return 0;
	}
	return strtof(this->value(), nullptr);
}

std::int32_t Node::asInt32() const noexcept{
	if(!this->value()){
		return 0;
	}
	return std::int32_t(strtol(this->value(), nullptr, 0));
}

std::int64_t Node::asInt64() const noexcept{
	if(!this->value()){
		return 0;
	}
	return std::int64_t(strtoll(this->value(), nullptr, 0));
}

long double Node::asLongDouble() const noexcept{
#if M_OS_NAME == M_OS_NAME_ANDROID //TODO: use strtold() when it becomes available on Android
	return this->asDouble();
#else
	if(!this->value()){
		return 0;
	}
	return strtold(this->value(), nullptr);
#endif
}

std::uint64_t Node::asUint64() const noexcept{
	if(!this->value()){
		return 0;
	}
	return std::uint64_t(strtoull(this->value(), nullptr, 0));
}
