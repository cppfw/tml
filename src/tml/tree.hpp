/*
The MIT License (MIT)

Copyright (c) 2012-2025 Ivan Gagis <igagis@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/* ================ LICENSE END ================ */

#pragma once

#include <string>

#include <fsif/file.hpp>
#include <utki/string.hpp>
#include <utki/tree.hpp>

/**
 * @brief Tree Markup Language.
 * TML is a markup language for describing trees of strings.
 * This namespace provides functionality for parsing and serializing TML documents.
 */
namespace tml {

/**
 * @brief A leaf, i.e. a node value in a markup document tree.
 * A leaf holds a single string.
 */
class leaf
{
public:
	/**
	 * @brief Leaf value.
	 */
	std::string string;

	/**
	 * @brief Default constructor.
	 * Creates an empty leaf.
	 */
	// NOLINTNEXTLINE(bugprone-exception-escape)
	leaf() = default;

	leaf(const leaf&) = default;
	leaf& operator=(const leaf&) = default;

	// NOLINTNEXTLINE(bugprone-exception-escape)
	leaf(leaf&&) = default;
	leaf& operator=(leaf&&) = default;

	/**
	 * @brief Constructor.
	 * Creates a leaf holding the given string.
	 * @param str - string to create the leaf from.
	 */
	leaf(const char* str) :
		string(str)
	{}

	/**
	 * @brief Constructor.
	 * Creates a leaf holding the given string.
	 * @param str - string to create the leaf from.
	 */
	leaf(std::string str) :
		string(std::move(str))
	{}

	/**
	 * @brief Constructor.
	 * Creates a leaf holding the given string.
	 * @param str - string to create the leaf from.
	 */
	leaf(std::string_view str) :
		string(str)
	{}

	/**
	 * @brief Default destructor.
	 */
	~leaf() = default;

	/**
	 * @brief Equality comparison with a string.
	 * @param str - string to compare with.
	 * @return true if the leaf value is equal to the given string.
	 */
	bool operator==(const char* str) const
	{
		return this->string == str;
	}

	/**
	 * @brief Inequality comparison with a string.
	 * @param str - string to compare with.
	 * @return true if the leaf value is not equal to the given string.
	 */
	bool operator!=(const char* str) const
	{
		return this->string != str;
	}

	/**
	 * @brief Equality comparison with a string.
	 * @param str - string to compare with.
	 * @return true if the leaf value is equal to the given string.
	 */
	bool operator==(const std::string& str) const
	{
		return this->string == str;
	}

	/**
	 * @brief Inequality comparison with a string.
	 * @param str - string to compare with.
	 * @return true if the leaf value is not equal to the given string.
	 */
	bool operator!=(const std::string& str) const
	{
		return this->string != str;
	}

	/**
	 * @brief Equality comparison with a string.
	 * @param str - string to compare with.
	 * @return true if the leaf value is equal to the given string.
	 */
	bool operator==(std::string_view str) const
	{
		return this->string == str;
	}

	/**
	 * @brief Inequality comparison with a string.
	 * @param str - string to compare with.
	 * @return true if the leaf value is not equal to the given string.
	 */
	bool operator!=(std::string_view str) const
	{
		return this->string != str;
	}

	/**
	 * @brief Equality comparison with another leaf.
	 * @param l - other leaf to compare with.
	 * @return true if the leaf values are equal.
	 */
	bool operator==(const leaf& l) const
	{
		return this->string == l.string;
	}

	/**
	 * @brief Inequality comparison with another leaf.
	 * @param l - other leaf to compare with.
	 * @return true if the leaf values are not equal.
	 */
	bool operator!=(const leaf& l) const
	{
		return this->string != l.string;
	}

	/**
	 * @brief Get the leaf value as a C string.
	 * @return the leaf value as a C string.
	 * @deprecated Use the public field 'string' instead.
	 */
	// TODO: remove
	[[deprecated("use public field string")]]
	const char* c_str() const noexcept
	{
		return this->string.c_str();
	}

	/**
	 * @brief Get length of the leaf value.
	 * @return length of the leaf value.
	 */
	size_t length() const noexcept
	{
		return this->string.length();
	}

	/**
	 * @brief Check if the leaf value is empty.
	 * @return true if the leaf value is empty.
	 */
	bool empty() const noexcept
	{
		return this->string.empty();
	}

	/**
	 * @brief Get a character of the leaf value.
	 * @param i - index of the character to get.
	 * @return character of the leaf value at the given index.
	 */
	char operator[](size_t i) const noexcept
	{
		return this->string[i];
	}

