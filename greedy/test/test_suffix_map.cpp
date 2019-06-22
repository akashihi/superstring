#include <boost/test/unit_test.hpp>
#include <boost/test/parameterized_test.hpp>
using namespace boost::unit_test;

#include <tuple>

#include "../suffix_ops.h"

typedef std::tuple<std::string, t_SuffixMap> SuffixMapTestParams;

void checkSuffixMap(const SuffixMapTestParams &params) {
    const auto& [problem, expected] = params;
    auto actual = buildSuffixMap(problem);

    BOOST_TEST(actual == expected);
}

test_suite* init_unit_test_suite(int,char**) {
     SuffixMapTestParams params[] = {
            std::make_tuple<std::string, t_SuffixMap>("banana", t_SuffixMap{{'a', {1, 3, 5}}, {'b', {0}}, {'n', {2, 4}}}),
            std::make_tuple<std::string, t_SuffixMap>("abracadabra", t_SuffixMap{{'a', {0, 3, 5, 7, 10}}, {'b', {1, 8}}, {'c', {4}}, {'d', {6}}, {'r', {2, 9}}})
    };

    framework::master_test_suite().
            add( BOOST_PARAM_TEST_CASE(&checkSuffixMap, params, params+2 ) );

    return nullptr;
}