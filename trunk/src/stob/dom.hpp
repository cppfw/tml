/* The MIT License:

Copyright (c) 2012-2014 Ivan Gagis

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


#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cinttypes>
#include <utility>
#include <memory>

#include <ting/config.hpp>
#include <ting/PoolStored.hpp>
#include <ting/fs/File.hpp>
#include <ting/utf8.hpp>
#include <ting/ArrayAdaptor.hpp>

#include "Exc.hpp"



namespace stob{



/**
 * @brief STOB document node.
 * This class represents a node of the STOB document object model.
 * The Node objects can be organized to a single-linked list. There are methods for managing it.
 * The Node objects can hold a list of child nodes, i.e. a single-linked list of child Node objects.
 * The Node class has overridden operators new and delete to allocate the memory for the objects from
 * a memory pool to avoid memory fragmentation.
 */
class Node{
	char* value; //node value

	std::unique_ptr<Node> next; //next sibling node

	std::unique_ptr<Node> children; //pointer to the first child

	void SetValueInternal(const ting::ArrayAdaptor<char> str){
		if(str.size() == 0){
			this->value = 0;
			return;
		}

		this->value = new char[str.size() + 1];
		memcpy(this->value, str.begin(), str.size());
		this->value[str.size()] = 0;//null-terminate
	}
	
	//constructor is private, no inheritance.
	Node(const ting::ArrayAdaptor<char> str){
		this->SetValueInternal(str);
	}

	//no copying
	Node(const Node&);
	Node& operator=(const Node&);

	static void* operator new(size_t size);

	void SetValue(const char* v, size_t size){
		this->SetValue(ting::ArrayAdaptor<char>(const_cast<char*>(v), size));
	}
public:
	~Node()NOEXCEPT{
		delete[] this->value;
	}

	static void operator delete(void *p)NOEXCEPT;

	/**
	 * @brief Create new node object.
	 * @param str - buffer holding the value to set for the created node.
	 * @return An auto-pointer to a newly created Node object.
	 */
	static std::unique_ptr<Node> New(const ting::ArrayAdaptor<char> str){
		return std::move(std::unique_ptr<Node>(new Node(str)));
	}

	/**
	 * @brief Create new node object.
	 * @param value - null-terminated string holding the value to set for the created node.
	 * @return An auto-pointer to a newly created Node object.
	 */
	static std::unique_ptr<Node> New(const char* value){
		if(value == 0){
			return Node::New();
		}
		return Node::New(ting::ArrayAdaptor<char>(const_cast<char*>(value), strlen(value)));
	}

	/**
	 * @brief Create new node object.
	 * The value is set to empty string.
	 * @return An auto-pointer to a newly created Node object.
	 */
	static std::unique_ptr<Node> New(){
		return Node::New(ting::ArrayAdaptor<char>(0, 0));
	}

	/**
	 * @brief Value stored by this node.
	 * Returns the value stored by this node, i.e. string value.
	 * Return value can be 0;
	 * @return A string representing this node.
	 */
	const char* Value()const NOEXCEPT{
		return this->value;
	}

	/**

	 * @brief Get value length in bytes.
	 * Calculates value length in bytes excluding terminating 0 byte.
	 * @return Value length in bytes.
	 */
	size_t ValueLength()const NOEXCEPT{
		if(this->Value() == 0){
			return 0;
		}
		return strlen(this->Value());
	}

	/**
	 * @brief Get node value as utf8 string.
	 * @return UTF-8 iterator to iterate through the string.
	 */
	ting::utf8::Iterator AsUTF8()const NOEXCEPT{
		return ting::utf8::Iterator(this->Value());
	}

	/**
	 * @brief Get node value as signed 32bit integer.
	 * Tries to parse the string as signed 32bit integer.
	 * @return Result of parsing node value as signed 32bit integer.
	 */
	std::int32_t AsS32()const NOEXCEPT{
		return std::int32_t(strtol(this->Value(), 0, 0));
	}

	/**
	 * @brief Get node value as unsigned 32bit integer.
	 * Tries to parse the string as unsigned 32bit integer.
	 * @return Result of parsing node value as unsigned 32bit integer.
	 */
	std::uint32_t AsU32()const NOEXCEPT{
		return std::uint32_t(strtoul(this->Value(), 0, 0));
	}

