#include "dom.hpp"

#include <vector>
#include <tuple>
#include <cstdint>

#include <utki/debug.hpp>
#include <papki/BufferFile.hpp>
#include <papki/MemoryFile.hpp>

#include "parser.hpp"



using namespace puu;



puu::node::nodeAndPrev node::next(const char* value)noexcept{
	node* prev = this;
	for(node* n = this->next(); n; prev = n, n = n->next()){
		if(n->operator==(value)){
			return nodeAndPrev(prev, n);
		}
	}
	return nodeAndPrev(prev, 0);
}



puu::node::nodeAndPrev node::child(const char* value)noexcept{
	if(!this->children){
		return nodeAndPrev(0, 0);
	}

	return this->children->thisOrNext(value);
}



puu::node::nodeAndPrev node::childNonProperty()noexcept{
	if(!this->children){
		return nodeAndPrev(0, 0);
	}

	if(!this->children->isProperty()){
		return nodeAndPrev(0, this->children.operator->());
	}

	return this->children->nextNonProperty();
}



puu::node::nodeAndPrev node::childProperty()noexcept{
	if(!this->children){
		return nodeAndPrev(0, 0);
	}

	if(this->children->isProperty()){
		return nodeAndPrev(0, this->children.operator->());
	}

	return this->children->nextProperty();
}



puu::node::nodeAndPrev node::nextNonProperty()noexcept{
	node* prev = this;
	for(node* n = this->next(); n; prev = n, n = n->next()){
		if(!n->isProperty()){
			return nodeAndPrev(prev, n);
		}
	}
	return nodeAndPrev(prev, 0);
}



puu::node::nodeAndPrev node::nextProperty()noexcept{
	node* prev = this;
	for(node* n = this->next(); n; prev = n, n = n->next()){
		if(n->isProperty()){
			return nodeAndPrev(prev, n);
		}
	}
	return nodeAndPrev(prev, 0);
}