	/**
	 * @brief Get the leaf value as a string.
	 * @return the leaf value as a string.
	 * @deprecated Use the public field 'string' instead.
	 */
	// TODO: remove
	[[deprecated("use public field string")]]
	const std::string& to_string() const noexcept
	{
		return this->string;
	}

	/**
	 * @brief Constructor.
	 * Creates a leaf holding the string "true" or "false".
	 * @param value - boolean value to create the leaf from.
	 */
	explicit leaf(bool value);

	/**
	 * @brief Constructor.
	 * The value is converted to a string and stored as the leaf value.
	 * @param value - numeric value to create the leaf from.
	 * @param conversion_base - numeric base to use for the conversion.
	 */
	explicit leaf(unsigned char value, utki::integer_base conversion_base = utki::integer_base::dec);

	/**
	 * @brief Constructor.
	 * The value is converted to a string and stored as the leaf value.
	 * @param value - numeric value to create the leaf from.
	 * @param conversion_base - numeric base to use for the conversion.
	 */
	explicit leaf(unsigned short int value, utki::integer_base conversion_base = utki::integer_base::dec);

	/**
	 * @brief Constructor.
	 * The value is converted to a string and stored as the leaf value.
	 * @param value - numeric value to create the leaf from.
	 */
	explicit leaf(signed int value);

	/**
	 * @brief Constructor.
	 * The value is converted to a string and stored as the leaf value.
	 * @param value - numeric value to create the leaf from.
	 * @param conversion_base - numeric base to use for the conversion.
	 */
	explicit leaf(unsigned int value, utki::integer_base conversion_base = utki::integer_base::dec);

	/**
	 * @brief Constructor.
	 * The value is converted to a string and stored as the leaf value.
	 * @param value - numeric value to create the leaf from.
	 */
	explicit leaf(signed long int value);

	/**
	 * @brief Constructor.
	 * The value is converted to a string and stored as the leaf value.
	 * @param value - numeric value to create the leaf from.
	 * @param conversion_base - numeric base to use for the conversion.
	 */
	explicit leaf(unsigned long int value, utki::integer_base conversion_base = utki::integer_base::dec);

	/**
	 * @brief Constructor.
	 * The value is converted to a string and stored as the leaf value.
	 * @param value - numeric value to create the leaf from.
	 */
	explicit leaf(signed long long int value);

	/**
	 * @brief Constructor.
	 * The value is converted to a string and stored as the leaf value.
	 * @param value - numeric value to create the leaf from.
	 * @param conversion_base - numeric base to use for the conversion.
	 */
	explicit leaf(unsigned long long int value, utki::integer_base conversion_base = utki::integer_base::dec);

	/**
	 * @brief Constructor.
	 * The value is converted to a string and stored as the leaf value.
	 * @param value - floating point value to create the leaf from.
	 */
	explicit leaf(float value);

	/**
	 * @brief Constructor.
	 * The value is converted to a string and stored as the leaf value.
	 * @param value - floating point value to create the leaf from.
	 */
	explicit leaf(double value);

	/**
	 * @brief Constructor.
	 * The value is converted to a string and stored as the leaf value.
	 * @param value - floating point value to create the leaf from.
	 */
	explicit leaf(long double value);

	/**
	 * @brief Convert the leaf value to a boolean.
	 * Only the string "true" is converted to true, any other value is converted to false.
	 * @return boolean value of the leaf.
	 */
	bool to_bool() const;

	/**
	 * @brief Convert the leaf value to a numeric value.
	 * @tparam number_type - numeric type to convert the leaf value to.
	 * @return numeric value of the leaf.
	 * @throw std::invalid_argument if the leaf value is not a number.
	 * @throw std::out_of_range if the number does not fit into the target type.
	 */
	template <typename number_type>
	// NOLINTNEXTLINE(modernize-use-constraints, "use enable_if instead of C++20 requires clause to keep the header C++17-compliant")
	std::enable_if_t<std::is_arithmetic_v<number_type>, number_type> to() const
	{
		return utki::string_parser(this->string).read_number<number_type>();
	}

	/**
	 * @brief Convert the leaf value to a 32-bit signed integer.
	 * @return 32-bit signed integer value of the leaf.
	 * @throw std::invalid_argument if the leaf value is not a number.
	 * @throw std::out_of_range if the number does not fit into the target type.
	 */
	int32_t to_int32() const
	{
		return utki::string_parser(this->string).read_number<int32_t>();
	}