	/**
	 * @brief Get node value as signed 64bit integer.
	 * Tries to parse the string as signed 64bit integer.
	 * @return Result of parsing node value as signed 64bit integer.
	 */
	std::int64_t AsS64()const NOEXCEPT{
		return std::int64_t(strtoll(this->Value(), 0 , 0));
	}

	/**
	 * @brief Get node value as unsigned 64bit integer.
	 * Tries to parse the string as unsigned 64bit integer.
	 * @return Result of parsing node value as unsigned 64bit integer.
	 */
	std::uint64_t AsU64()const NOEXCEPT{
		return std::uint64_t(strtoull(this->Value(), 0 , 0));
	}

	/**
	 * @brief Get node value as float value (32bits).
	 * Tries to parse the string as float value (32bits).
	 * @return Result of parsing node value as float value (32bits).
	 */
	float AsFloat()const NOEXCEPT{
		return float(this->AsDouble());
	}

	/**
	 * @brief Get node value as double precision float value (64bits).
	 * Tries to parse the string as double precision float value (64bits).
	 * @return Result of parsing node value as double precision float value (64bits).
	 */
	double AsDouble()const NOEXCEPT{
		return strtod(this->Value(), 0);
	}

	/**
	 * @brief Get node value as long double precision float value (64bits).
	 * Tries to parse the string as long double precision float value (64bits).
	 * @return Result of parsing node value as long double precision float value (64bits).
	 */
	long double AsLongDouble()const NOEXCEPT{
#if _XOPEN_SOURCE >= 600 || _ISOC99_SOURCE || _POSIX_C_SOURCE >= 200112L
		return strtold(this->Value(), 0);
#else//strtold() not supported
		return (long double)(this->AsDouble());
#endif
	}

	/**
	 * @brief Get node value as boolean value.
	 * Tries to parse the string as boolean value. That means if string is "true"
	 * then the returned value will be true. In all other cases it will return false.
	 * @return true if string is "true".
	 * @return false otherwise.
	 */
	bool AsBool()const NOEXCEPT{
		return strcmp(this->Value(), "true") == 0;
	}

	/**
	 * @brief Set value of the node.
	 * Set the value of the node. Value is copied from passed buffer.
	 * @param v - null-terminated string to set as a node value.
	 */
	void SetValue(const char* v = 0)NOEXCEPT{
		this->SetValue(ting::ArrayAdaptor<char>(const_cast<char*>(v), v == 0 ? 0 : strlen(v)));
	}

	/**
	 * @brief Set value of the node.
	 * Set the value of the node. Value is copied from passed buffer.
	 * @param str - string to set as a node value.
	 */
	void SetValue(const ting::ArrayAdaptor<char> str){
		delete[] this->value;

		this->SetValueInternal(str);
	}

	/**
	 * @brief Set value from signed 32 bit integer.
	 * Sets value from signed 32 bit integer. The value is converted to string
	 * and then the resulting string is set as a value of the node.
	 * @param v - signed 32 bit integer to set as a value of the node.
	 */
	void SetS32(std::int32_t v)NOEXCEPT{
		char buf[64];

		int res = snprintf(buf, sizeof(buf), "%" PRIi32, v);

		if(res < 0 || res > int(sizeof(buf))){
			this->SetValue(0, 0);
		}else{
			this->SetValue(buf, res);
		}
	}

	/**
	 * @brief Set value from unsigned 32 bit integer.
	 * Sets value from unsigned 32 bit integer. The value is converted to string
	 * and then the resulting string is set as a value of the node.
	 * @param v - unsigned 32 bit integer to set as a value of the node.
	 */
	void SetU32(std::uint32_t v)NOEXCEPT{
		char buf[64];

		int res = snprintf(buf, sizeof(buf), "%" PRIu32, v);

		if(res < 0 || res > int(sizeof(buf))){
			this->SetValue(0, 0);
		}else{
			this->SetValue(buf, res);
		}
	}

