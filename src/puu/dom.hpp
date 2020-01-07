#pragma once


#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cinttypes>
#include <cstring>
#include <utility>
#include <memory>

#include <utki/config.hpp>
#include <utki/types.hpp>
#include <utki/Buf.hpp>
#include <utki/Unique.hpp>

#include <papki/file.hpp>

#include <unikod/utf8.hpp>

#include "exception.hpp"


namespace puu{



/**
 * @brief puu document node.
 * This class represents a node of the puu document object model.
 * The node objects can be organized to a single-linked list.
 * The node objects can hold a list of child nodes, i.e. a single-linked list of child node objects.
 */
class node final : public utki::Unique{
	template< class T, class... Args > friend std::unique_ptr<T> utki::makeUnique(Args&&... args);

	std::unique_ptr<char[]> value_v; //node value

	std::unique_ptr<node> next_v; //next sibling node

	std::unique_ptr<node> children; //pointer to the first child

	void set_value_internal(const utki::Buf<char> str);

	void set_value(const char* v, size_t size){
		this->set_value(utki::Buf<char>(const_cast<char*>(v), size));
	}
public:
	node(const node&) = delete;
	node& operator=(const node&) = delete;

	node(const utki::Buf<char> str){
		this->set_value_internal(str);
	}

	node(const std::string& str) :
			node(utki::Buf<char>(const_cast<char*>(str.c_str()), str.size()))
	{}

	node(){}

	node(const char* value) :
			node(utki::Buf<char>(const_cast<char*>(value), value == nullptr ? 0 : strlen(value)))
	{}

	class not_found_exc : puu::exception{
	public:
		not_found_exc(const std::string& message) :
				puu::exception(message)
		{}
	};

	// TODO: deprecated, remove in v1.0.0
	typedef not_found_exc NodeNotFoundExc;

	//TODO: is needed? can be replaced by not_found_exc?
	class NodeHasNoChldrenExc : puu::exception{
	public:
		NodeHasNoChldrenExc(const std::string& message) :
				puu::exception(message)
		{}
	};

	~node()noexcept{}


