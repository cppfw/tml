#include <tst/set.hpp>
#include <tst/check.hpp>

#include "../../../src/tml/tree.hpp"
#include "../../../src/tml/tree_ext.hpp"

using namespace std::string_view_literals;
using namespace std::string_literals;

namespace{
// NOLINTNEXTLINE(bugprone-exception-escape)
template <class value_type> struct sample_template{
	tml::leaf leaf;
	std::string expected_string;
	value_type expected_value;
};

template <class test_type> test_type to_test_type(const tml::leaf& l);

template <> int32_t to_test_type<int32_t>(const tml::leaf& l){
	return l.to_int32();
}

template <> int64_t to_test_type<int64_t>(const tml::leaf& l){
	return l.to_int64();
}

template <> uint32_t to_test_type<uint32_t>(const tml::leaf& l){
	return l.to_uint32();
}

template <> uint64_t to_test_type<uint64_t>(const tml::leaf& l){
	return l.to_uint64();
}
}

namespace{
template <class test_type>
void test_int(tst::suite& suite, const std::string& test_name){
	suite.add<sample_template<test_type>>(
			test_name,
			{
				{tml::leaf(int8_t(-13)), "-13", test_type(-13)},
				{tml::leaf(int16_t(-13)), "-13", test_type(-13)},
				{tml::leaf(int32_t(-13)), "-13", test_type(-13)},
				{tml::leaf(int64_t(-13)), "-13", test_type(-13)},
				{tml::leaf(-13), "-13", test_type(-13)},
				{tml::leaf(-13L), "-13", test_type(-13)},
				{tml::leaf(-13LL), "-13", test_type(-13)},
				{tml::leaf(int8_t(13)), "13", test_type(13)},
				{tml::leaf(int16_t(13)), "13", test_type(13)},
				{tml::leaf(int32_t(13)), "13", test_type(13)},
				{tml::leaf(int64_t(13)), "13", test_type(13)},
				{tml::leaf(13), "13", test_type(13)},
				{tml::leaf(13L), "13", test_type(13)},
				{tml::leaf(13LL), "13", test_type(13)},
				{tml::leaf(-13.34f), "-13.34", test_type(-13)},
			},
			[](auto& p){
				auto value = to_test_type<test_type>(p.leaf);
				tst::check_eq(p.leaf.string, p.expected_string, SL);
				tst::check_eq(value, p.expected_value, SL);
			}
		);
}
}

namespace{
template <class test_type>
void test_uint(tst::suite& suite, const std::string& test_name){
	suite.add<sample_template<test_type>>(
			test_name,
			{
				{tml::leaf(uint8_t(0x8d)), "141", test_type(0x8d)},
				{tml::leaf(uint16_t(0x8d)), "141", test_type(0x8d)},
				{tml::leaf(uint32_t(0x8d)), "141", test_type(0x8d)},
				{tml::leaf(uint64_t(0x8d)), "141", test_type(0x8d)},
				{tml::leaf(0x8d), "141", test_type(0x8d)},
				{tml::leaf(0x8du), "141", test_type(0x8d)},
				{tml::leaf(0x8dul), "141", test_type(0x8d)},
				{tml::leaf(0x8dull), "141", test_type(0x8d)},

				// {tml::leaf(~0u), "", test_type(~0)},
				// {tml::leaf(~0ul), "", test_type(~0)},
				// {tml::leaf(~0ull), "", test_type(~0)},
				{tml::leaf(13.34f), "13.34", 13},

				{tml::leaf(uint8_t(074), utki::integer_base::oct), "074", test_type(074)},
				{tml::leaf(uint16_t(074), utki::integer_base::oct), "074", test_type(074)},
				{tml::leaf(uint32_t(074), utki::integer_base::oct), "074", test_type(074)},
				{tml::leaf(uint64_t(074), utki::integer_base::oct), "074", test_type(074)},
				{tml::leaf(074u, utki::integer_base::oct), "074", test_type(074)},
				{tml::leaf(074ul, utki::integer_base::oct), "074", test_type(074)},
				{tml::leaf(074ull, utki::integer_base::oct), "074", test_type(074)},

				// {tml::leaf(~0u, utki::integer_base::oct), "", test_type(~0)},
				// {tml::leaf(~0ul, utki::integer_base::oct), "", test_type(~0)},
				// {tml::leaf(~0ull, utki::integer_base::oct), "", test_type(~0)},
				{tml::leaf(0u, utki::integer_base::oct), "00", test_type(0)},
				{tml::leaf(0ul, utki::integer_base::oct), "00", test_type(0)},
				{tml::leaf(0ull, utki::integer_base::oct), "00", test_type(0)},

				{tml::leaf(0x8du, utki::integer_base::hex), "0x8d", test_type(0x8d)},
				{tml::leaf(0x8dul, utki::integer_base::hex), "0x8d", test_type(0x8d)},
				// {tml::leaf(~0u, utki::integer_base::hex), "", test_type(~0)},
				// {tml::leaf(~0ul, utki::integer_base::hex), "", test_type(~0)},
				// {tml::leaf(~0ull, utki::integer_base::hex), "", test_type(~0)},
				{tml::leaf(0u, utki::integer_base::hex), "0x0", test_type(0)},
				{tml::leaf(0ul, utki::integer_base::hex), "0x0", test_type(0)},
				{tml::leaf(0ull, utki::integer_base::hex), "0x0", test_type(0)},
			},
			[](auto& p){
				auto value = to_test_type<test_type>(p.leaf);
				if(p.expected_string.length() != 0){
					tst::check_eq(p.leaf.string, p.expected_string, SL);
				}
				tst::check_eq(value, p.expected_value, SL) << " string = " << p.leaf.string;
			}
		);
}
}