	/**
	 * @brief Set value from signed 64 bit integer.
	 * Sets value from signed 64 bit integer. The value is converted to string
	 * and then the resulting string is set as a value of the node.
	 * @param v - signed 64 bit integer to set as a value of the node.
	 */
	void SetS64(std::int64_t v)NOEXCEPT{
		char buf[64];

		int res = snprintf(buf, sizeof(buf), "%" PRIi64, v);

		if(res < 0 || res > int(sizeof(buf))){
			this->SetValue(0, 0);
		}else{
			this->SetValue(buf, res);
		}
	}

	/**
	 * @brief Set value from unsigned 64 bit integer.
	 * Sets value from unsigned 64 bit integer. The value is converted to string
	 * and then the resulting string is set as a value of the node.
	 * @param v - unsigned 64 bit integer to set as a value of the node.
	 */
	void SetU64(std::uint64_t v)NOEXCEPT{
		char buf[64];

		int res = snprintf(buf, sizeof(buf), "%" PRIu64, v);

		if(res < 0 || res > int(sizeof(buf))){
			this->SetValue(0, 0);
		}else{
			this->SetValue(buf, res);
		}
	}

	/**
	 * @brief Set value from 'float'.
	 * Sets value from 'float'. The value is converted to string
	 * and then the resulting string is set as a value of the node.
	 * @param v - 'float' to set as a value of the node.
	 */
	void SetFloat(float v)NOEXCEPT{
		char buf[64];

		int res = snprintf(buf, sizeof(buf), "%.8G", double(v));

		if(res < 0 || res > int(sizeof(buf))){
			this->SetValue(0, 0);
		}else{
			this->SetValue(buf, res);
		}
	}

	/**
	 * @brief Set value from 'double'.
	 * Sets value from 'double'. The value is converted to string
	 * and then the resulting string is set as a value of the node.
	 * @param v - 'double' to set as a value of the node.
	 */
	void SetDouble(double v)NOEXCEPT{
		char buf[64];

		int res = snprintf(buf, sizeof(buf), "%.17G", v);

		if(res < 0 || res > int(sizeof(buf))){
			this->SetValue(0, 0);
		}else{
			this->SetValue(buf, res);
		}
	}

	/**
	 * @brief Set value from 'long double'.
	 * Sets value from 'long double'. The value is converted to string
	 * and then the resulting string is set as a value of the node.
	 * @param v - 'long double' to set as a value of the node.
	 */
	void SetLongDouble(long double v)NOEXCEPT{
		char buf[128];

		int res = snprintf(buf, sizeof(buf), "%.31LG", v);

		if(res < 0 || res > int(sizeof(buf))){
			this->SetValue(0, 0);
		}else{
			this->SetValue(buf, res);
		}
	}

	/**
	 * @brief Set value from 'bool'.
	 * Sets value from 'bool'. The value is converted to string
	 * and then the resulting string is set as a value of the node.
	 * @param v - 'bool' to set as a value of the node.
	 */
	void SetBool(bool v)NOEXCEPT{
		this->SetValue(v ? "true" : "false");
	}

	/**
	 * @brief Compare value of the node to given string.
     * @param str - string to compare the value to.
     * @return true if value and given string are equal, including cases when
	 *         value is null-pointer and 'str' is an empty string and vice versa.
	 * @return false otherwise.
     */
	bool operator==(const char* str)const NOEXCEPT{
		if(this->Value()){
			if(str){
				return strcmp(this->Value(), str) == 0;
			}
			return strlen(this->Value()) == 0;
		}
		if(str){
			return strlen(str) == 0;
		}
		return true;
	}

	/**
	 * @brief Deep compare of this node to another node.
	 * Performs deep comparison of a STOB tree represented by this node to
	 * another STOB tree represented by given node.
     * @param n - node to compare this node to.
     * @return true if two STOB trees are completely equal.
	 * @return false otherwise.
     */
	bool operator==(const Node& n)const NOEXCEPT;
	
	/**
	 * @brief Set children list for this node.
	 * Sets the children nodes list for this node. Previously set list will be discarded if any.
	 * @param first - auto-pointer to the first node of the children single.linked list.
	 */
	void SetChildren(std::unique_ptr<Node> first)NOEXCEPT{
		this->children = std::move(first);
	}

	/**
	 * @brief Remove children list from the node.
	 * Removes the list of children from this node.
	 * @return auto-pointer to the first node in the children list.
	 */
	std::unique_ptr<Node> RemoveChildren()NOEXCEPT{
		return std::move(this->children);
	}