	/**
	 * @brief value stored by this node.
	 * Returns the value stored by this node, i.e. string value.
	 * The string is null-terminated.
	 * Return value can be nullptr;
	 * @return A string representing this node.
	 */
	const char* value()const noexcept{
		return this->value_v.get();
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
	unikod::Utf8Iterator as_utf8()const noexcept{
		return unikod::Utf8Iterator(this->value());
	}

	//TODO: deprecated
	unikod::Utf8Iterator asUTF8()const noexcept{
		return this->as_utf8();
	}

	/**
	 * @brief Get node value as std::string.
	 * @return std::string holding the copy of node value.
	 */
	std::string as_string()const noexcept{
		return std::string(this->value());
	}

	//TODO: deprecated
	std::string asString()const noexcept{
		return this->as_string();
	}

	/**
	 * @brief Get node value as UTF-32 string.
	 * Converts node value to UTF-32 as if it was in UTF-8.
	 * @return std::u32string holding the node value.
	 */
	std::u32string as_u32string()const noexcept;

	//TODO: deprecated
	std::u32string asU32String()const noexcept{
		return this->as_u32string();
	}

	/**
	 * @brief Get node value as signed 32bit integer.
	 * Tries to parse the string as signed 32bit integer.
	 * @return Result of parsing node value as signed 32bit integer.
	 */
	std::int32_t as_int32()const noexcept;

	//TODO:deprecated
	std::int32_t asInt32()const noexcept{
		return this->as_int32();
	}

	/**
	 * @brief Get node value as unsigned 32bit integer.
	 * Tries to parse the string as unsigned 32bit integer.
	 * @return Result of parsing node value as unsigned 32bit integer.
	 */
	std::uint32_t as_uint32()const noexcept;

	//TODO: deprecated
	std::uint32_t asUint32()const noexcept{
		return this->as_uint32();
	}

	/**
	 * @brief Get node value as signed 64bit integer.
	 * Tries to parse the string as signed 64bit integer.
	 * @return Result of parsing node value as signed 64bit integer.
	 */
	std::int64_t as_int64()const noexcept;

	//TODO:deprecated
	std::int64_t asInt64()const noexcept{
		return this->as_int64();
	}

	/**
	 * @brief Get node value as unsigned 64bit integer.
	 * Tries to parse the string as unsigned 64bit integer.
	 * @return Result of parsing node value as unsigned 64bit integer.
	 */
	std::uint64_t as_uint64()const noexcept;

	//TODO:deprecated
	std::uint64_t asUint64()const noexcept{
		return this->as_uint64();
	}

	/**
	 * @brief Get node value as float value (32bits).
	 * Tries to parse the string as float value (32bits).
	 * @return Result of parsing node value as float value (32bits).
	 */
	float as_float()const noexcept;

	//TODO:deprecated
	float asFloat()const noexcept{
		return this->as_float();
	}

	/**
	 * @brief Get node value as double precision float value (64bits).
	 * Tries to parse the string as double precision float value (64bits).
	 * @return Result of parsing node value as double precision float value (64bits).
	 */
	double as_double()const noexcept;

	//TODO:deprecated
	double asDouble()const noexcept{
		return this->as_double();
	}

	/**
	 * @brief Get node value as long double precision float value (64bits).
	 * Tries to parse the string as long double precision float value (64bits).
	 * @return Result of parsing node value as long double precision float value (64bits).
	 */
	long double as_long_double()const noexcept;

	//TODO:deprecated
	long double asLongDouble()const noexcept{
		return this->as_long_double();
	}

	/**
	 * @brief Get node value as boolean value.
	 * Tries to parse the string as boolean value. That means if string is "true"
	 * then the returned value will be true. In all other cases it will return false.
	 * @return true if string is "true".
	 * @return false otherwise.
	 */
	bool as_bool()const noexcept;

	//TODO: deprecated
	bool asBool()const noexcept{
		return this->as_bool();
	}

	/**
	 * @brief Set value of the node.
	 * Set the value of the node. value is copied from passed buffer.
	 * @param v - null-terminated string to set as a node value.
	 */
	void set_value(const char* v)noexcept{
		this->set_value(utki::Buf<char>(const_cast<char*>(v), v ? strlen(v) : 0));
	}

	//TODO: deprecated
	void setValue(const char* v)noexcept{
		this->set_value(v);
	}

	/**
	 * @brief Set value of the node.
	 * Set the value of the node. value is copied from passed buffer.
	 * @param str - string to set as a node value.
	 */
	void set_value(const utki::Buf<char> str){
		this->set_value_internal(str);
	}

	//TODO: deprecated
	void setValue(const utki::Buf<char> str){
		this->set_value(str);
	}

	/**
	 * @brief Set value from signed 32 bit integer.
	 * Sets value from signed 32 bit integer. The value is converted to string
	 * and then the resulting string is set as a value of the node.
	 * @param v - signed 32 bit integer to set as a value of the node.
	 */
	void set_int32(std::int32_t v)noexcept{
		char buf[64];

		int res = snprintf(buf, sizeof(buf), "%" PRIi32, v);

		if(res < 0 || res > int(sizeof(buf))){
			this->set_value(nullptr, 0);
		}else{
			this->set_value(buf, res);
		}
	}

	//TODO:deprecated
	void setInt32(std::int32_t v)noexcept{
		this->set_int32(v);
	}

	/**
	 * @brief Set value from unsigned 32 bit integer.
	 * Sets value from unsigned 32 bit integer. The value is converted to string
	 * and then the resulting string is set as a value of the node.
	 * @param v - unsigned 32 bit integer to set as a value of the node.
	 */
	void set_uint32(std::uint32_t v)noexcept{
		char buf[64];

		int res = snprintf(buf, sizeof(buf), "%" PRIu32, v);

		if(res < 0 || res > int(sizeof(buf))){
			this->set_value(nullptr, 0);
		}else{
			this->set_value(buf, res);
		}
	}

	//TODO:deprecated
	void setUint32(std::uint32_t v)noexcept{
		this->set_uint32(v);
	}

	/**
	 * @brief Set value from signed 64 bit integer.
	 * Sets value from signed 64 bit integer. The value is converted to string
	 * and then the resulting string is set as a value of the node.
	 * @param v - signed 64 bit integer to set as a value of the node.
	 */
	void set_int64(std::int64_t v)noexcept{
		char buf[64];

		int res = snprintf(buf, sizeof(buf), "%" PRIi64, v);

		if(res < 0 || res > int(sizeof(buf))){
			this->set_value(nullptr, 0);
		}else{
			this->set_value(buf, res);
		}
	}

	//TODO: deprecated
	void setInt64(std::int64_t v)noexcept{
		this->set_int64(v);
	}

	/**
	 * @brief Set value from unsigned 64 bit integer.
	 * Sets value from unsigned 64 bit integer. The value is converted to string
	 * and then the resulting string is set as a value of the node.
	 * @param v - unsigned 64 bit integer to set as a value of the node.
	 */
	void set_uint64(std::uint64_t v)noexcept{
		char buf[64];

		int res = snprintf(buf, sizeof(buf), "%" PRIu64, v);

		if(res < 0 || res > int(sizeof(buf))){
			this->set_value(nullptr, 0);
		}else{
			this->set_value(buf, res);
		}
	}

	//TODO: deprecated
	void setUint64(std::uint64_t v)noexcept{
		this->set_uint64(v);
	}

	/**
	 * @brief Set value from 'float'.
	 * Sets value from 'float'. The value is converted to string
	 * and then the resulting string is set as a value of the node.
	 * @param v - 'float' to set as a value of the node.
	 */
	void set_float(float v)noexcept{
		char buf[64];

		int res = snprintf(buf, sizeof(buf), "%.8G", double(v));

		if(res < 0 || res > int(sizeof(buf))){
			this->set_value(nullptr, 0);
		}else{
			this->set_value(buf, res);
		}
	}

	//TODO: deprecated
	void setFloat(float v)noexcept{
		this->set_float(v);
	}

	/**
	 * @brief Set value from 'float' as hexadecimal 'float'.
	 * This should make a lose-less representation of a float number.
	 * @param v - 'float' to set as a value of the node.
	 */
	void set_hex_float(float v)noexcept{
		char buf[64];

		int res = snprintf(buf, sizeof(buf), "%.8a", double(v));

		if(res < 0 || res > int(sizeof(buf))){
			this->set_value(nullptr, 0);
		}else{
			this->set_value(buf, res);
		}
	}

	//TODO:deprecated
	void setHexFloat(float v)noexcept{
		this->set_hex_float(v);
	}

	/**
	 * @brief Set value from 'double'.
	 * Sets value from 'double'. The value is converted to string
	 * and then the resulting string is set as a value of the node.
	 * @param v - 'double' to set as a value of the node.
	 */
	void set_double(double v)noexcept{
		char buf[64];

		int res = snprintf(buf, sizeof(buf), "%.17G", v);

		if(res < 0 || res > int(sizeof(buf))){
			this->set_value(nullptr, 0);
		}else{
			this->set_value(buf, res);
		}
	}

	//TODO: deprecated
	void setDouble(double v)noexcept{
		this->set_double(v);
	}

	/**
	 * @brief Set value from 'double' as hexadecimal 'double'.
	 * This should make a lose-less representation of a double number.
	 * @param v - 'double' to set as a value of the node.
	 */
	void set_hex_double(double v)noexcept{
		char buf[64];

		int res = snprintf(buf, sizeof(buf), "%.17a", v);

		if(res < 0 || res > int(sizeof(buf))){
			this->set_value(nullptr, 0);
		}else{
			this->set_value(buf, res);
		}
	}

	//TODO: deprecated
	void setHexDouble(double v)noexcept{
		this->set_hex_double(v);
	}

	/**
	 * @brief Set value from 'long double'.
	 * Sets value from 'long double'. The value is converted to string
	 * and then the resulting string is set as a value of the node.
	 * @param v - 'long double' to set as a value of the node.
	 */
	void set_long_double(long double v)noexcept{
		char buf[128];

		int res = snprintf(buf, sizeof(buf), "%.31LG", v);

		if(res < 0 || res > int(sizeof(buf))){
			this->set_value(nullptr, 0);
		}else{
			this->set_value(buf, res);
		}
	}

	//TODO: deprecated
	void setLongDouble(long double v)noexcept{
		this->set_long_double(v);
	}

	/**
	 * @brief Set value from 'long double' as hexadecimal 'long double'.
	 * This should make a lose-less representation of a long double number.
	 * @param v - 'long double' to set as a value of the node.
	 */
	void set_hex_long_double(long double v)noexcept{
		char buf[64];

		int res = snprintf(buf, sizeof(buf), "%.31La", v);

		if(res < 0 || res > int(sizeof(buf))){
			this->set_value(nullptr, 0);
		}else{
			this->set_value(buf, res);
		}
	}

	//TODO: deprecated
	void setHexLongDouble(long double v)noexcept{
		this->set_hex_long_double(v);
	}

	/**
	 * @brief Set value from 'bool'.
	 * Sets value from 'bool'. The value is converted to string
	 * and then the resulting string is set as a value of the node.
	 * @param v - 'bool' to set as a value of the node.
	 */
	void set_bool(bool v)noexcept{
		this->set_value(v ? "true" : "false");
	}

	//TODO: deprecated
	void setBool(bool v)noexcept{
		this->set_bool(v);
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
	 * Performs deep comparison of a puu tree represented by this node to
	 * another puu tree represented by given node.
	 * @param n - node to compare this node to.
	 * @return true if two puu trees are completely equal.
	 * @return false otherwise.
	 */
	bool operator==(const node& n)const noexcept;

	/**
	 * @brief Count number of children.
	 * @return Number of children.
	 */
	size_t count()const noexcept;

	/**
	 * @brief Count number of nodes in chain.
	 * @return number of nodes in chain.
	 */
	size_t count_chain()const noexcept;

	//TODO: deprecated
	size_t countChain()const noexcept{
		return this->count_chain();
	}

	/**
	 * @brief Set children list for this node.
	 * Sets the children nodes list for this node. Previously set list will be discarded if any.
	 * @param first - auto-pointer to the first node of the children single.linked list.
	 */
	void set_children(std::unique_ptr<node> first)noexcept{
		this->children = std::move(first);
	}

	//TODO: deprecated
	void setChildren(std::unique_ptr<node> first)noexcept{
		this->set_children(std::move(first));
	}

	/**
	 * @brief Remove children list from the node.
	 * Removes the list of children from this node.
	 * @return auto-pointer to the first node in the children list.
	 */
	std::unique_ptr<node> remove_children()noexcept{
		return std::move(this->children);
	}

	//TODO: deprecated
	std::unique_ptr<node> removeChildren()noexcept{
		return this->remove_children();
	}

	/**
	 * @brief Remove first child from the list of children.
	 * @return auto-pointer to the node which was the first child.
	 */
	std::unique_ptr<node> remove_first_child()noexcept{
		if(!this->children){
			return std::unique_ptr<node>();
		}

		std::unique_ptr<node> ret = std::move(this->children);
		this->children = std::move(ret->next_v);

		return ret;
	}

	//TODO: deprecated
	std::unique_ptr<node> removeFirstChild()noexcept{
		return this->remove_first_child();
	}

	/**
	 * @brief Remove child hilding given value.
	 * Removes the first child which holds given value.
	 * @param value - value to search for among children.
	 * @return auto-pointer to the removed node.
	 * @return invalid auto-pointer if there was no child with given value found.
	 */
	std::unique_ptr<node> remove_child(const char* value)noexcept{
		nodeAndPrev f = this->child(value);

		if(f.prev()){
			return f.prev()->removeNext();
		}

		return this->remove_first_child();
	}


	//TODO: deprecated
	std::unique_ptr<node> removeChild(const char* value)noexcept{
		return this->remove_child(value);
	}

	/**
	 * @brief Remove given child.
	 * @param c - child node to remove.
	 * @return Unique pointer to a removed child.
	 * @return nullptr if no child found.
	 */
	std::unique_ptr<node> remove_child(const puu::node* c)noexcept;

	//TODO: reprecated
	std::unique_ptr<node> removeChild(const puu::node* c)noexcept{
		return this->remove_child(c);
	}

	/**
	 * @brief Get list of child nodes.
	 * @return pointer to the first child node.
	 */
	node* child()noexcept{
		return this->children.operator->();
	}

	/**
	 * @brief Get constant list of child nodes.
	 * @return constant pointer to the first child node.
	 */
	const node* child()const noexcept{
		return this->children.operator->();
	}

	/**
	 * @brief Replace this node in the chain by another chain of nodes.
	 * @param chain - chain of nodes to replace by.
	 * @return replaced node.
	 */
	std::unique_ptr<node> replace(std::unique_ptr<node> chain);

	/**
	 * @brief Replace this node with the clone of the given chain of nodes.
	 * @param chain - chain of nodes to replace by.
	 * @return replaced node.
	 */
	std::unique_ptr<node> replace(const node& chain);

	/**
	 * @brief node and its previous node.
	 * Class holding a pointer to a node and pointer to its previous node in
	 * the single linked list.
	 * If 'node' is not 0 and 'prev' is not 0, then prev()->Next() is same as 'node'.
	 * If 'prev' is 0 and 'node' is not 0, then 'node' points to the very first node in the single-linked list.
	 * If 'node' is 0 and 'prev' is not 0, then 'prev' points to the last node in the single-linked list.
	 */
	class nodeAndPrev{
		friend class puu::node;

		node* prevnode;
		node* curnode;

		nodeAndPrev(node* prev, node* node) :
				prevnode(prev),
				curnode(node)
		{}
	public:
		/**
		 * @brief Get pointer to node.
		 * @return Pointer to node.
		 */
		node* get_node()noexcept{
			return this->curnode;
		}

		/**
		 * @brief Get constant pointer to node.
		 * @return Constant pointer to node.
		 */
		const node* get_node()const noexcept{
			return this->curnode;
		}

		/**
		 * @brief Get pointer to previous node.
		 * @return Pointer to previous node.
		 */
		node* prev()noexcept{
			return this->prevnode;
		}

		/**
		 * @brief Get constant pointer to previous node.
		 * @return Constant pointer to previous node.
		 */
		const node* prev()const noexcept{
			return this->prevnode;
		}
	};

	/**
	 * @brief Get child node holding the given value.
	 * @param value - value to search for among children.
	 * @return instance of nodeAndPrev structure holding information about found node.
	 */
	nodeAndPrev child(const char* value)noexcept;

	/**
	 * @brief Get constant child node holding the given value.
	 * @param value - value to search for among children.
	 * @return constant instance of nodeAndPrev structure holding information about found node.
	 */
	const nodeAndPrev child(const char* value)const noexcept{
		return const_cast<utki::remove_constptr<decltype(this)>::type*>(this)->child(value);
	}

	/**
	 * @brief Get child node by index.
	 * @param index - index of the child node to get.
	 * @return instance of nodeAndPrev structure holding information about found node.
	 */
	nodeAndPrev child(size_t index)noexcept;

	/**
	 * @brief Get constant child node by index.
	 * @param index - index of the child node to get.
	 * @return constant instance of nodeAndPrev structure holding information about found node.
	 */
	const nodeAndPrev child(size_t index)const noexcept{
		return const_cast<utki::remove_constptr<decltype(this)>::type*>(this)->child(index);
	}

	/**
	 * @brief Get fist child.
	 * @return reference to the first child node.
	 * @throw nodeHasNoChldrenExc - in case the node has no children at all.
	 */
	node& up(){
		auto r = this->child();
		if(!r){
			throw NodeHasNoChldrenExc(this->value());
		}
		return *r;
	}

	/**
	 * @brief Const version of get().
	 * @return Const reference to the first child node.
	 * @throw nodeHasNoChldrenExc - in case the node has no children at all.
	 */
	const node& up()const{
		return const_cast<utki::remove_constptr<decltype(this)>::type*>(this)->up();
	}

	/**
	 * @brief Get child node with given value.
	 * In contrast to child(value) method this one returns reference and throws exception if node is not found.
	 * @param value - value to looks for amongst children.
	 * @return reference to the found node.
	 * @throw not_found_exc - in case node with given value is not found.
	 */
	node& up(const char* value){
		auto r = this->child(value).get_node();
		if(!r){
			throw not_found_exc(value);
		}
		return *r;
	}

	/**
	 * @brief Const version of get().
	 * @param value - value to looks for amongst children.
	 * @return const reference to the found node.
	 * @throw not_found_exc - in case node with given value is not found.
	 */
	const node& up(const char* value)const{
		return const_cast<utki::remove_constptr<decltype(this)>::type*>(this)->up(value);
	}

	/**
	 * @brief Get node with the given value from the chain.
	 * @param value - value to look for.
	 * @return Reference to the found.
	 * @throw not_found_exc - in case node with given value is not found.
	 */
	node& side(const char* value){
		auto r = this->thisOrNext(value).get_node();
		if(!r){
			throw not_found_exc(value);
		}
		return *r;
	}

	/**
	 * @brief Const version of side().
	 * @param value - value to look for.
	 * @return Reference to the found.
	 * @throw not_found_exc - in case node with given value is not found.
	 */
	const node& side(const char* value)const{
		return const_cast<utki::remove_constptr<decltype(this)>::type*>(this)->side(value);
	}

	/**
	 * @brief Get first non-property child.
	 * @return instance of nodeAndPrev structure holding information about found node.
	 */
	nodeAndPrev childNonProperty()noexcept;

	/**
	 * @brief Get constant first non-property child.
	 * @return constant instance of nodeAndPrev structure holding information about found node.
	 */
	const nodeAndPrev childNonProperty()const noexcept{
		return const_cast<utki::remove_constptr<decltype(this)>::type*>(this)->childNonProperty();
	}

	/**
	 * @brief Get first property child.
	 * @return instance of nodeAndPrev class holding information about found node.
	 */
	nodeAndPrev childProperty()noexcept;

	/**
	 * @brief Get constant first property child.
	 * @return constant instance of nodeAndPrev class holding information about found node.
	 */
	const nodeAndPrev childProperty()const noexcept{
		return const_cast<utki::remove_constptr<decltype(this)>::type*>(this)->childProperty();
	}

	/**
	 * @brief Get next node in the single-linked list.
	 * Get next sibling node in the single-linked list of nodes.
	 * @return pointer to the next node in the single-linked list.
	 */
	node* next()noexcept{
		return this->next_v.operator->();
	}

	/**
	 * @brief Get constant next node in the single-linked list.
	 * Get constant next sibling node in the single-linked list of nodes.
	 * @return constant pointer tot the next node in the single-linked list.
	 */
	const node* next()const noexcept{
		return this->next_v.operator->();
	}

	/**
	 * @brief Get next node holding the given value.
	 * Get next closest node in the single-linked list which holds the given value.
	 * @param value - value to look for.
	 * @return instance of nodeAndPrev class holding information about found node, previous node is always valid.
	 */
	nodeAndPrev next(const char* value)noexcept;

	/**
	 * @brief Get constant next node holding the given value.
	 * Get constant next closest node in the single-linked list which holds the given value.
	 * @param value - value to look for.
	 * @return instance of nodeAndPrev class holding information about found node, previous node is always valid.
	 */
	const nodeAndPrev next(const char* value)const noexcept{
		return const_cast<utki::remove_constptr<decltype(this)>::type*>(this)->next(value);
	}

	/**
	 * @brief Get node with given value in the chain of nodes.
	 * Get closest node in the single-linked list which holds the given value. This node is included in the search.
	 * @param value - value to look for.
	 * @return instance of nodeAndPrev structure holding information about found node.
	 */
	nodeAndPrev thisOrNext(const char* value)noexcept{
		if(this->operator==(value)){
			return nodeAndPrev(0, this);
		}

		return this->next(value);
	}

	/**
	 * @brief Get constant node with given value in the chain of nodes.
	 * Get closest constant node in the single-linked list which holds the given value. This node is included in the search.
	 * @param value - value to look for.
	 * @return instance of nodeAndPrev structure holding information about found node.
	 */
	const nodeAndPrev thisOrNext(const char* value)const noexcept{
		return const_cast<utki::remove_constptr<decltype(this)>::type*>(this)->thisOrNext(value);
	}


	/**
	 * @brief Get child of given node in the chain of nodes.
	 * @param value - value to look for in the chain of nodes.
	 * @return First child of the node with given value in the chain of nodes.
	 */
	node* childOfThisOrNext(const char* value)noexcept{
		if(auto c = this->thisOrNext(value).get_node()){
			return c->child();
		}
		return nullptr;
	}

	/**
	 * @brief Get constant child of given node in the chain of nodes.
	 * @param value - value to look for in the chain of nodes.
	 * @return Constant first child of the node with given value in the chain of nodes.
	 */
	const node* childOfThisOrNext(const char* value)const noexcept{
		return const_cast<utki::remove_constptr<decltype(this)>::type*>(this)->childOfThisOrNext(value);
	}

	/**
	 * @brief Get next non-property node.
	 * @return instance of nodeAndPrev class holding information about found node, previous node is always valid.
	 */
	nodeAndPrev nextNonProperty()noexcept;

	/**
	 * @brief Get constant next non-property node.
	 * @return constant instance of nodeAndPrev class holding information about found node, previous node is always valid.
	 */
	const nodeAndPrev nextNonProperty()const noexcept{
		return const_cast<utki::remove_constptr<decltype(this)>::type*>(this)->nextNonProperty();
	}

	/**
	 * @brief Get closest non-property node.
	 * This node is included in the search.
	 * @return instance of nodeAndPrev class holding information about found node.
	 */
	nodeAndPrev thisOrNextNonProperty()noexcept{
		if(!this->isProperty()){
			return nodeAndPrev(0, this);
		}

		return this->nextNonProperty();
	}

	/**
	 * @brief Get closest constant non-property node.
	 * This node is included in the search.
	 * @return instance of nodeAndPrev class holding information about found node.
	 */
	const nodeAndPrev thisOrNextNonProperty()const noexcept{
		return const_cast<utki::remove_constptr<decltype(this)>::type*>(this)->thisOrNextNonProperty();
	}

	/**
	 * @brief Get next property child.
	 * @return instance of nodeAndPrev class holding information about found node, previous node is always valid.
	 */
	nodeAndPrev nextProperty()noexcept;

	/**
	 * @brief Get constant next property child.
	 * @return constant instance of nodeAndPrev class holding information about found node, previous node is always valid.
	 */
	const nodeAndPrev nextProperty()const noexcept{
		return const_cast<utki::remove_constptr<decltype(this)>::type*>(this)->nextProperty();
	}

	/**
	 * @brief Get closest property node.
	 * This node is included in the search.
	 * @return instance of nodeAndPrev class holding information about found node.
	 */
	nodeAndPrev thisOrNextProperty()noexcept{
		if(this->isProperty()){
			return nodeAndPrev(0, this);
		}

		return this->nextProperty();
	}

	/**
	 * @brief Get closest constant property node.
	 * This node is included in the search.
	 * @return instance of nodeAndPrev class holding information about found node.
	 */
	const nodeAndPrev thisOrNextProperty()const noexcept{
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
	node* getProperty(const char* propName)noexcept{
		node* prop = this->child(propName).get_node();
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
	const node* getProperty(const char* propName)const noexcept{
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
	node* addProperty(const char* propName);

	/**
	 * @brief Insert a node as a first child.
	 * @param node - node to insert.
	 */
	void addAsFirstChild(std::unique_ptr<node> node)noexcept{
		node->setNext(this->remove_children());
		this->children = std::move(node);
	}

	/**
	 * @brief Insert a new node as a first child.
	 * Make a new node with given value and insert it as a first child.
	 * @param value - value of the new node.
	 */
	void addAsFirstChild(const char* value){
		this->addAsFirstChild(utki::makeUnique<node>(value));
	}

	/**
	 * @brief Insert node into the single-linked list.
	 * Insert the node to the single-linked list as a next node after this node.
	 * @param node - node to insert.
	 */
	void insertNext(std::unique_ptr<node> node)noexcept{
		if(node){
			node->next_v = std::move(this->next_v);
		}
		this->next_v = std::move(node);
	}

	/**
	 * @brief Remove next node from single-linked list.
	 * @return auto-pointer to the node object which has been removed from the single-linked list.
	 */
	std::unique_ptr<node> removeNext()noexcept{
		std::unique_ptr<node> ret = std::move(this->next_v);
		if(ret){
			this->next_v = std::move(ret->next_v);
		}
		return ret;
	}

	/**
	 * @brief Chop single-linked list starting from next node.
	 * After this operation there will be no next node in this single-linked list.
	 * @return auto-pointer to the first node of the single-linked list tail which has been chopped.
	 */
	std::unique_ptr<node> chopNext()noexcept{
		return std::move(this->next_v);
	}

	/**
	 * @brief Set tail of the single-linked list.
	 * Sets the next node for this node to the specified node.
	 * @param node - node to set as the next node.
	 */
	void setNext(std::unique_ptr<node> node)noexcept{
		this->next_v = std::move(node);
	}

	/**
	 * @brief Create a deep copy of the node.
	 * Clones this node and all the underlying nodes hierarchy.
	 * @return A deep copy of this node.
	 */
	std::unique_ptr<node> clone()const;


	/**
	 * @brief Create a deep copy of the node chain.
	 * Clones this node with all its children hierarchy and chained next nodes.
	 * @return a deep copy of this node chain.
	 */
	std::unique_ptr<node> cloneChain()const;

	/**
	 * @brief Clone children chain.
	 * @return a deep copy of this node's children chain.
	 * @return nullptr if this node has no children.
	 */
	std::unique_ptr<node> cloneChildren()const;

	/**
	 * @brief Check if the node is a property.
	 * This is just a convenience method.
	 * Checks if the first character of the value is one of the capital Latin alphabet letters from A to Z.
	 * @return false if the first character of the node's value is a capital letter of Latin alphabet.
	 * @return true otherwise.
	 */
	bool isProperty()const noexcept{
		return this->value() == 0 || this->value()[0] < 'A' || 'Z' < this->value()[0];
	}

	/**
	 * @brief Write this document-object model to the file interface as puu document.
	 * @param fi - file interface to write to.
	 * @param formatted - if true then the puu document will be written with formatting.
	 *                    if false then no formatting will be applied.
	 */
	void writeChain(papki::File& fi, bool formatted = true)const;

	/**
	 * @brief Convert node's chain to string.
	 * @param formatted - should a formatting be applied for better human readability.
	 * @return puu as string.
	 */
	std::string chainToString(bool formatted = false)const;

	//Swap node contents
	friend void swap(node& a, node& b){
		std::swap(a.value_v, b.value_v);
		std::swap(a.children, b.children);
	}
};



/**
 * @brief Load document-object model from puu document.
 * Load document-object model from puu document provided by given file interface.
 * @param fi - file interface to get the puu data from.
 * @return auto-pointer to the first node in the chain of the document-object model.
 */
std::unique_ptr<node> load(const papki::File& fi);



/**
 * @brief Create puu document-object model from string.
 * @param str - null-terminated string describing puu document.
 * @return auto-pointer to the first node in the chain of the document-object model.
 */
std::unique_ptr<node> parse(const char *str);


// TODO: Node is deprecated, remove in version 1.0.0
typedef node Node;


}//~namespace


// TODO: stob name is deprecated, remove in version 1.0.0
namespace stob = puu;
