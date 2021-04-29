#include "crawler.hpp"

using namespace treeml;


crawler crawler::in(){
	ASSERT(this->i != this->b.end())
	if(this->get().children.size() == 0){
		throw std::logic_error("crawler::in() failed, node has no children");
	}
	return crawler(this->get().children);
}

crawler& crawler::next(){
	ASSERT(this->i != this->b.end())
	++this->i;
	if(this->i == this->b.end()){
		throw std::logic_error("crawler::next() failed, reached end of node list");
	}
	return *this;
}

crawler& crawler::to(const std::string& str){
	this->i = std::find(this->i, this->b.end(), str);
	if(this->i != this->b.end()){
		return *this;
	}
	throw std::runtime_error("crawler::to() failed, reached end of node list");
}