	/**
	 * @brief Convert the leaf value to a 32-bit unsigned integer.
	 * @return 32-bit unsigned integer value of the leaf.
	 * @throw std::invalid_argument if the leaf value is not a number.
	 * @throw std::out_of_range if the number does not fit into the target type.
	 */
	uint32_t to_uint32() const
	{
		return utki::string_parser(this->string).read_number<uint32_t>();
	}

	/**
	 * @brief Convert the leaf value to a 64-bit signed integer.
	 * @return 64-bit signed integer value of the leaf.
	 * @throw std::invalid_argument if the leaf value is not a number.
	 * @throw std::out_of_range if the number does not fit into the target type.
	 */
	int64_t to_int64() const
	{
		return utki::string_parser(this->string).read_number<int64_t>();
	}

	/**
	 * @brief Convert the leaf value to a 64-bit unsigned integer.
	 * @return 64-bit unsigned integer value of the leaf.
	 * @throw std::invalid_argument if the leaf value is not a number.
	 * @throw std::out_of_range if the number does not fit into the target type.
	 */
	uint64_t to_uint64() const
	{
		return utki::string_parser(this->string).read_number<uint64_t>();
	}

	/**
	 * @brief Convert the leaf value to a single precision floating point.
	 * @return single precision floating point value of the leaf.
	 * @throw std::invalid_argument if the leaf value is not a number.
	 */
	float to_float() const
	{
		return utki::string_parser(this->string).read_number<float>();
	}

	/**
	 * @brief Convert the leaf value to a double precision floating point.
	 * @return double precision floating point value of the leaf.
	 * @throw std::invalid_argument if the leaf value is not a number.
	 */
	double to_double() const
	{
		return utki::string_parser(this->string).read_number<double>();
	}

	/**
	 * @brief Convert the leaf value to an extended precision floating point.
	 * @return extended precision floating point value of the leaf.
	 * @throw std::invalid_argument if the leaf value is not a number.
	 */
	long double to_long_double() const
	{
		return utki::string_parser(this->string).read_number<long double>();
	}

	/**
	 * @brief Print the leaf value to an output stream.
	 * @param o - output stream to print to.
	 * @param l - leaf to print.
	 * @return the output stream.
	 */
	friend std::ostream& operator<<(std::ostream& o, const leaf& l)
	{
		o << l.string;
		return o;
	}
};

/**
 * @brief Type of a markup document tree node.
 */
using tree = utki::tree<leaf>;

/**
 * @brief Type of a markup document forest, i.e. a container of trees.
 */
using forest = tree::container_type;

/**
 * @brief Read a markup document from a file.
 * @param fi - file to read the document from. File must not be opened.
 * @return parsed markup document.
 */
forest read(const fsif::file& fi);

/**
 * @brief Read a markup document from a string.
 * @param str - string to read the document from.
 * @return parsed markup document.
 */
forest read(std::string_view str);

/**
 * @brief Markup document write formatting.
 */
enum class formatting {
	/**
	 * @brief Normal formatting with indentation and newlines.
	 */
	normal,

	/**
	 * @brief Minimal formatting with no indentation and newlines.
	 */
	minimal
};

/**
 * @brief Write a markup document to a file.
 * @param wood - markup document to write.
 * @param fi - file to write the document to. File must not be opened.
 * @param fmt - document write formatting.
 */
void write(
	const forest& wood, //
	fsif::file& fi,
	formatting fmt = formatting::normal
);

/**
 * @brief Convert a markup document to a string.
 * @param f - markup document to convert.
 * @return string representation of the markup document.
 */
std::string to_string(const forest& f);

/**
 * @brief Convert a markup document tree to a string.
 * @param t - markup document tree to convert.
 * @return string representation of the markup document tree.
 */
inline std::string to_string(const tree& t)
{
	return to_string(tml::forest({t}));
}

/**
 * @brief Print a markup document to an output stream.
 * @param o - output stream to print to.
 * @param f - markup document to print.
 * @return the output stream.
 */
inline std::ostream& operator<<(std::ostream& o, const forest& f)
{
	o << to_string(f);
	return o;
}

/**
 * @brief Print a markup document tree to an output stream.
 * @param o - output stream to print to.
 * @param t - markup document tree to print.
 * @return the output stream.
 */
inline std::ostream& operator<<(std::ostream& o, const tree& t)
{
	o << to_string(t);
	return o;
}

} // namespace tml
