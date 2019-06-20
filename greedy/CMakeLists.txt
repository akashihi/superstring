cmake_minimum_required(VERSION 3.14)
project(greedy)

set(CMAKE_CXX_STANDARD 17)

#Check for libraries presence
SET(Boost_USE_STATIC_LIBS ON)
#SET(Boost_USE_MULTITHREADED ON)
FIND_PACKAGE(Boost 1.70.0 COMPONENTS system unit_test_framework)

add_executable(greedy main.cpp suffix_array.h suffix_array.cpp)

enable_testing()
SET(CTEST_ENVIRONMENT "BOOST_TEST_LOG_LEVEL=message")
include(CTest)

add_executable(test_suffix_array test/test_suffix_array.cpp suffix_array.h suffix_array.cpp)
target_link_libraries(test_suffix_array ${Boost_LIBRARIES})
add_test(SuffixArray test_suffix_array)