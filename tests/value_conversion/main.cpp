#include "../../src/treeml/tree.hpp"

#include <iomanip>

#include <utki/debug.hpp>
#include <papki/fs_file.hpp>

namespace{
template <class T> struct sample_template{
	treeml::leaf leaf;
	std::string expected_string;
	T expected_value;
};

template <class test_type> test_type to_test_type(const treeml::leaf& l);

template <> int32_t to_test_type<int32_t>(const treeml::leaf& l){
	return l.to_int32();
}

template <> int64_t to_test_type<int64_t>(const treeml::leaf& l){
	return l.to_int64();
}

template <> uint32_t to_test_type<uint32_t>(const treeml::leaf& l){
	return l.to_uint32();
}

template <> uint64_t to_test_type<uint64_t>(const treeml::leaf& l){
	return l.to_uint64();
}

template <class test_type> void test_int(){
	typedef sample_template<test_type> sample;

	std::vector<sample> samples = {{
		sample{treeml::leaf(int8_t(-13)), "-13", test_type(-13)},
		sample{treeml::leaf(int16_t(-13)), "-13", test_type(-13)},
		sample{treeml::leaf(int32_t(-13)), "-13", test_type(-13)},
		sample{treeml::leaf(int64_t(-13)), "-13", test_type(-13)},
		sample{treeml::leaf(-13), "-13", test_type(-13)},
		sample{treeml::leaf(-13L), "-13", test_type(-13)},
		sample{treeml::leaf(-13LL), "-13", test_type(-13)},

		sample{treeml::leaf(int8_t(13)), "13", test_type(13)},
		sample{treeml::leaf(int16_t(13)), "13", test_type(13)},
		sample{treeml::leaf(int32_t(13)), "13", test_type(13)},
		sample{treeml::leaf(int64_t(13)), "13", test_type(13)},
		sample{treeml::leaf(13), "13", test_type(13)},
		sample{treeml::leaf(13L), "13", test_type(13)},
		sample{treeml::leaf(13LL), "13", test_type(13)},

		sample{treeml::leaf(-13.34f), "-13.34", test_type(-13)},
	}};

	for(auto& s: samples){
		auto value = to_test_type<test_type>(s.leaf);
		ASSERT_INFO_ALWAYS(s.leaf.to_string() == s.expected_string, "to_string() = " << s.leaf.to_string() << ", expected = " << s.expected_string)
		ASSERT_INFO_ALWAYS(value == s.expected_value, "to_string() = " << s.leaf.to_string() << ", value = " << value)
	}
}

template <class test_type> void test_uint(){
	typedef sample_template<test_type> sample;

	std::vector<sample> samples = {{
		sample{treeml::leaf(uint8_t(0x8d)), "141", test_type(0x8d)},
		sample{treeml::leaf(uint16_t(0x8d)), "141", test_type(0x8d)},
		sample{treeml::leaf(uint32_t(0x8d)), "141", test_type(0x8d)},
		sample{treeml::leaf(uint64_t(0x8d)), "141", test_type(0x8d)},
		sample{treeml::leaf(0x8d), "141", test_type(0x8d)},
		sample{treeml::leaf(0x8du), "141", test_type(0x8d)},
		sample{treeml::leaf(0x8dul), "141", test_type(0x8d)},
		sample{treeml::leaf(0x8dull), "141", test_type(0x8d)},

		// sample{treeml::leaf(~0u), "", test_type(~0)},
		// sample{treeml::leaf(~0ul), "", test_type(~0)},
		// sample{treeml::leaf(~0ull), "", test_type(~0)},
		sample{treeml::leaf(13.34f), "13.34", 13},

		sample{treeml::leaf(uint8_t(074), treeml::base::oct), "074", test_type(074)},
		sample{treeml::leaf(uint16_t(074), treeml::base::oct), "074", test_type(074)},
		sample{treeml::leaf(uint32_t(074), treeml::base::oct), "074", test_type(074)},
		sample{treeml::leaf(uint64_t(074), treeml::base::oct), "074", test_type(074)},
		sample{treeml::leaf(074u, treeml::base::oct), "074", test_type(074)},
		sample{treeml::leaf(074ul, treeml::base::oct), "074", test_type(074)},
		sample{treeml::leaf(074ull, treeml::base::oct), "074", test_type(074)},

		// sample{treeml::leaf(~0u, treeml::base::oct), "", test_type(~0)},
		// sample{treeml::leaf(~0ul, treeml::base::oct), "", test_type(~0)},
		// sample{treeml::leaf(~0ull, treeml::base::oct), "", test_type(~0)},
		sample{treeml::leaf(0u, treeml::base::oct), "00", test_type(0)},
		sample{treeml::leaf(0ul, treeml::base::oct), "00", test_type(0)},
		sample{treeml::leaf(0ull, treeml::base::oct), "00", test_type(0)},

		sample{treeml::leaf(0x8du, treeml::base::hex), "0x8d", test_type(0x8d)},
		sample{treeml::leaf(0x8dul, treeml::base::hex), "0x8d", test_type(0x8d)},
		// sample{treeml::leaf(~0u, treeml::base::hex), "", test_type(~0)},
		// sample{treeml::leaf(~0ul, treeml::base::hex), "", test_type(~0)},
		// sample{treeml::leaf(~0ull, treeml::base::hex), "", test_type(~0)},
		sample{treeml::leaf(0u, treeml::base::hex), "0x0", test_type(0)},
		sample{treeml::leaf(0ul, treeml::base::hex), "0x0", test_type(0)},
		sample{treeml::leaf(0ull, treeml::base::hex), "0x0", test_type(0)},
	}};

	for(auto& s: samples){
		auto value = to_test_type<test_type>(s.leaf);
		if(s.expected_string.length() != 0){
			ASSERT_INFO_ALWAYS(
					s.leaf.to_string() == s.expected_string,
					"to_string() = " << s.leaf.to_string() <<
					", expected_string = " << s.expected_string <<
					", index = " << std::distance(
							samples.begin(),
							std::find_if(
									samples.begin(),
									samples.end(),
									[&s](const sample& smp) -> bool {
										return &s == &smp;
									}
								)
						)
				)
		}
		ASSERT_INFO_ALWAYS(value == s.expected_value, "to_string() = " << s.leaf.to_string() << ", value = " << value << ", expected_value = " << s.expected_value)
	}
}

}

