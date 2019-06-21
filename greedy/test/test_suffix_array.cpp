#include <boost/test/unit_test.hpp>
#include <boost/test/parameterized_test.hpp>
using namespace boost::unit_test;

#include <tuple>

#include "../suffix_array.h"

typedef std::tuple<std::string, std::vector<int>> SuffixMapTestParams;

void checkSuffixArray(const SuffixMapTestParams& params) {
    const auto& [problem, expected] = params;
    auto actual = buildSuffixArray(problem);

    BOOST_TEST(actual == expected);
}

test_suite* init_unit_test_suite(int,char**)
{
    SuffixMapTestParams params[] = {
            std::make_tuple<std::string, std::vector<int>>("banana", {5, 3, 1, 0, 4, 2}),
            std::make_tuple<std::string, std::vector<int>>("abracadabra", {10, 7, 0, 3, 5, 8, 1, 4, 6, 9, 2})
    };

    framework::master_test_suite().
            add( BOOST_PARAM_TEST_CASE( &checkSuffixArray, params, params+2 ) );

    return nullptr;
}