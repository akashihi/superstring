#pragma once
#include <algorithm>
#include <cstring>
#include <map>
#include <memory.h>
#include <iostream>
#include <string.h>

typedef std::tuple<std::string, std::vector<int>> t_StringSuffix;
std::vector<int> buildSuffixArray(const std::string& str);

typedef std::map<const char, std::vector<int>> t_SuffixMap;
t_SuffixMap buildSuffixMap(const std::string& str);