void test_treeml_value_conversion(){
	// bool
	{
		typedef sample_template<bool> sample;

		std::vector<sample> samples = {{
			sample{treeml::leaf(true), "true", true},
			sample{treeml::leaf(false), "false", false},
			sample{treeml::leaf(""), "", false},
			sample{treeml::leaf("werqwe"), "werqwe", false},
			sample{treeml::leaf("false"), "false", false},
			sample{treeml::leaf("true"), "true", true}
		}};

		for(auto& s : samples){
			auto value = s.leaf.to_bool();
			ASSERT_INFO_ALWAYS(s.leaf.to_string() == s.expected_string, "to_string() = " << s.leaf.to_string() << ", expected_string = " << s.expected_string)
			ASSERT_INFO_ALWAYS(value == s.expected_value, "to_string() = " << s.leaf.to_string() << ", value = " << value)
		}
	}

	test_int<int32_t>();
	test_int<int64_t>();

	test_uint<uint32_t>();
	test_uint<uint64_t>();

	// float
	{
		std::vector<float> samples = {{
			3.14f,
			13.0f,
			-0.0f,
			0.0f
		}};

		for(auto n : samples){
			treeml::leaf l(n);
			ASSERT_ALWAYS(l.to_float() == n)
		}
	}

	// double
	{
		std::vector<double> samples = {{
			3.14,
			13.0,
			-0.0,
			0.0
		}};

		for(auto n : samples){
			treeml::leaf l(n);
			ASSERT_ALWAYS(l.to_double() == n)
		}
	}

	// long double
	{
		std::vector<long double> samples = {{
			3.14l,
			13.0l,
			-0.0l,
			0.0l
		}};

		for(auto n : samples){
			treeml::leaf l(n);
			// TRACE(<< "l.to_string() = " << l.to_string() << ", n = " << std::setprecision(31) << n << ", l.to_long_double() = " << l.to_long_double() << std::endl)
			ASSERT_INFO_ALWAYS(l.to_long_double() == n, "l.to_string() = " << l.to_string() << ", n = " << n << ", l.to_long_double() = " << l.to_long_double())
		}
	}
}

void test_tree_to_string_conversion(){
	auto forest = treeml::read(R"qwertyuiop(
		a{b{c}}d{e}f
	)qwertyuiop");

	ASSERT_ALWAYS(treeml::to_string(forest[0]) == "a{b{c}}")
}

void test_forest_to_string_conversion(){
	auto forest = treeml::read(R"qwertyuiop(
		a{b{c}}d{e}f
	)qwertyuiop");

	auto str = treeml::to_string(forest);
	ASSERT_INFO_ALWAYS(str == "a{b{c}}d{e}f", "forest = " << str)
}

int main(int argc, char** argv){

	test_treeml_value_conversion();

	test_tree_to_string_conversion();

	test_forest_to_string_conversion();

	return 0;
}
