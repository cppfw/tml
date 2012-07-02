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



namespace stob{



class Node : public ting::PoolStored<Node, 4096 / (4 * sizeof(void*))>{
	ting::Ptr<const char> value; //node value
	
	ting::Ptr<Node> next; //next sibling node
	Node* prev; //previous sibling node
	
	ting::Ptr<Node> children; //pointer to the first child
	
	//constructor is private, no inheritance.
	Node(){}
public:
	
	static inline ting::Ptr<Node> New(){
		return ting::Ptr<Node>(new Node());
	}
	
	inline const char* Value()const throw(){
		return this->value.operator->();
	}
	
	inline void SetValue(ting::Ptr<const char> value)throw(){
		this->value = value;
	}
	
	//TODO: as int, as uint, as float, as double, as boolean
	
	//TODO: get children
	
	//TODO: insert after/before
	
	//TODO: pull out (remove)
	
	void Write(ting::fs::File& fi);
};



ting::Ptr<Node> Load(ting::fs::File& fi);



}//~namespace