	/**
	 * @brief Remove first child from the list of children.
	 * @return auto-pointer to the node which was the first child.
	 */
	std::unique_ptr<Node> RemoveFirstChild()NOEXCEPT{
		if(!this->children){
			return std::unique_ptr<Node>();
		}

		std::unique_ptr<Node> ret = std::move(this->children);
		this->children = std::move(ret->next);

		return std::move(ret);
	}

	/**
	 * @brief Remove child hilding given value.
	 * Removes the first child which holds given value.
	 * @param value - value to search for among children.
	 * @return auto-pointer to the removed node.
	 * @return invalid auto-pointer if there was no child with given value found.
	 */
	std::unique_ptr<Node> RemoveChild(const char* value)NOEXCEPT{
		NodeAndPrev f = this->Child(value);

		if(f.prev()){
			return f.prev()->RemoveNext();
		}

		return this->RemoveFirstChild();
	}

	/**
	 * @brief Get list of child nodes.
	 * @return pointer to the first child node.
	 */
	Node* Child()NOEXCEPT{
		return this->children.operator->();
	}

	/**
	 * @brief Get constant list of child nodes.
	 * @return constant pointer to the first child node.
	 */
	const Node* Child()const NOEXCEPT{
		return this->children.operator->();
	}

	/**
	 * @brief Node and its previous node.
	 * Class holding a pointer to a Node and pointer to its previous Node in
	 * the single linked list.
	 * If 'node' is not 0 and 'prev' is not 0, then prev()->Next() is same as 'node'.
	 * If 'prev' is 0 and 'node' is not 0, then 'node' points to the very first node in the single-linked list.
	 * If 'node' is 0 and 'prev' is not 0, then 'prev' points to the last node in the single-linked list.
	 */
	class NodeAndPrev{
		friend class stob::Node;
		
		Node* prevNode;
		Node* curNode;
		
		NodeAndPrev(Node* prev, Node* node) :
				prevNode(prev),
				curNode(node)
		{}
	public:
		/**
		 * @brief Get pointer to Node.
         * @return Pointer to Node.
         */
		Node* node()NOEXCEPT{
			return this->curNode;
		}
		
		/**
		 * @brief Get constant pointer to Node.
         * @return Constant pointer to Node.
         */
		const Node* node()const NOEXCEPT{
			return this->curNode;
		}
		
		/**
		 * @brief Get pointer to previous Node.
         * @return Pointer to previous Node.
         */
		Node* prev()NOEXCEPT{
			return this->prevNode;
		}
		
		/**
		 * @brief Get constant pointer to previous Node.
         * @return Constant pointer to previous Node.
         */
		const Node* prev()const NOEXCEPT{
			return this->prevNode;
		}
	};
	
	/**
	 * @brief Get child node holding the given value.
	 * @param value - value to search for among children.
	 * @return instance of NodeAndPrev structure holding information about found Node.
	 */
	NodeAndPrev Child(const char* value)NOEXCEPT;

	/**
	 * @brief Get constant child node holding the given value.
	 * @param value - value to search for among children.
	 * @return constant instance of NodeAndPrev structure holding information about found Node.
	 */
	const NodeAndPrev Child(const char* value)const NOEXCEPT{
		return const_cast<Node* const>(this)->Child(value);
	}

	/**
	 * @brief Get first non-property child.
	 * @return instance of NodeAndPrev structure holding information about found Node.
	 */
	NodeAndPrev ChildNonProperty()NOEXCEPT;
	
	/**
	 * @brief Get constant first non-property child.
	 * @return constant instance of NodeAndPrev structure holding information about found Node.
	 */
	const NodeAndPrev ChildNonProperty()const NOEXCEPT{
		return const_cast<Node* const>(this)->ChildNonProperty();
	}
	
	/**
	 * @brief Get first property child.
     * @return instance of NodeAndPrev class holding information about found Node.
     */
	NodeAndPrev ChildProperty()NOEXCEPT;
	
	/**
	 * @brief Get constant first property child.
     * @return constant instance of NodeAndPrev class holding information about found Node.
     */
	const NodeAndPrev ChildProperty()const NOEXCEPT{
		return const_cast<Node* const>(this)->ChildProperty();
	}
	
