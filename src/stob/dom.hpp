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
#include <cstring>

#include <utki/config.hpp>
#include <utki/PoolStored.hpp>
#include <utki/types.hpp>
#include <utki/Buf.hpp>
#include <utki/Unique.hpp>

#include <papki/File.hpp>

#include <unikod/utf8.hpp>

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
class Node final : public utki::Unique{
	template< class T, class... Args > friend std::unique_ptr<T> utki::makeUnique(Args&&... args);
	
	std::unique_ptr<char[]> value_var; //node value

	std::unique_ptr<Node> next_var; //next sibling node

	std::unique_ptr<Node> children; //pointer to the first child

	void setValueInternal(const utki::Buf<char> str){
		if(str.size() == 0){
			this->value_var = nullptr;
			return;
		}

		this->value_var = decltype(this->value_var)(new char[str.size() + 1]);
		memcpy(this->value_var.get(), str.begin(), str.size());
		this->value_var[str.size()] = 0;//null-terminate
	}


	static void* operator new(size_t size);

	void setValue(const char* v, size_t size){
		this->setValue(utki::Buf<char>(const_cast<char*>(v), size));
	}
public:
	Node(const Node&) = delete;
	Node& operator=(const Node&) = delete;
	
	Node(const utki::Buf<char> str){
		this->setValueInternal(str);
	}
	
	Node(){}
	
	Node(const char* value) :
			Node(utki::Buf<char>(const_cast<char*>(value), value == nullptr ? 0 : strlen(value)))
	{}
	
	class NodeNotFoundExc : stob::Exc{
	public:
		NodeNotFoundExc(const std::string& message) :
				stob::Exc(message)
		{}
	};

	class NodeHasNoChldrenExc : stob::Exc{
	public:
		NodeHasNoChldrenExc(const std::string& message) :
				stob::Exc(message)
		{}
	};

	~Node()noexcept{}

	static void operator delete(void *p)noexcept;


	/**
	 * @brief value stored by this node.
	 * Returns the value stored by this node, i.e. string value.
	 * Return value can be 0;
	 * @return A string representing this node.
	 */
	const char* value()const noexcept{
		return this->value_var.get();
	}

	/**

	 * @brief Get value length in bytes.
	 * Calculates value length in bytes excluding terminating 0 byte.
	 * @return value length in bytes.
	 */
	size_t length()const noexcept{
		if(this->value() == 0){
			return 0;
		}
		return strlen(this->value());
	}

	/**
	 * @brief Get node value as utf8 string.
	 * @return UTF-8 iterator to iterate through the string.
	 */
	unikod::Utf8Iterator asUTF8()const noexcept{
		return unikod::Utf8Iterator(this->value());
	}

	/**
	 * @brief Get node value as signed 32bit integer.
	 * Tries to parse the string as signed 32bit integer.
	 * @return Result of parsing node value as signed 32bit integer.
	 */
	std::int32_t asInt32()const noexcept{
		return std::int32_t(strtol(this->value(), 0, 0));
	}

	/**
	 * @brief Get node value as unsigned 32bit integer.
	 * Tries to parse the string as unsigned 32bit integer.
	 * @return Result of parsing node value as unsigned 32bit integer.
	 */
	std::uint32_t asUint32()const noexcept{
		return std::uint32_t(strtoul(this->value(), 0, 0));
	}

	/**
	 * @brief Get node value as signed 64bit integer.
	 * Tries to parse the string as signed 64bit integer.
	 * @return Result of parsing node value as signed 64bit integer.
	 */
	std::int64_t asInt64()const noexcept{
		return std::int64_t(strtoll(this->value(), 0 , 0));
	}

	/**
	 * @brief Get node value as unsigned 64bit integer.
	 * Tries to parse the string as unsigned 64bit integer.
	 * @return Result of parsing node value as unsigned 64bit integer.
	 */
	std::uint64_t asUint64()const noexcept{
		return std::uint64_t(strtoull(this->value(), 0 , 0));
	}

	/**
	 * @brief Get node value as float value (32bits).
	 * Tries to parse the string as float value (32bits).
	 * @return Result of parsing node value as float value (32bits).
	 */
	float asFloat()const noexcept{
		return strtof(this->value(), 0);
	}

