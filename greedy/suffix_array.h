#pragma once
#include <algorithm>
#include <cstring>
#include <map>
#include <memory.h>
#include <iostream>
#include <string.h>

/**
 * Map of characters and their positions in a string.
 * Positions arrays is sorted by position ascending.
 */
typedef std::map<const char, std::vector<int>> t_SuffixMap;

/**
 * Takes a string and builds map of characters and their positions.
 * @param str String to process
 * @return Map, containing entry for each str's character and array of its occurences.
 */
t_SuffixMap buildSuffixMap(const std::string& str);

/**
 * Takes string and suffix map of the other string and returns length
 * of a longest preffix-suffix match between them. Prefix is taken from the
 * stream and suffixes, obviously, from the suffixes.
 * @param str String to match prefix
 * @param suffixStr String to match suffix
 * @param suffixes suffixes to match suffix
 * @return Longest possible common prefix-suffix of the strings above or 0 if there is no match
 */
int longestSuffix(const std::string& str, const std::string& suffixStr, const t_SuffixMap& suffixes);