	/**
	 * @brief Get next node in the single-linked list.
	 * Get next sibling node in the single-linked list of nodes.
	 * @return pointer to the next node in the single-linked list.
	 */
	Node* Next()NOEXCEPT{
		return this->next.operator->();
	}

	/**
	 * @brief Get constant next node in the single-linked list.
	 * Get constant next sibling node in the single-linked list of nodes.
	 * @return constant pointer tot the next node in the single-linked list.
	 */
	const Node* Next()const NOEXCEPT{
		return this->next.operator->();
	}

	/**
	 * @brief Get next node holding the given value.
	 * Get next closest node in the single-linked list which holds the given value.
	 * @param value - value to look for.
	 * @return instance of NodeAndPrev class holding information about found node, previous node is always valid.
	 */
	NodeAndPrev Next(const char* value)NOEXCEPT;

	/**
	 * @brief Get constant next node holding the given value.
	 * Get constant next closest node in the single-linked list which holds the given value.
	 * @param value - value to look for.
	 * @return instance of NodeAndPrev class holding information about found node, previous node is always valid.
	 */
	const NodeAndPrev Next(const char* value)const NOEXCEPT{
		return const_cast<Node* const>(this)->Next(value);
	}

	/**
	 * @brief Get node with given value in the chain of nodes.
	 * Get closest node in the single-linked list which holds the given value. This node is included in the search.
     * @param value - value to look for.
     * @return instance of NodeAndPrev structure holding information about found Node.
     */
	NodeAndPrev ThisOrNext(const char* value)NOEXCEPT{
		if(this->operator==(value)){
			return NodeAndPrev(0, this);
		}

		return this->Next(value);
	}
	
	/**
	 * @brief Get constant node with given value in the chain of nodes.
	 * Get closest constant node in the single-linked list which holds the given value. This node is included in the search.
     * @param value - value to look for.
     * @return instance of NodeAndPrev structure holding information about found Node.
     */
	const NodeAndPrev ThisOrNext(const char* value)const NOEXCEPT{
		return const_cast<Node* const>(this)->ThisOrNext(value);
	}
	
	/**
	 * @brief Get next non-property node.
     * @return instance of NodeAndPrev class holding information about found node, previous node is always valid.
     */
	NodeAndPrev NextNonProperty()NOEXCEPT;
	
	/**
	 * @brief Get constant next non-property node.
     * @return constant instance of NodeAndPrev class holding information about found node, previous node is always valid.
     */
	const NodeAndPrev NextNonProperty()const NOEXCEPT{
		return const_cast<Node* const>(this)->NextNonProperty();
	}
	
	/**
	 * @brief Get closest non-property node.
	 * This node is included in the search.
     * @return instance of NodeAndPrev class holding information about found node.
     */
	NodeAndPrev ThisOrNextNonProperty()NOEXCEPT{
		if(!this->IsProperty()){
			return NodeAndPrev(0, this);
		}
		
		return this->NextNonProperty();
	}
	
	/**
	 * @brief Get closest constant non-property node.
	 * This node is included in the search.
     * @return instance of NodeAndPrev class holding information about found node.
     */
	const NodeAndPrev ThisOrNextNonProperty()const NOEXCEPT{
		return const_cast<Node* const>(this)->ThisOrNextNonProperty();
	}
	
	/**
	 * @brief Get next property child.
     * @return instance of NodeAndPrev class holding information about found node, previous node is always valid.
     */
	NodeAndPrev NextProperty()NOEXCEPT;
	
	/**
	 * @brief Get constant next property child.
     * @return constant instance of NodeAndPrev class holding information about found node, previous node is always valid.
     */
	const NodeAndPrev NextProperty()const NOEXCEPT{
		return const_cast<Node* const>(this)->NextProperty();
	}
	
	/**
	 * @brief Get closest property node.
	 * This node is included in the search.
     * @return instance of NodeAndPrev class holding information about found node.
     */
	NodeAndPrev ThisOrNextProperty()NOEXCEPT{
		if(this->IsProperty()){
			return NodeAndPrev(0, this);
		}
		
		return this->NextProperty();
	}
	
