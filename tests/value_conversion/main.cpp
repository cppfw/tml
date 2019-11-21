#include "../../src/puu/tree.hpp"

#include <iomanip>

#include <utki/debug.hpp>
#include <papki/FSFile.hpp>



void test_puu_value_conversion(){
	// bool
	{
		puu::branches roots;

		roots.emplace_back(puu::leaf(true));
		roots.emplace_back(puu::leaf(false));
		roots.emplace_back(puu::leaf(""));
		roots.emplace_back(puu::leaf("qwerfqwef"));

		ASSERT_ALWAYS(roots.size() == 4)
		ASSERT_ALWAYS(roots[0].value.str() == "true")
		ASSERT_ALWAYS(roots[0].value.to_bool() == true)
		ASSERT_ALWAYS(roots[1].value.str() == "false")
		ASSERT_ALWAYS(roots[1].value.to_bool() == false)
		ASSERT_INFO_ALWAYS(roots[2].value.to_bool() == false, roots[2].value.str())
		ASSERT_ALWAYS(roots[3].value.to_bool() == false)
	}

	// int32
	{
		puu::branches roots;

		roots.emplace_back(puu::leaf(-13));
		roots.emplace_back(puu::leaf(13));
		roots.emplace_back(puu::leaf());

		ASSERT_ALWAYS(roots.size() == 3)
		ASSERT_ALWAYS(roots[0].value.str() == "-13")
		ASSERT_ALWAYS(roots[0].value.to_int32() == -13)
		ASSERT_ALWAYS(roots[1].value.str() == "13")
		ASSERT_ALWAYS(roots[1].value.to_int32() == 13)
		ASSERT_ALWAYS(roots[2].value.str().length() == 0)
		ASSERT_ALWAYS(roots[2].value.to_int32() == 0)
	}

	// uint32
	{
		puu::branches roots;

		roots.emplace_back(puu::leaf(0xffffffffu));
		roots.emplace_back(puu::leaf());
		roots.emplace_back(puu::leaf(0x8du, std::hex));
		roots.emplace_back(puu::leaf(0xffffffffu, std::hex));
		roots.emplace_back(puu::leaf(0u, std::hex));
		roots.emplace_back(puu::leaf(074u, std::oct));
		roots.emplace_back(puu::leaf(037777777777u, std::oct));
		roots.emplace_back(puu::leaf(0u, std::oct));

		ASSERT_ALWAYS(roots.size() == 8)

		ASSERT_INFO_ALWAYS(roots[0].value.str() == "4294967295", roots[0].value.str())
		ASSERT_ALWAYS(roots[0].value.to_uint32() == 0xffffffffu)

		ASSERT_ALWAYS(roots[1].value.str().length() == 0)
		ASSERT_ALWAYS(roots[1].value.to_uint32() == 0)

		ASSERT_INFO_ALWAYS(roots[2].value.str() == "0x8d", roots[2].value.str())
		ASSERT_ALWAYS(roots[2].value.to_uint32() == 0x8d)

		ASSERT_ALWAYS(roots[3].value.str() == "0xffffffff")
		ASSERT_ALWAYS(roots[3].value.to_uint32() == 0xffffffff)

		ASSERT_INFO_ALWAYS(roots[4].value.str() == "0x0", roots[4].value.str())
		ASSERT_ALWAYS(roots[4].value.to_uint32() == 0)

		ASSERT_INFO_ALWAYS(roots[5].value.str() == "074", roots[5].value.str())
		ASSERT_ALWAYS(roots[5].value.to_uint32() == 074)

		ASSERT_INFO_ALWAYS(roots[6].value.str() == "037777777777", roots[6].value.str())
		ASSERT_ALWAYS(roots[6].value.to_uint32() == 037777777777)

		ASSERT_INFO_ALWAYS(roots[7].value.str() == "00", roots[7].value.str())
		ASSERT_ALWAYS(roots[7].value.to_uint32() == 0)
	}

	// int64
	{
		puu::branches roots;

		roots.emplace_back(puu::leaf(-13L));
		roots.emplace_back(puu::leaf(13L));
		roots.emplace_back(puu::leaf());

		ASSERT_ALWAYS(roots.size() == 3)
		ASSERT_ALWAYS(roots[0].value.str() == "-13")
		ASSERT_ALWAYS(roots[0].value.to_int64() == -13)
		ASSERT_ALWAYS(roots[1].value.str() == "13")
		ASSERT_ALWAYS(roots[1].value.to_int64() == 13)
		ASSERT_ALWAYS(roots[2].value.str().length() == 0)
		ASSERT_ALWAYS(roots[2].value.to_int64() == 0)
	}

	// uint64
	{
		puu::branches roots;

		roots.emplace_back(puu::leaf(~0ul));
		roots.emplace_back(puu::leaf());
		roots.emplace_back(puu::leaf(0x8dul, std::hex));
		roots.emplace_back(puu::leaf(~0ul, std::hex));
		roots.emplace_back(puu::leaf(0ul, std::hex));
		roots.emplace_back(puu::leaf(074ul, std::oct));
		roots.emplace_back(puu::leaf(~0ul, std::oct));
		roots.emplace_back(puu::leaf(0ul, std::oct));

		ASSERT_ALWAYS(roots.size() == 8)

		ASSERT_INFO_ALWAYS(roots[0].value.str() == "18446744073709551615", roots[0].value.str())
		ASSERT_ALWAYS(roots[0].value.to_uint64() == 0xfffffffffffffffful)

		ASSERT_ALWAYS(roots[1].value.str().length() == 0)
		ASSERT_ALWAYS(roots[1].value.to_uint64() == 0)

		ASSERT_INFO_ALWAYS(roots[2].value.str() == "0x8d", roots[2].value.str())
		ASSERT_ALWAYS(roots[2].value.to_uint64() == 0x8d)

		ASSERT_ALWAYS(roots[3].value.str() == "0xffffffffffffffff")
		ASSERT_ALWAYS(roots[3].value.to_uint64() == 0xffffffffffffffff)

		ASSERT_INFO_ALWAYS(roots[4].value.str() == "0x0", roots[4].value.str())
		ASSERT_ALWAYS(roots[4].value.to_uint64() == 0)

		ASSERT_INFO_ALWAYS(roots[5].value.str() == "074", roots[5].value.str())
		ASSERT_ALWAYS(roots[5].value.to_uint64() == 074)

		ASSERT_INFO_ALWAYS(roots[6].value.str() == "01777777777777777777777", roots[6].value.str())
		ASSERT_ALWAYS(roots[6].value.to_uint64() == ~0ul)

		ASSERT_INFO_ALWAYS(roots[7].value.str() == "00", roots[7].value.str())
		ASSERT_ALWAYS(roots[7].value.to_uint64() == 0)
	}

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
			// TRACE(<< "l.str() = " << l.str() << ", n = " << std::setprecision(31) << n << ", l.to_long_double() = " << l.to_long_double() << std::endl)
			ASSERT_INFO_ALWAYS(l.to_long_double() == n, "l.str() = " << l.str() << ", n = " << n << ", l.to_long_double() = " << l.to_long_double())
		}
	}
}



int main(int argc, char** argv){

	test_puu_value_conversion();

	return 0;
}
