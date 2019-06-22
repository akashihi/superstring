#include <algorithm>
#include <fstream>
#include <iterator>
#include <iostream>
#include <list>
#include <set>
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/sort/sort.hpp>
#include "suffix_ops.h"

namespace {

}

int main(int argc, char** argv) {
    if (argc<2) {
        std::cout << "Usage: greedy <file_with_strings>" << std::endl;
        return 1;
    }

    //0 - Load the file
    std::ifstream inputFile(argv[1]);
    std::vector<std::string> inputStrings;
    std::copy(std::istream_iterator<std::string>(inputFile),
              std::istream_iterator<std::string>(),
              std::back_inserter(inputStrings));
    std::remove_if(std::begin(inputStrings), std::end(inputStrings), [](const std::string& str) { return !str.empty();  });
    boost::sort::pdqsort(std::begin(inputStrings), std::end(inputStrings), [](const std::string& left, const std::string right) { return left.size() > right.size(); });
    std::cout << "Loaded " << inputStrings.size() << " words" << std::endl;

    //1 - Calculate suffix map for each dataset entry
    std::list<t_StringWithSuffix> stringSuffixes;
    std::transform(std::begin(inputStrings), std::end(inputStrings), std::back_inserter(stringSuffixes), [](const std::string& str) {
        return std::make_tuple(str, buildSuffixMap(str));
    });

    //2 - Greedy solver
    std::string result;
    for(auto outer = stringSuffixes.cbegin(); outer != stringSuffixes.cend();) {
        auto [outerStr, suffixMap] = *outer;

        std::set<std::string> duplicates;

        std::string match;
        int maxSuffix = 0;

        for (auto inner = outer; inner != stringSuffixes.cend(); ++inner ) {
            const std::string& innerStr = std::get<0>(*inner);
            //std::cout << "Outer is: " << outerStr << ", inner is: " << innerStr << std::endl;
            if (outer == inner || outerStr == innerStr) {
                //Found self, skipping
                continue;
            }
            if (boost::algorithm::contains(outerStr, innerStr)) {
                //std::cout << "Inner is contained in the outer, removing" << std::endl;
                duplicates.insert(innerStr);
                continue;
            }
            if (maxSuffix > innerStr.size()) {
                //No reason to continue, as all following words are shorter.
                //But we will not break up the cycle, as we want to find other fully
                //contained words
                continue;
            }
            int suffix = longestSuffix(innerStr, outerStr, suffixMap);
            if (suffix > maxSuffix) {
                maxSuffix = suffix;
                match = innerStr;
            }
        }
        result += outerStr;

        if (maxSuffix > 0) {
            result.append(match.begin()+maxSuffix, match.end());
            duplicates.insert(match);
        }

        stringSuffixes.erase(std::remove_if(stringSuffixes.begin(), stringSuffixes.end(), [&duplicates](auto item) { return duplicates.find(std::get<0>(item)) != duplicates.end(); }), stringSuffixes.end());

        ++outer;
    }

    std::cout << "Super string length is " << result.size() << std::endl;
    std::cout << "Super string is " << result << std::endl;
    return 0;
}