node* node::addProperty(const char* propName){
	auto p = utki::makeUnique<node>();
	p->set_value(propName);
	p->setNext(this->remove_children());
	this->set_children(std::move(p));

	this->child()->set_children(utki::makeUnique<node>());

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


void writeChainInternal(const puu::node* chain, papki::File& fi, bool formatted, unsigned indentation){
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
		bool unqouted = canStringBeUnquoted(n->get_value(), length, numEscapes);

		if(!unqouted){
			fi.write(utki::wrapBuf(quote));

			if(numEscapes == 0){
				fi.write(utki::Buf<std::uint8_t>(
						const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(n->get_value())),
						length
					));
			}else{
				std::vector<std::uint8_t> buf(length + numEscapes);

				MakeEscapedString(n->get_value(), utki::wrapBuf(buf));

				fi.write(utki::wrapBuf(buf));
			}

			fi.write(utki::wrapBuf(quote));
		}else{
			bool isQuotedEmptyString = false;
			if(n->get_length() == 0){//if empty string
				if(!n->child() || !prevHadChildren){
					isQuotedEmptyString = true;
				}
			}

			//unquoted string
			if(!formatted && prevWasUnquotedWithoutChildren && !isQuotedEmptyString){
				fi.write(utki::wrapBuf(space));
			}

			if(n->get_length() == 0){//if empty string
				if(isQuotedEmptyString){
					fi.write(utki::wrapBuf(quote));
					fi.write(utki::wrapBuf(quote));
				}
			}else{
				ASSERT(numEscapes == 0)
				fi.write(utki::Buf<std::uint8_t>(
						const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(n->get_value())),
						length
					));
				if(!n->child() && length == 1 && n->get_value()[0] == 'R'){
					fi.write(utki::wrapBuf(space));
				}
			}
		}

		prevHadChildren = (n->child() != nullptr);
		if(!n->child()){
			if(formatted){
				fi.write(utki::wrapBuf(newLine));
			}
			prevWasUnquotedWithoutChildren = (unqouted && n->get_length() != 0);
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



void node::writeChain(papki::File& fi, bool formatted)const{
	papki::File::Guard fileGuard(fi, papki::File::E_Mode::CREATE);

	writeChainInternal(this, fi, formatted, 0);
}



std::unique_ptr<puu::node> puu::load(const papki::File& fi){
	class Listener : public puu::listener{
		typedef std::pair<std::unique_ptr<node>, node*> T_Pair; //NOTE: use pair, because tuple does not work on iOS when adding it to vector, for some reason.
		std::vector<T_Pair> stack;

	public:
		std::unique_ptr<node> chains;
		node* lastChain;

		void on_children_parse_finished() override{
			std::get<1>(this->stack.back())->set_children(std::move(this->chains));
			this->chains = std::move(std::get<0>(this->stack.back()));
			this->lastChain = std::get<1>(this->stack.back());
			this->stack.pop_back();
		}

		void on_children_parse_started() override{
			this->stack.emplace_back(
					std::make_pair(std::move(this->chains), this->lastChain)
				);
		}

		void on_string_parsed(const utki::Buf<char> str)override{
			auto n = utki::makeUnique<node>(str);

			if(!this->chains){
				this->chains = std::move(n);
				this->lastChain = this->chains.operator->();
			}else{
				this->lastChain->insertNext(std::move(n));
				this->lastChain = this->lastChain->next();
			}
		}

		~Listener()noexcept{}
	} listener;

	puu::parse(fi, listener);

	return std::move(listener.chains);
}



std::unique_ptr<puu::node> node::clone()const{
	auto ret = utki::makeUnique<node>(this->get_value());

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

	ret->set_children(std::move(c));
	return ret;
}


std::unique_ptr<node> node::cloneChain() const{
	auto ret = this->clone();

	if(this->next()){
		ret->setNext(this->next()->cloneChain());
	}

	return ret;
}


bool node::operator==(const node& n)const noexcept{
	if(!this->operator==(n.get_value())){
		return false;
	}

	const puu::node* c = this->child();
	const puu::node* cn = n.child();
	for(; c && cn; c = c->next(), cn = cn->next()){
		if(!c->operator==(cn->get_value())){
			return false;
		}
	}

	//if not equal number of children
	if((c && !cn) || (!c && cn)){
		return false;
	}

	return true;
}



std::unique_ptr<node> puu::parse(const char *str){
	if(!str){
		return nullptr;
	}

	size_t len = strlen(str);

	//TODO: make const Buffer file
	papki::BufferFile fi(utki::Buf<std::uint8_t>(reinterpret_cast<std::uint8_t*>(const_cast<char*>(str)), len));

	return load(fi);
}



std::string node::chainToString(bool formatted)const{
	papki::MemoryFile fi;

	this->writeChain(fi, formatted);

	auto data = fi.resetData();

	return std::string(reinterpret_cast<char*>(&*data.begin()), data.size());
}


size_t node::count() const noexcept{
	size_t ret = 0;

	for(auto c = this->children.get(); c; c = c->next()){
		++ret;
	}

	return ret;
}


node::nodeAndPrev node::child(size_t index)noexcept{
	auto c = this->child();
	decltype(c) prev = nullptr;

	for(; c && index != 0; prev = c, c = c->next(), --index){}

	if(index == 0){
		return nodeAndPrev(prev, c);
	}

	return nodeAndPrev(nullptr, nullptr);
}

std::unique_ptr<node> node::remove_child(const puu::node* c)noexcept{
	auto ch = this->child();
	if(ch == c){
		return this->remove_first_child();
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

void node::set_value_internal(const utki::Buf<char> str) {
	if (str.size() == 0) {
		this->value_v = nullptr;
		return;
	}

	this->value_v = decltype(this->value_v)(new char[str.size() + 1]);
	memcpy(this->value_v.get(), str.begin(), str.size());
	this->value_v[str.size()] = 0; //null-terminate
}


std::u32string node::as_u32string() const noexcept{
	std::vector<char32_t> v;
	for(auto i = this->as_utf8(); !i.isEnd(); ++i){
		v.push_back(i.character());
	}

	return std::u32string(&*v.begin(), v.size());
}


std::unique_ptr<node> node::cloneChildren() const {
	if(!this->child()){
		return nullptr;
	}
	return this->child()->cloneChain();
}

size_t node::count_chain() const noexcept{
	size_t ret = 0;

	auto n = this;
	for(; n; n = n->next()){
		++ret;
	}

	return ret;
}



std::unique_ptr<node> node::replace(std::unique_ptr<node> chain) {
	if(!chain){
		throw puu::Exc("could not replace node by null chain");
	}

	//find chain's last node
	node* last = chain.get();
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


std::unique_ptr<node> node::replace(const node& chain){
	//clone chain

	auto head = chain.clone();

	auto last = head.get();

	for(auto curnode = chain.next(); curnode; curnode = curnode->next()){
		last->setNext(curnode->clone());
		last = last->next();
	}

	if(head->next()){
		last->setNext(this->chopNext());
		this->setNext(head->chopNext());
	}

	swap(*this, *head);

	return head;
}

std::uint32_t node::as_uint32() const noexcept{
	if(!this->get_value()){
		return 0;
	}
	return std::uint32_t(strtoul(this->get_value(), nullptr, 0));
}

bool node::as_bool() const noexcept{
	return strcmp(this->get_value(), "true") == 0;
}

double node::as_double() const noexcept{
	if(!this->get_value()){
		return 0;
	}
	return strtod(this->get_value(), nullptr);
}

float node::as_float() const noexcept{
	if(!this->get_value()){
		return 0;
	}
	return strtof(this->get_value(), nullptr);
}

std::int32_t node::as_int32() const noexcept{
	if(!this->get_value()){
		return 0;
	}
	return std::int32_t(strtol(this->get_value(), nullptr, 0));
}

std::int64_t node::as_int64() const noexcept{
	if(!this->get_value()){
		return 0;
	}
	return std::int64_t(strtoll(this->get_value(), nullptr, 0));
}

long double node::as_long_double() const noexcept{
#if M_OS_NAME == M_OS_NAME_ANDROID //TODO: use strtold() when it becomes available on Android
	return this->asDouble();
#else
	if(!this->get_value()){
		return 0;
	}
	return strtold(this->get_value(), nullptr);
#endif
}

std::uint64_t node::as_uint64() const noexcept{
	if(!this->get_value()){
		return 0;
	}
	return std::uint64_t(strtoull(this->get_value(), nullptr, 0));
}
