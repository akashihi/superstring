cmake_minimum_required(VERSION 3.14)
project(greedy)

set(CMAKE_CXX_STANDARD 17)

#Check for libraries presence
SET(Boost_USE_STATIC_LIBS ON)
#SET(Boost_USE_MULTITHREADED ON)
FIND_PACKAGE(Boost 1.70.0 COMPONENTS system unit_test_framework)

add_library(staticops STATIC suffix_ops.h suffix_ops.cpp)

add_executable(greedy main.cpp suffix_ops.h)
target_link_libraries(greedy staticops)

enable_testing()
SET(CTEST_ENVIRONMENT "BOOST_TEST_LOG_LEVEL=message")
include(CTest)

add_executable(test_suffix_map test/test_suffix_map.cpp suffix_ops.h)
target_link_libraries(test_suffix_map staticops ${Boost_LIBRARIES})
add_test(SuffixMap test_suffix_map)

add_executable(test_longest_suffix test/test_longest_suffix.cpp suffix_ops.h)
target_link_libraries(test_longest_suffix staticops ${Boost_LIBRARIES})
add_test(LongestSuffix test_longest_suffix)