	/**
	 * @brief Get node value as double precision float value (64bits).
	 * Tries to parse the string as double precision float value (64bits).
	 * @return Result of parsing node value as double precision float value (64bits).
	 */
	double asDouble()const noexcept{
		return strtod(this->value(), 0);
	}

	/**
	 * @brief Get node value as long double precision float value (64bits).
	 * Tries to parse the string as long double precision float value (64bits).
	 * @return Result of parsing node value as long double precision float value (64bits).
	 */
	long double asLongDouble()const noexcept{
#if M_OS_NAME == M_OS_NAME_ANDROID //TODO: use strtold() when it becomes available on Android
		return this->asDouble();
#else
		return strtold(this->value(), 0);
#endif
	}

	/**
	 * @brief Get node value as boolean value.
	 * Tries to parse the string as boolean value. That means if string is "true"
	 * then the returned value will be true. In all other cases it will return false.
	 * @return true if string is "true".
	 * @return false otherwise.
	 */
	bool asBool()const noexcept{
		return strcmp(this->value(), "true") == 0;
	}

	/**
	 * @brief Set value of the node.
	 * Set the value of the node. value is copied from passed buffer.
	 * @param v - null-terminated string to set as a node value.
	 */
	void setValue(const char* v = 0)noexcept{
		this->setValue(utki::Buf<char>(const_cast<char*>(v), v == 0 ? 0 : strlen(v)));
	}

	/**
	 * @brief Set value of the node.
	 * Set the value of the node. value is copied from passed buffer.
	 * @param str - string to set as a node value.
	 */
	void setValue(const utki::Buf<char> str){
		this->setValueInternal(str);
	}

	/**
	 * @brief Set value from signed 32 bit integer.
	 * Sets value from signed 32 bit integer. The value is converted to string
	 * and then the resulting string is set as a value of the node.
	 * @param v - signed 32 bit integer to set as a value of the node.
	 */
	void setInt32(std::int32_t v)noexcept{
		char buf[64];

		int res = snprintf(buf, sizeof(buf), "%" PRIi32, v);

		if(res < 0 || res > int(sizeof(buf))){
			this->setValue(0, 0);
		}else{
			this->setValue(buf, res);
		}
	}

	/**
	 * @brief Set value from unsigned 32 bit integer.
	 * Sets value from unsigned 32 bit integer. The value is converted to string
	 * and then the resulting string is set as a value of the node.
	 * @param v - unsigned 32 bit integer to set as a value of the node.
	 */
	void setUint32(std::uint32_t v)noexcept{
		char buf[64];

		int res = snprintf(buf, sizeof(buf), "%" PRIu32, v);

		if(res < 0 || res > int(sizeof(buf))){
			this->setValue(0, 0);
		}else{
			this->setValue(buf, res);
		}
	}

	/**
	 * @brief Set value from signed 64 bit integer.
	 * Sets value from signed 64 bit integer. The value is converted to string
	 * and then the resulting string is set as a value of the node.
	 * @param v - signed 64 bit integer to set as a value of the node.
	 */
	void setInt64(std::int64_t v)noexcept{
		char buf[64];

		int res = snprintf(buf, sizeof(buf), "%" PRIi64, v);

		if(res < 0 || res > int(sizeof(buf))){
			this->setValue(0, 0);
		}else{
			this->setValue(buf, res);
		}
	}

	/**
	 * @brief Set value from unsigned 64 bit integer.
	 * Sets value from unsigned 64 bit integer. The value is converted to string
	 * and then the resulting string is set as a value of the node.
	 * @param v - unsigned 64 bit integer to set as a value of the node.
	 */
	void setUint64(std::uint64_t v)noexcept{
		char buf[64];

		int res = snprintf(buf, sizeof(buf), "%" PRIu64, v);

		if(res < 0 || res > int(sizeof(buf))){
			this->setValue(0, 0);
		}else{
			this->setValue(buf, res);
		}
	}

