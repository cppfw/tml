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

// Home page: http://stob.googlecode.com

/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once


#include <string>

#include <ting/PoolStored.hpp>
#include <ting/Ptr.hpp>
#include <ting/fs/File.hpp>

#include "Exc.hpp"



namespace stob{



/**
 * @brief STOB document node.
 * This class represents a node of the STOB document object model.
 * The Node objects can be organized to a single-linked list. There are methods for managing it.
 * The Node objects can hold a list of child nodes, i.e. a single-linked list of child Node objects.
 * The Node class has overriden operators new and delete to allocate the memore for the objects from
 * a memory pool to avoid memory fragmentation.
 */
class Node{
	std::string value; //node value
	
	ting::Ptr<Node> next; //next sibling node
	
	ting::Ptr<Node> children; //pointer to the first child
	
	//constructor is private, no inheritance.
	Node(){}
	
	static void* operator new(size_t size);
		
public:
	static void operator delete(void *p)throw();
	
	/**
	 * @brief Create new node object.
     * @return An auto-pointer to a newly created Node object.
     */
	static inline ting::Ptr<Node> New(){
		return ting::Ptr<Node>(new Node());
	}
	
	/**
	 * @brief Value stored by this node.
	 * Returns the value stored by this node, i.e. string value.
     * @return A string representing this node.
     */
	inline const std::string& Value()const throw(){
		return this->value;
	}
	
	/**
	 * @brief Set value of the node.
     * @param value - string to set as a node value.
     */
	inline void SetValue(const std::string& value)throw(){
		this->value = value;
	}
	
	/**
	 * TODO:
     * @param value
     */
	inline void SetValue(ting::s32 value)throw(){
		//TODO:
	}
	
	/**
	 * TODO:
     * @param value
     */
	inline void SetValue(ting::u32 value)throw(){
		//TODO:
	}
	
	/**
	 * TODO:
     * @param value
     */
	inline void SetValue(ting::s64 value)throw(){
		//TODO:
	}
	
	/**
	 * TODO:
     * @param value
     */
	inline void SetValue(ting::u64 value)throw(){
		//TODO:
	}
	
	/**
	 * TODO:
     * @param value
     */
	inline void SetValue(float value)throw(){
		//TODO:
	}
	
	/**
	 * TODO:
     * @return 
     */
	inline void SetValue(double value)throw(){
		//TODO:
	}
	
	/**
	 * TODO:
     * @param value
     * @return 
     */
	inline vois SetValue(bool value)throw(){
		//TODO:
	}
	
	/**
	 * @brief Get node value as signed 32bit integer.
	 * Tries to parse the string as signed 32bit integer.
     * @return Result of parsing node value as signed 32bit integer.
     */
	ting::s32 AsS32()throw();
	
	/**
	 * @brief Get node value as unsigned 32bit integer.
	 * Tries to parse the string as unsigned 32bit integer.
     * @return Result of parsing node value as unsigned 32bit integer.
     */
	ting::u32 AsU32()throw();
	
	/**
	 * @brief Get node value as signed 64bit integer.
	 * Tries to parse the string as signed 64bit integer.
     * @return Result of parsing node value as signed 64bit integer.
     */
	ting::s64 AsS64()throw();
	
	/**
	 * @brief Get node value as unsigned 64bit integer.
	 * Tries to parse the string as unsigned 64bit integer.
     * @return Result of parsing node value as unsigned 64bit integer.
     */
	ting::u64 AsU64()throw();
	
	/**
	 * @brief Get node value as float value (32bits).
	 * Tries to parse the string as float value (32bits).
     * @return Result of parsing node value as float value (32bits).
     */
	float AsFloat()throw();
	
	/**
	 * @brief Get node value as double precision float value (64bits).
	 * Tries to parse the string as double precision float value (64bits).
     * @return Result of parsing node value as double precision float value (64bits).
     */
	double AsDouble()throw();
	
	/**
	 * @brief Get node value as boolean value.
	 * Tries to parse the string as boolean value. That means if string is "true"
	 * then the returned value will be true. In all other cases it will return false.
     * @return true if string is "true".
	 * @return false otherwise.
     */
	inline bool AsBool()throw(){
		return this->Value() == "true";
	}
	
	/**
	 * @brief Set children list for this node.
	 * Sets the children nodes list for this node. Previously set list will be discarded if any.
     * @param first - auto-pointer to the first node of the children single.linked list.
     */
	void SetChildren(const ting::Ptr<Node>& first)throw(){
		this->children = first;
	}
	
