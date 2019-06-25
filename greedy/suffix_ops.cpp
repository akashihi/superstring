#include "suffix_ops.h"

#include <boost/algorithm/string.hpp>

t_SuffixMap buildSuffixMap(const std::string& str) {
    t_SuffixMap result;

    for(int indx = 0; indx < str.size(); ++indx) {
        result[str.at(indx)].push_back(indx);
    }

    return result;
}

int longestSuffix(const std::string& str, const std::string& suffixStr, const t_SuffixMap& suffixes) {
    if (str.empty()) {
        return 0;
    }
    char first = *str.begin();
    auto positions = suffixes.find(first);
    if (positions == suffixes.end()) {
        return 0; //No first character in that word
    }
    for(auto pos: positions->second) {
        std::string substr = std::string(suffixStr.begin()+pos, suffixStr.end());
        if (boost::algorithm::starts_with(str, substr)) {
            return suffixStr.size() - pos;
        }
    }
    return 0;
}