	/**
	 * @brief Set value from 'float'.
	 * Sets value from 'float'. The value is converted to string
	 * and then the resulting string is set as a value of the node.
	 * @param v - 'float' to set as a value of the node.
	 */
	void setFloat(float v)noexcept{
		char buf[64];

		int res = snprintf(buf, sizeof(buf), "%.8G", double(v));

		if(res < 0 || res > int(sizeof(buf))){
			this->setValue(0, 0);
		}else{
			this->setValue(buf, res);
		}
	}

	/**
	 * @brief Set value from 'float' as hexadecimal 'float'.
	 * This should make a lose-less representation of a float number.
	 * @param v - 'float' to set as a value of the node.
	 */
	void setHexFloat(float v)noexcept{
		char buf[64];

		int res = snprintf(buf, sizeof(buf), "%.8a", double(v));

		if(res < 0 || res > int(sizeof(buf))){
			this->setValue(0, 0);
		}else{
			this->setValue(buf, res);
		}
	}

	/**
	 * @brief Set value from 'double'.
	 * Sets value from 'double'. The value is converted to string
	 * and then the resulting string is set as a value of the node.
	 * @param v - 'double' to set as a value of the node.
	 */
	void setDouble(double v)noexcept{
		char buf[64];

		int res = snprintf(buf, sizeof(buf), "%.17G", v);

		if(res < 0 || res > int(sizeof(buf))){
			this->setValue(0, 0);
		}else{
			this->setValue(buf, res);
		}
	}

	/**
	 * @brief Set value from 'double' as hexadecimal 'double'.
	 * This should make a lose-less representation of a double number.
	 * @param v - 'double' to set as a value of the node.
	 */
	void setHexDouble(double v)noexcept{
		char buf[64];

		int res = snprintf(buf, sizeof(buf), "%.17a", v);

		if(res < 0 || res > int(sizeof(buf))){
			this->setValue(0, 0);
		}else{
			this->setValue(buf, res);
		}
	}

	/**
	 * @brief Set value from 'long double'.
	 * Sets value from 'long double'. The value is converted to string
	 * and then the resulting string is set as a value of the node.
	 * @param v - 'long double' to set as a value of the node.
	 */
	void setLongDouble(long double v)noexcept{
		char buf[128];

		int res = snprintf(buf, sizeof(buf), "%.31LG", v);

		if(res < 0 || res > int(sizeof(buf))){
			this->setValue(0, 0);
		}else{
			this->setValue(buf, res);
		}
	}

	/**
	 * @brief Set value from 'long double' as hexadecimal 'long double'.
	 * This should make a lose-less representation of a long double number.
	 * @param v - 'long double' to set as a value of the node.
	 */
	void setHexLongDouble(long double v)noexcept{
		char buf[64];

		int res = snprintf(buf, sizeof(buf), "%.31La", v);

		if(res < 0 || res > int(sizeof(buf))){
			this->setValue(0, 0);
		}else{
			this->setValue(buf, res);
		}
	}

	/**
	 * @brief Set value from 'bool'.
	 * Sets value from 'bool'. The value is converted to string
	 * and then the resulting string is set as a value of the node.
	 * @param v - 'bool' to set as a value of the node.
	 */
	void setBool(bool v)noexcept{
		this->setValue(v ? "true" : "false");
	}