	/**
	 * @brief Get closest constant property node.
	 * This node is included in the search.
     * @return instance of NodeAndPrev class holding information about found node.
     */
	const NodeAndPrev ThisOrNextProperty()const NOEXCEPT{
		return const_cast<Node* const>(this)->ThisOrNextProperty();
	}
	
	/**
	 * @brief Get property.
	 * This is a convenience method which searches for the first child node with
	 * specified name (property name) and returns its first child node.
	 * @param propName - name of the property to get.
	 * @return pointer to a node representing property value.
	 * @return zero pointer if no property with a given name found or property has no value.
	 */
	Node* GetProperty(const char* propName)NOEXCEPT{
		Node* prop = this->Child(propName).node();
		if(!prop){
			return 0;
		}

		return prop->Child();
	}

	/**
	 * @brief Get constant property.
	 * This is a convenience method which searches for the first child node with
	 * specified name (property name) and returns its first child node.
	 * @param propName - name of the property to get.
	 * @return constant pointer to a node representing property value.
	 * @return zero pointer if no property with a given name found or property has no value.
	 */
	const Node* GetProperty(const char* propName)const NOEXCEPT{
		return const_cast<Node* const>(this)->GetProperty(propName);
	}

	/**
	 * @brief Add new property to the node.
	 * This is a convenience method which adds a new child with a given value (property name)
	 * to this node and adds one child node (property value) to that added child. It then
	 * returns a pointer to the node representing property value.
	 * Note, that it does not check if property with a given name already exists.
	 * In case the property with a given name already exists this method will just add another
	 * property with the same name, so there will be two (or more) child nodes having that
	 * property name.
	 * The property node is added to the beginning of the children list.
	 * @param propName - name of the new property, null-terminated string.
	 * @return pointer to a node representing value of the newly created property.
	 *         The returned pointer is always valid.
	 */
	Node* AddProperty(const char* propName);

	/**
	 * @brief Insert node into the single-linked list.
	 * Insert the node to the single-linked list as a next node after this Node.
	 * @param node - node to insert.
	 */
	void InsertNext(std::unique_ptr<Node> node)NOEXCEPT{
		if(node){
			node->next = std::move(this->next);
		}
		this->next = std::move(node);
	}

	/**
	 * @brief Remove next node from single-linked list.
	 * @return auto-pointer to the Node object which has been removed from the single-linked list.
	 */
	std::unique_ptr<Node> RemoveNext()NOEXCEPT{
		std::unique_ptr<Node> ret = std::move(this->next);
		if(ret){
			this->next = std::move(ret->next);
		}
		return ret;
	}

	/**
	 * @brief Chop single-linked list starting from next node.
	 * After this operation there will be no next node in this single-linked list.
	 * @return auto-pointer to the first node of the single-linked list tail which has been chopped.
	 */
	std::unique_ptr<Node> ChopNext()NOEXCEPT{
		return std::move(this->next);
	}

	/**
	 * @brief Set tail of the single-linked list.
	 * Sets the next node for this node to the specified node.
	 * @param node - node to set as the next node.
	 */
	void SetNext(std::unique_ptr<Node> node)NOEXCEPT{
		this->next = std::move(node);
	}

	/**
	 * @brief Create a deep copy of the Node.
	 * Clones this node and all the underlying nodes hierarchy.
	 * @return A deep copy of this Node.
	 */
	std::unique_ptr<Node> Clone()const;

	/**
	 * @brief Check if the Node is a property.
	 * This is just a convenience method.
	 * Checks if the first character of the value is one of the capital Latin alphabet letters from A to Z. 
	 * @return false if the first character of the node's value is a capital letter of Latin alphabet.
	 * @return true otherwise.
	 */
	bool IsProperty()const NOEXCEPT{
		return this->Value() == 0 || this->Value()[0] < 'A' || 'Z' < this->Value()[0];
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
 * @return auto-pointer to the first node in the chain of the document-object model.
 */
std::unique_ptr<Node> Load(ting::fs::File& fi);



/**
 * @brief Create STOB document-object model from string.
 * @param str - null-terminated string describing STOB document.
 * @return auto-pointer to the first node in the chain of the document-object model.
 */
std::unique_ptr<Node> Parse(const char *str);



}//~namespace
