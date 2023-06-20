/*
The MIT License (MIT)

Copyright (c) 2012-2023 Ivan Gagis <igagis@gmail.com>

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

#include <papki/file.hpp>
#include <utki/string.hpp>
#include <utki/tree.hpp>

// TODO: doxygen
namespace tml {

class leaf
{
	std::string string;

public:
	leaf() = default;
	leaf(const leaf&) = default;
	leaf(leaf&&) = default;
	leaf& operator=(const leaf&) = default;

	leaf(const char* str) :
		string(str)
	{}

	leaf(const std::string& str) :
		string(str)
	{}

	leaf(std::string&& str) :
		string(std::move(str))
	{}

	bool operator==(const char* str) const
	{
		return this->string == str;
	}

	bool operator!=(const char* str) const
	{
		return this->string != str;
	}

	bool operator==(const std::string& str) const
	{
		return this->string == str;
	}

	bool operator!=(const std::string& str) const
	{
		return this->string != str;
	}

	bool operator==(std::string_view str) const
	{
		return this->string == str;
	}

	bool operator!=(std::string_view str) const
	{
		return this->string != str;
	}

	bool operator==(const leaf& l) const
	{
		return this->string == l.string;
	}

	bool operator!=(const leaf& l) const
	{
		return this->string != l.string;
	}

	const char* c_str() const noexcept
	{
		return this->string.c_str();
	}

	size_t length() const noexcept
	{
		return this->string.length();
	}

	bool empty() const noexcept
	{
		return this->string.empty();
	}

	char operator[](size_t i) const noexcept
	{
		return this->string[i];
	}

	const std::string& to_string() const noexcept
	{
		return this->string;
	}

	explicit leaf(bool value);

	explicit leaf(unsigned char value, utki::integer_base conversion_base = utki::integer_base::dec);
	explicit leaf(unsigned short int value, utki::integer_base conversion_base = utki::integer_base::dec);

	explicit leaf(signed int value);
	explicit leaf(unsigned int value, utki::integer_base conversion_base = utki::integer_base::dec);

	explicit leaf(signed long int value);
	explicit leaf(unsigned long int value, utki::integer_base conversion_base = utki::integer_base::dec);

	explicit leaf(signed long long int value);
	explicit leaf(unsigned long long int value, utki::integer_base conversion_base = utki::integer_base::dec);

	explicit leaf(float value);
	explicit leaf(double value);
	explicit leaf(long double value);

	bool to_bool() const;

	template <typename number_type>
	std::enable_if_t<std::is_arithmetic_v<number_type>, number_type> to() const
	{
		return utki::string_parser(this->string).read_number<number_type>();
	}

	int32_t to_int32() const
	{
		return utki::string_parser(this->string).read_number<int32_t>();
	}

	uint32_t to_uint32() const
	{
		return utki::string_parser(this->string).read_number<uint32_t>();
	}

	int64_t to_int64() const
	{
		return utki::string_parser(this->string).read_number<int64_t>();
	}

	uint64_t to_uint64() const
	{
		return utki::string_parser(this->string).read_number<uint64_t>();
	}

	float to_float() const
	{
		return utki::string_parser(this->string).read_number<float>();
	}

	double to_double() const
	{
		return utki::string_parser(this->string).read_number<double>();
	}

	long double to_long_double() const
	{
		return utki::string_parser(this->string).read_number<long double>();
	}

	friend std::ostream& operator<<(std::ostream& o, const leaf& l)
	{
		o << l.to_string();
		return o;
	}
};

typedef utki::tree<leaf> tree;
typedef tree::container_type forest;

forest read(const papki::file& fi);
forest read(const std::string& str);

enum class formatting {
	normal,
	minimal
};

void write(const forest& wood, papki::file& fi, formatting fmt = formatting::normal);

std::string to_string(const forest& f);

inline std::string to_string(const tree& t)
{
	return to_string(tml::forest({t}));
}

inline std::ostream& operator<<(std::ostream& o, const forest& f)
{
	o << to_string(f);
	return o;
}

inline std::ostream& operator<<(std::ostream& o, const tree& t)
{
	o << to_string(t);
	return o;
}

} // namespace tml

// TODO: remove?
namespace treeml = tml;