	/**
	 * @brief Remove children list from the node.
	 * Removes the list of children from this node.
     * @return auto-pointer to the first node in the children list.
     */
	inline ting::Ptr<Node> RemoveChildren()throw(){
		return this->children;
	}
	
	/**
	 * @brief Remove first child from the list of children.
     * @return auto-pointer to the node which was the first child.
     */
	inline ting::Ptr<Node> RemoveFirstChild()throw(){
		ting::Ptr<Node> ret = this->children;
		if(ret.IsValid()){
			this->children = ret->next;
		}
		return ret;
	}
	
	/**
	 * @brief Get list of child nodes.
     * @return pointer to the first child node.
     */
	inline Node* Children()throw(){
		return this->children.operator->();
	}
	
	/**
	 * @brief Get constant list of child nodes.
     * @return constant pointer to the first child node.
     */
	inline const Node* Children()const throw(){
		return this->children.operator->();
	}
	
	/**
	 * @brief Get child node holding the given value.
     * @param value - value to search for among children.
     * @return Pointer to the first found child node with the given value.
	 * @return null-pointer if no child with given value was found.
     */
	Node* Child(const std::string& value)throw();
	
	/**
	 * @brief Get constant child node holding the given value.
     * @param value - value to search for among children.
     * @return Constant pointer to the first found child node with the given value.
	 * @return null-pointer if no child with given value was found.
     */
	const Node* Child(const std::string& value)const throw();
	
	/**
	 * @brief Get next node in the single-linked list.
	 * Get next sibling node in the single-linked list of nodes.
     * @return Pointer to the next node in the single-linked list.
     */
	inline Node* Next()throw(){
		return this->next.operator->();
	}
	
	/**
	 * @brief Get constant next node in the single-linked list.
	 * Get constant next sibling node in the single-linked list of nodes.
     * @return Constant pointer tot the next node in the single-linked list.
     */
	inline const Node* Next()const throw(){
		return this->next.operator->();
	}
	
	/**
	 * @brief Get next node holding the given value.
	 * Get next closest node in the single-linked list which holds the given value.
     * @param value - value to look for.
     * @return Pointer to the next closest node in the single-linked list which holds the given value.
	 * @return null-pointer if there are no nodes with the given value found.
     */
	Node* Next(const std::string& value)throw();
	
	/**
	 * @brief Get constant next node holding the given value.
	 * Get constant next closest node in the single-linked list which holds the given value.
     * @param value - value to look for.
     * @return Constant pointer to the next closest node in the single-linked list which holds the given value.
	 * @return null-pointer if there are no nodes with the given value found.
     */
	const Node* Next(const std::string& value)const throw();
	
	/**
	 * @brief Insert node into the single-linked list.
	 * Insert the node to the single-linked list as a next node after this Node.
     * @param node - node to insert.
     */
	inline void InsertNext(const ting::Ptr<Node>& node)throw(){
		if(node.IsValid()){
			node->next = this->next;
		}
		this->next = node;
	}
	
	/**
	 * @brief Remove next node from single-linked list.
     * @return auto-pointer to the Node object which has been removed from the single-linked list.
     */
	inline ting::Ptr<Node> RemoveNext()throw(){
		ting::Ptr<Node> ret = this->next;
		if(ret.IsValid()){
			this->next = ret->next;
		}
		return ret;
	}
	
	/**
	 * @brief Chop single-linked list starting from next node.
	 * After this operation there will be no next node in this single-linked list.
     * @return auto-pointer to the first node of the single-linked list tail which has been chopped.
     */
	inline ting::Ptr<Node> ChopNext()throw(){
		return this->next;
	}
	
	/**
	 * @brief Write this document-object model to the file interface as STOB document.
     * @param fi - file interface to write to.
     * @param formatted - if true then the STOB document will be written with formatting.
	 *                    if false then no formatting will be applied.
     */
	void Write(ting::fs::File& fi, bool formatted = true);
};



/**
 * @brief Load document-object model from STOB document.
 * Load document-object model from STOB document provided by given file interface.
 * @param fi - file interface to get the STOB data from.
 * @return auto-pointer to the root node of the document-object model.
 */
ting::Ptr<Node> Load(ting::fs::File& fi);



}//~namespace
