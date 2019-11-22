#include "../../src/puu/tree.hpp"

#include <iomanip>

#include <utki/debug.hpp>
#include <papki/FSFile.hpp>

namespace{
template <class T> struct sample_template{
	puu::leaf leaf;
	std::string expected_string;
	T expected_value;
};

template <class test_type> test_type to_test_type(const puu::leaf& l);

template <> int32_t to_test_type<int32_t>(const puu::leaf& l){
	return l.to_int32();
}

template <> int64_t to_test_type<int64_t>(const puu::leaf& l){
	return l.to_int64();
}

template <> uint32_t to_test_type<uint32_t>(const puu::leaf& l){
	return l.to_uint32();
}

template <> uint64_t to_test_type<uint64_t>(const puu::leaf& l){
	return l.to_uint64();
}

template <class test_type> void test_int(){
	typedef sample_template<test_type> sample;

	std::vector<sample> samples = {{
		sample{puu::leaf(int8_t(-13)), "-13", test_type(-13)},
		sample{puu::leaf(int16_t(-13)), "-13", test_type(-13)},
		sample{puu::leaf(int32_t(-13)), "-13", test_type(-13)},
		sample{puu::leaf(int64_t(-13)), "-13", test_type(-13)},
		sample{puu::leaf(-13), "-13", test_type(-13)},
		sample{puu::leaf(-13L), "-13", test_type(-13)},
		sample{puu::leaf(-13LL), "-13", test_type(-13)},

		sample{puu::leaf(int8_t(13)), "13", test_type(13)},
		sample{puu::leaf(int16_t(13)), "13", test_type(13)},
		sample{puu::leaf(int32_t(13)), "13", test_type(13)},
		sample{puu::leaf(int64_t(13)), "13", test_type(13)},
		sample{puu::leaf(13), "13", test_type(13)},
		sample{puu::leaf(13L), "13", test_type(13)},
		sample{puu::leaf(13LL), "13", test_type(13)},

		sample{puu::leaf(-13.34f), "-13.34", test_type(-13)},
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
		sample{puu::leaf(uint8_t(0x8d)), "141", test_type(0x8d)},
		sample{puu::leaf(uint16_t(0x8d)), "141", test_type(0x8d)},
		sample{puu::leaf(uint32_t(0x8d)), "141", test_type(0x8d)},
		sample{puu::leaf(uint64_t(0x8d)), "141", test_type(0x8d)},
		sample{puu::leaf(0x8d), "141", test_type(0x8d)},
		sample{puu::leaf(0x8du), "141", test_type(0x8d)},
		sample{puu::leaf(0x8dul), "141", test_type(0x8d)},
		sample{puu::leaf(0x8dull), "141", test_type(0x8d)},

		// sample{puu::leaf(~0u), "", test_type(~0)},
		// sample{puu::leaf(~0ul), "", test_type(~0)},
		// sample{puu::leaf(~0ull), "", test_type(~0)},
		sample{puu::leaf(13.34f), "13.34", 13},

		sample{puu::leaf(uint8_t(074), std::oct), "074", test_type(074)},
		sample{puu::leaf(uint16_t(074), std::oct), "074", test_type(074)},
		sample{puu::leaf(uint32_t(074), std::oct), "074", test_type(074)},
		sample{puu::leaf(uint64_t(074), std::oct), "074", test_type(074)},
		sample{puu::leaf(074u, std::oct), "074", test_type(074)},
		sample{puu::leaf(074ul, std::oct), "074", test_type(074)},
		sample{puu::leaf(074ull, std::oct), "074", test_type(074)},

		// sample{puu::leaf(~0u, std::oct), "", test_type(~0)},
		// sample{puu::leaf(~0ul, std::oct), "", test_type(~0)},
		// sample{puu::leaf(~0ull, std::oct), "", test_type(~0)},
		sample{puu::leaf(0u, std::oct), "00", test_type(0)},
		sample{puu::leaf(0ul, std::oct), "00", test_type(0)},
		sample{puu::leaf(0ull, std::oct), "00", test_type(0)},

		sample{puu::leaf(0x8du, std::hex), "0x8d", test_type(0x8d)},
		sample{puu::leaf(0x8dul, std::hex), "0x8d", test_type(0x8d)},
		// sample{puu::leaf(~0u, std::hex), "", test_type(~0)},
		// sample{puu::leaf(~0ul, std::hex), "", test_type(~0)},
		// sample{puu::leaf(~0ull, std::hex), "", test_type(~0)},
		sample{puu::leaf(0u, std::hex), "0x0", test_type(0)},
		sample{puu::leaf(0ul, std::hex), "0x0", test_type(0)},
		sample{puu::leaf(0ull, std::hex), "0x0", test_type(0)},
	}};

	for(auto& s: samples){
		auto value = to_test_type<test_type>(s.leaf);
		if(s.expected_string.length() != 0){
			ASSERT_INFO_ALWAYS(
					s.leaf.to_string() == s.expected_string,
					"to_string() = " << s.leaf.to_string() <<
					", expected = " << s.expected_string <<
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

void test_puu_value_conversion(){
	// bool
	{
		typedef sample_template<bool> sample;

		std::vector<sample> samples = {{
			sample{puu::leaf(true), "true", true},
			sample{puu::leaf(false), "false", false},
			sample{puu::leaf(""), "", false},
			sample{puu::leaf("werqwe"), "werqwe", false},
			sample{puu::leaf("false"), "false", false},
			sample{puu::leaf("true"), "true", true}
		}};

		for(auto& s : samples){
			auto value = s.leaf.to_bool();
			ASSERT_INFO_ALWAYS(s.leaf.to_string() == s.expected_string, "to_string() = " << s.leaf.to_string() << ", expected = " << s.expected_string)
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
			puu::leaf l(n);
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
			puu::leaf l(n);
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
			puu::leaf l(n);
			// TRACE(<< "l.to_string() = " << l.to_string() << ", n = " << std::setprecision(31) << n << ", l.to_long_double() = " << l.to_long_double() << std::endl)
			ASSERT_INFO_ALWAYS(l.to_long_double() == n, "l.to_string() = " << l.to_string() << ", n = " << n << ", l.to_long_double() = " << l.to_long_double())
		}
	}
}



int main(int argc, char** argv){

	test_puu_value_conversion();

	return 0;
}
