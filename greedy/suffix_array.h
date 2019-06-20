#pragma once
#include <algorithm>
#include <cstring>
#include <memory.h>
#include <iostream>
#include <string.h>

typedef std::tuple<std::string, std::vector<int>> t_StringSuffix;
std::vector<int> buildSuffixArray(const std::string& str);