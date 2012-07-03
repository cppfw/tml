/* The MIT License:

Copyright (c) 2012 Ivan Gagis

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE. */

#pragma once


#include <string>

#include <ting/PoolStored.hpp>
#include <ting/Ptr.hpp>
#include <ting/fs/File.hpp>

#include "Exc.hpp"



namespace stob{



class Node{
	std::string value; //node value
	
	ting::Ptr<Node> next; //next sibling node
	
	ting::Ptr<Node> children; //pointer to the first child
	
	//constructor is private, no inheritance.
	Node(){}
	
	static void* operator new(size_t size);
		
public:
	static void operator delete(void *p)throw();
	
	static inline ting::Ptr<Node> New(){
		return ting::Ptr<Node>(new Node());
	}
	
	inline const std::string& Value()const throw(){
		return this->value;
	}
	
	inline void SetValue(const std::string& value)throw(){
		this->value = value;
	}
	
	ting::s32 AsS32()throw();
	
	ting::u32 AsU32()throw();
	
	ting::s64 AsS64()throw();
	
	ting::u64 AsU64()throw();
	
	float AsFloat()throw();
	
	double AsDouble()throw();
	
	inline bool AsBool()throw(){
		return this->Value() == "true";
	}
	
	void SetChildren(const ting::Ptr<Node>& first)throw(){
		this->children = first;
	}
	
	inline ting::Ptr<Node> RemoveChildren()throw(){
		return this->children;
	}
	
	inline ting::Ptr<Node> RemoveFirstChild()throw(){
		ting::Ptr<Node> ret = this->children;
		if(ret.IsValid()){
			this->children = ret->next;
		}
		return ret;
	}
	
	inline Node* Children()throw(){
		return this->children.operator->();
	}
	
	inline const Node* Children()const throw(){
		return this->children.operator->();
	}
	
	Node* Child(const std::string& value)throw();
	
	const Node* Child(const std::string& value)const throw();
	
	inline Node* Next()throw(){
		return this->next.operator->();
	}
	
	inline const Node* Next()const throw(){
		return this->next.operator->();
	}
	
	Node* Next(const std::string& value)throw();
	
	const Node* Next(const std::string& value)const throw();
	
	inline void InsertNext(const ting::Ptr<Node>& node)throw(){
		if(node.IsValid()){
			node->next = this->next;
		}
		this->next = node;
	}
	
	inline ting::Ptr<Node> RemoveNext()throw(){
		ting::Ptr<Node> ret = this->next;
		if(ret.IsValid()){
			this->next = ret->next;
		}
		return ret;
	}
	
	inline ting::Ptr<Node> ChopNext()throw(){
		return this->next;
	}
	
	void Write(ting::fs::File& fi, bool formatted = true);
};



ting::Ptr<Node> Load(ting::fs::File& fi);



}//~namespace