	/**
	 * @brief Compare value of the node to given string.
	 * @param str - string to compare the value to.
	 * @return true if value and given string are equal, including cases when
	 *         value is null-pointer and 'str' is an empty string and vice versa.
	 * @return false otherwise.
	 */
	bool operator==(const char* str)const noexcept{
		if(this->value()){
			if(str){
				return strcmp(this->value(), str) == 0;
			}
			return strlen(this->value()) == 0;
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
	bool operator==(const Node& n)const noexcept;

	/**
	 * @brief Count number of children.
	 * @return Number of children.
	 */
	size_t count()const noexcept;

	/**
	 * @brief Set children list for this node.
	 * Sets the children nodes list for this node. Previously set list will be discarded if any.
	 * @param first - auto-pointer to the first node of the children single.linked list.
	 */
	void setChildren(std::unique_ptr<Node> first)noexcept{
		this->children = std::move(first);
	}

	/**
	 * @brief Remove children list from the node.
	 * Removes the list of children from this node.
	 * @return auto-pointer to the first node in the children list.
	 */
	std::unique_ptr<Node> removeChildren()noexcept{
		return std::move(this->children);
	}

	/**
	 * @brief Remove first child from the list of children.
	 * @return auto-pointer to the node which was the first child.
	 */
	std::unique_ptr<Node> removeFirstChild()noexcept{
		if(!this->children){
			return std::unique_ptr<Node>();
		}

		std::unique_ptr<Node> ret = std::move(this->children);
		this->children = std::move(ret->next_var);

		return std::move(ret);
	}

	/**
	 * @brief Remove child hilding given value.
	 * Removes the first child which holds given value.
	 * @param value - value to search for among children.
	 * @return auto-pointer to the removed node.
	 * @return invalid auto-pointer if there was no child with given value found.
	 */
	std::unique_ptr<Node> removeChild(const char* value)noexcept{
		NodeAndPrev f = this->child(value);

		if(f.prev()){
			return f.prev()->removeNext();
		}

		return this->removeFirstChild();
	}

	/**
	 * @brief Remove given child.
     * @param c - child node to remove.
     * @return Unique pointer to a removed child.
	 * @return nullptr if no child found.
     */
	std::unique_ptr<Node> removeChild(const stob::Node* c)noexcept;
	
	/**
	 * @brief Get list of child nodes.
	 * @return pointer to the first child node.
	 */
	Node* child()noexcept{
		return this->children.operator->();
	}

	/**
	 * @brief Get constant list of child nodes.
	 * @return constant pointer to the first child node.
	 */
	const Node* child()const noexcept{
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
		Node* node()noexcept{
			return this->curNode;
		}

		/**
		 * @brief Get constant pointer to Node.
		 * @return Constant pointer to Node.
		 */
		const Node* node()const noexcept{
			return this->curNode;
		}

		/**
		 * @brief Get pointer to previous Node.
		 * @return Pointer to previous Node.
		 */
		Node* prev()noexcept{
			return this->prevNode;
		}

		/**
		 * @brief Get constant pointer to previous Node.
		 * @return Constant pointer to previous Node.
		 */
		const Node* prev()const noexcept{
			return this->prevNode;
		}
	};

	/**
	 * @brief Get child node holding the given value.
	 * @param value - value to search for among children.
	 * @return instance of NodeAndPrev structure holding information about found Node.
	 */
	NodeAndPrev child(const char* value)noexcept;

	/**
	 * @brief Get constant child node holding the given value.
	 * @param value - value to search for among children.
	 * @return constant instance of NodeAndPrev structure holding information about found Node.
	 */
	const NodeAndPrev child(const char* value)const noexcept{
		return const_cast<utki::remove_constptr<decltype(this)>::type*>(this)->child(value);
	}

	/**
	 * @brief Get child node by index.
     * @param index - index of the child node to get.
     * @return instance of NodeAndPrev structure holding information about found Node.
     */
	NodeAndPrev child(size_t index)noexcept;
	
	/**
	 * @brief Get constant child node by index.
     * @param index - index of the child node to get.
     * @return constant instance of NodeAndPrev structure holding information about found Node.
     */
	const NodeAndPrev child(size_t index)const noexcept{
		return const_cast<utki::remove_constptr<decltype(this)>::type*>(this)->child(index);
	}
	
	/**
	 * @brief Get fist child.
	 * @return reference to the first child node.
	 * @throw NodeHasNoChldrenExc - in case the node has no children at all.
	 */
	Node& up(){
		auto r = this->child();
		if(!r){
			throw NodeHasNoChldrenExc(this->value());
		}
		return *r;
	}

	/**
	 * @brief Const version of get().
	 * @return Const reference to the first child node.
	 * @throw NodeHasNoChldrenExc - in case the node has no children at all.
	 */
	const Node& up()const{
		return const_cast<utki::remove_constptr<decltype(this)>::type*>(this)->up();
	}

	/**
	 * @brief Get child node with given value.
	 * In contrast to Child(value) method this one returns reference and throws exception if node is not found.
	 * @param value - value to looks for amongst children.
	 * @return reference to the found node.
	 * @throw NodeNotFoundExc - in case node with given value is not found.
	 */
	Node& up(const char* value){
		auto r = this->child(value).node();
		if(!r){
			throw NodeNotFoundExc(value);
		}
		return *r;
	}

	/**
	 * @brief Const version of get().
	 * @param value - value to looks for amongst children.
	 * @return const reference to the found node.
	 * @throw NodeNotFoundExc - in case node with given value is not found.
	 */
	const Node& up(const char* value)const{
		return const_cast<utki::remove_constptr<decltype(this)>::type*>(this)->up(value);
	}

	/**
	 * @brief Get node with the given value from the chain.
	 * @param value - value to look for.
	 * @return Reference to the found.
	 * @throw NodeNotFoundExc - in case node with given value is not found.
	 */
	Node& side(const char* value){
		auto r = this->thisOrNext(value).node();
		if(!r){
			throw NodeNotFoundExc(value);
		}
		return *r;
	}

	/**
	 * @brief Const version of side().
	 * @param value - value to look for.
	 * @return Reference to the found.
	 * @throw NodeNotFoundExc - in case node with given value is not found.
	 */
	const Node& side(const char* value)const{
		return const_cast<utki::remove_constptr<decltype(this)>::type*>(this)->side(value);
	}

	/**
	 * @brief Get first non-property child.
	 * @return instance of NodeAndPrev structure holding information about found Node.
	 */
	NodeAndPrev childNonProperty()noexcept;

	/**
	 * @brief Get constant first non-property child.
	 * @return constant instance of NodeAndPrev structure holding information about found Node.
	 */
	const NodeAndPrev childNonProperty()const noexcept{
		return const_cast<utki::remove_constptr<decltype(this)>::type*>(this)->childNonProperty();
	}

	/**
	 * @brief Get first property child.
	 * @return instance of NodeAndPrev class holding information about found Node.
	 */
	NodeAndPrev childProperty()noexcept;

	/**
	 * @brief Get constant first property child.
	 * @return constant instance of NodeAndPrev class holding information about found Node.
	 */
	const NodeAndPrev childProperty()const noexcept{
		return const_cast<utki::remove_constptr<decltype(this)>::type*>(this)->childProperty();
	}

	/**
	 * @brief Get next node in the single-linked list.
	 * Get next sibling node in the single-linked list of nodes.
	 * @return pointer to the next node in the single-linked list.
	 */
	Node* next()noexcept{
		return this->next_var.operator->();
	}

	/**
	 * @brief Get constant next node in the single-linked list.
	 * Get constant next sibling node in the single-linked list of nodes.
	 * @return constant pointer tot the next node in the single-linked list.
	 */
	const Node* next()const noexcept{
		return this->next_var.operator->();
	}

	/**
	 * @brief Get next node holding the given value.
	 * Get next closest node in the single-linked list which holds the given value.
	 * @param value - value to look for.
	 * @return instance of NodeAndPrev class holding information about found node, previous node is always valid.
	 */
	NodeAndPrev next(const char* value)noexcept;

	/**
	 * @brief Get constant next node holding the given value.
	 * Get constant next closest node in the single-linked list which holds the given value.
	 * @param value - value to look for.
	 * @return instance of NodeAndPrev class holding information about found node, previous node is always valid.
	 */
	const NodeAndPrev next(const char* value)const noexcept{
		return const_cast<utki::remove_constptr<decltype(this)>::type*>(this)->next(value);
	}

	/**
	 * @brief Get node with given value in the chain of nodes.
	 * Get closest node in the single-linked list which holds the given value. This node is included in the search.
	 * @param value - value to look for.
	 * @return instance of NodeAndPrev structure holding information about found Node.
	 */
	NodeAndPrev thisOrNext(const char* value)noexcept{
		if(this->operator==(value)){
			return NodeAndPrev(0, this);
		}

		return this->next(value);
	}

	/**
	 * @brief Get constant node with given value in the chain of nodes.
	 * Get closest constant node in the single-linked list which holds the given value. This node is included in the search.
	 * @param value - value to look for.
	 * @return instance of NodeAndPrev structure holding information about found Node.
	 */
	const NodeAndPrev thisOrNext(const char* value)const noexcept{
		return const_cast<utki::remove_constptr<decltype(this)>::type*>(this)->thisOrNext(value);
	}


	/**
	 * @brief Get child of given node in the chain of nodes.
	 * @param value - value to look for in the chain of nodes.
	 * @return First child of the node with given value in the chain of nodes.
	 */
	Node* childOfThisOrNext(const char* value)noexcept{
		if(auto c = this->thisOrNext(value).node()){
			return c->child();
		}
		return nullptr;
	}

	/**
	 * @brief Get constant child of given node in the chain of nodes.
	 * @param value - value to look for in the chain of nodes.
	 * @return Constant first child of the node with given value in the chain of nodes.
	 */
	const Node* childOfThisOrNext(const char* value)const noexcept{
		return const_cast<utki::remove_constptr<decltype(this)>::type*>(this)->childOfThisOrNext(value);
	}

	/**
	 * @brief Get next non-property node.
	 * @return instance of NodeAndPrev class holding information about found node, previous node is always valid.
	 */
	NodeAndPrev nextNonProperty()noexcept;

	/**
	 * @brief Get constant next non-property node.
	 * @return constant instance of NodeAndPrev class holding information about found node, previous node is always valid.
	 */
	const NodeAndPrev nextNonProperty()const noexcept{
		return const_cast<utki::remove_constptr<decltype(this)>::type*>(this)->nextNonProperty();
	}

	/**
	 * @brief Get closest non-property node.
	 * This node is included in the search.
	 * @return instance of NodeAndPrev class holding information about found node.
	 */
	NodeAndPrev thisOrNextNonProperty()noexcept{
		if(!this->isProperty()){
			return NodeAndPrev(0, this);
		}

		return this->nextNonProperty();
	}

	/**
	 * @brief Get closest constant non-property node.
	 * This node is included in the search.
	 * @return instance of NodeAndPrev class holding information about found node.
	 */
	const NodeAndPrev thisOrNextNonProperty()const noexcept{
		return const_cast<utki::remove_constptr<decltype(this)>::type*>(this)->thisOrNextNonProperty();
	}

	/**
	 * @brief Get next property child.
	 * @return instance of NodeAndPrev class holding information about found node, previous node is always valid.
	 */
	NodeAndPrev nextProperty()noexcept;

	/**
	 * @brief Get constant next property child.
	 * @return constant instance of NodeAndPrev class holding information about found node, previous node is always valid.
	 */
	const NodeAndPrev nextProperty()const noexcept{
		return const_cast<utki::remove_constptr<decltype(this)>::type*>(this)->nextProperty();
	}

	/**
	 * @brief Get closest property node.
	 * This node is included in the search.
	 * @return instance of NodeAndPrev class holding information about found node.
	 */
	NodeAndPrev thisOrNextProperty()noexcept{
		if(this->isProperty()){
			return NodeAndPrev(0, this);
		}

		return this->nextProperty();
	}

	/**
	 * @brief Get closest constant property node.
	 * This node is included in the search.
	 * @return instance of NodeAndPrev class holding information about found node.
	 */
	const NodeAndPrev thisOrNextProperty()const noexcept{
		return const_cast<utki::remove_constptr<decltype(this)>::type*>(this)->thisOrNextProperty();
	}

	/**
	 * @brief Get property.
	 * This is a convenience method which searches for the first child node with
	 * specified name (property name) and returns its first child node.
	 * @param propName - name of the property to get.
	 * @return pointer to a node representing property value.
	 * @return zero pointer if no property with a given name found or property has no value.
	 */
	Node* getProperty(const char* propName)noexcept{
		Node* prop = this->child(propName).node();
		if(!prop){
			return 0;
		}

		return prop->child();
	}

	/**
	 * @brief Get constant property.
	 * This is a convenience method which searches for the first child node with
	 * specified name (property name) and returns its first child node.
	 * @param propName - name of the property to get.
	 * @return constant pointer to a node representing property value.
	 * @return zero pointer if no property with a given name found or property has no value.
	 */
	const Node* getProperty(const char* propName)const noexcept{
		return const_cast<utki::remove_constptr<decltype(this)>::type*>(this)->getProperty(propName);
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
	Node* addProperty(const char* propName);

	/**
	 * @brief Insert a node as a first child.
     * @param node - node to insert.
     */
	void addAsFirstChild(std::unique_ptr<Node> node)noexcept{
		node->setNext(this->removeChildren());
		this->children = std::move(node);
	}
	
	/**
	 * @brief Insert a new node as a first child.
	 * Make a new node with given value and insert it as a first child.
     * @param value - value of the new node.
     */
	void addAsFirstChild(const char* value){
		this->addAsFirstChild(utki::makeUnique<Node>(value));
	}
	
	/**
	 * @brief Insert node into the single-linked list.
	 * Insert the node to the single-linked list as a next node after this Node.
	 * @param node - node to insert.
	 */
	void insertNext(std::unique_ptr<Node> node)noexcept{
		if(node){
			node->next_var = std::move(this->next_var);
		}
		this->next_var = std::move(node);
	}

	/**
	 * @brief Remove next node from single-linked list.
	 * @return auto-pointer to the Node object which has been removed from the single-linked list.
	 */
	std::unique_ptr<Node> removeNext()noexcept{
		std::unique_ptr<Node> ret = std::move(this->next_var);
		if(ret){
			this->next_var = std::move(ret->next_var);
		}
		return ret;
	}

	/**
	 * @brief Chop single-linked list starting from next node.
	 * After this operation there will be no next node in this single-linked list.
	 * @return auto-pointer to the first node of the single-linked list tail which has been chopped.
	 */
	std::unique_ptr<Node> chopNext()noexcept{
		return std::move(this->next_var);
	}

	/**
	 * @brief Set tail of the single-linked list.
	 * Sets the next node for this node to the specified node.
	 * @param node - node to set as the next node.
	 */
	void setNext(std::unique_ptr<Node> node)noexcept{
		this->next_var = std::move(node);
	}

	/**
	 * @brief Create a deep copy of the Node.
	 * Clones this node and all the underlying nodes hierarchy.
	 * @return A deep copy of this Node.
	 */
	std::unique_ptr<Node> clone()const;


	/**
	 * @brief Create a deep copy of the Node chain.
	 * Clones this node with all its children hierarchy and chained next Nodes.
	 * @return a deep copy of this Node chain.
	 */
	std::unique_ptr<Node> cloneChain()const;

	/**
	 * @brief Check if the Node is a property.
	 * This is just a convenience method.
	 * Checks if the first character of the value is one of the capital Latin alphabet letters from A to Z. 
	 * @return false if the first character of the node's value is a capital letter of Latin alphabet.
	 * @return true otherwise.
	 */
	bool isProperty()const noexcept{
		return this->value() == 0 || this->value()[0] < 'A' || 'Z' < this->value()[0];
	}

	/**
	 * @brief Write this document-object model to the file interface as STOB document.
	 * @param fi - file interface to write to.
	 * @param formatted - if true then the STOB document will be written with formatting.
	 *                    if false then no formatting will be applied.
	 */
	void writeChain(papki::File& fi, bool formatted = true)const;

	/**
	 * @brief Convert Node's chain to string.
	 * @param formatted - should a formatting be applied for better human readability.
	 * @return STOB as string.
	 */
	std::string chainToString(bool formatted = false)const;
};



/**
 * @brief Load document-object model from STOB document.
 * Load document-object model from STOB document provided by given file interface.
 * @param fi - file interface to get the STOB data from.
 * @return auto-pointer to the first node in the chain of the document-object model.
 */
std::unique_ptr<Node> load(const papki::File& fi);



/**
 * @brief Create STOB document-object model from string.
 * @param str - null-terminated string describing STOB document.
 * @return auto-pointer to the first node in the chain of the document-object model.
 */
std::unique_ptr<Node> parse(const char *str);



}//~namespace
