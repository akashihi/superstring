#include <boost/test/unit_test.hpp>
#include <boost/test/parameterized_test.hpp>
using namespace boost::unit_test;

#include <tuple>

#include "../suffix_array.h"

typedef std::tuple<std::string, std::string, int> LongestSuffixTestParams;

void checkLongestSuffix(const LongestSuffixTestParams &params) {
    const auto& [suffixString, testString, expected] = params;
    auto suffixMap = buildSuffixMap(suffixString);

    auto actual = longestSuffix(testString, suffixString, suffixMap);

    BOOST_TEST(actual == expected);
}

test_suite* init_unit_test_suite(int,char**) {
    LongestSuffixTestParams params[] = {
            std::make_tuple("banana", "", 0),
            std::make_tuple("banana", "zulu", 0),
            std::make_tuple("banana", "ananas", 5),
            std::make_tuple("banana", "anatol", 3),
            std::make_tuple("banana", "ban", 0)
    };

    framework::master_test_suite().
            add( BOOST_PARAM_TEST_CASE(&checkLongestSuffix, params, params+5 ) );

    return nullptr;
}