namespace{
const tst::set set("value_conversion", [](tst::suite& suite){
	suite.add<sample_template<bool>>(
			"convert_to_bool",
			{
				// NOLINTNEXTLINE(modernize-use-designated-initializers, "needs C++20, but we use C++17")
				{tml::leaf(true), "true", true},
				{tml::leaf(false), "false", false}, // NOLINT(modernize-use-designated-initializers)
				{tml::leaf(""), "", false}, // NOLINT(modernize-use-designated-initializers)
				{tml::leaf("werqwe"), "werqwe", false}, // NOLINT(modernize-use-designated-initializers)
				{tml::leaf("false"s), "false", false}, // NOLINT(modernize-use-designated-initializers)
				{tml::leaf("true"sv), "true", true} // NOLINT(modernize-use-designated-initializers)
			},
			[](auto& p){
				auto value = p.leaf.to_bool();
				tst::check_eq(p.leaf.string, p.expected_string, SL);
				tst::check_eq(value, p.expected_value, SL);
			}
		);

	test_int<int32_t>(suite, "convert_to_int32_t");
	test_int<int64_t>(suite, "convert_to_int64_t");
	test_uint<uint32_t>(suite, "convert_to_uint32_t");
	test_uint<uint64_t>(suite, "convert_to_uint64_t");

	suite.add<float>(
			"convert_to_float",
			{
				3.14f,
				13.0f,
				-0.0f,
				0.0f
			},
			[](auto& p){
				tml::leaf l(p);
				tst::check_eq(l.to_float(), p, SL);
			}
		);
	
	suite.add<double>(
			"convert_to_double",
			{
				3.14,
				13.0,
				-0.0,
				0.0
			},
			[](auto& p){
				tml::leaf l(p);
				tst::check_eq(l.to_double(), p, SL);
			}
		);
	
	suite.add<long double>(
			"convert_to_long_double",
			{
				3.140000000000000000095409791179,
				13.0l,
				-0.0l,
				0.0l
			},
			[](auto& p){
				tml::leaf l(p);
				tst::check_eq(l.to_long_double(), p, SL) << "l.string = " << l.string;
			}
		);
	
	suite.add<std::pair<std::string, std::string>>(
			"convert_tree_to_string",
			{
				{"a{b{c}}d{e}f", "a{b{c}}"},
				{"a{b{c}c{\"d\"}}d{e}f", "a{b{c}c{d}}"},
			},
			[](auto& p){
				auto forest = tml::read(p.first);
				tst::check_eq(tml::to_string(forest[0]), p.second, SL);
			}
		);
	
	suite.add<std::pair<std::string, std::string>>(
			"convert_forest_to_string",
			{
				{"a{b{c}}d{e}f", "a{b{c}}d{e}f"},
			},
			[](auto& p){
				auto forest = tml::read(p.first);
				auto str = tml::to_string(forest);
				tst::check_eq(str, p.second, SL);
			}
		);
	
	suite.add("string_to_leaf_ext", [](){
		std::string str = "hello!";

		tml::leaf_ext l{std::string(str)};

		tst::check_eq(l.string, str, SL);
	});
});
}
