#include <algorithm>
#include <fstream>
#include <iterator>
#include <iostream>
#include <set>
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/sort/sort.hpp>
#include "suffix_ops.h"

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

    //1 - Drop shorter words which are contained in a longer ones
    std::set<std::string> contained;
    for(auto outer = inputStrings.cbegin(); outer != inputStrings.cend(); ++outer) {
        for (auto inner = outer+1; inner != inputStrings.cend(); ++inner ) {
            if (boost::algorithm::contains(*outer, *inner)) {
                std::cout << *outer << " contains " << *inner << std::endl;
                contained.insert(*inner);
            }
        }
    }
    inputStrings.erase(std::remove_if(inputStrings.begin(), inputStrings.end(), [&contained](auto item) { return contained.find(item) != contained.end(); }), inputStrings.end());

    //2 - Calculate suffix map for each dataset entry
    std::vector<t_StringWithSuffix> stringSuffixes;
    std::transform(std::begin(inputStrings), std::end(inputStrings), std::back_inserter(stringSuffixes), [](const std::string& str) {
        return std::make_tuple(str, buildSuffixMap(str));
    });

    //3 - Greedy solver
    std::string result;
    for(auto outer = stringSuffixes.cbegin(); outer != stringSuffixes.cend();) {
        auto [outerStr, outerSuffixMap] = *outer;

        std::string matchPrefix;
        int maxSuffix = 0;
        std::string matchSuffix;
        int maxPrefix = 0;

        for (auto inner = outer+1; inner != stringSuffixes.cend(); ++inner ) {
            auto [innerStr, innerSuffixMap] = *inner;
            //std::cout << "Outer is: " << outerStr << ", inner is: " << innerStr << std::endl;
            if (maxSuffix > innerStr.size() || maxPrefix > innerStr.size()) {
                //No reason to continue, as all following words are shorter.
                break;
            }
            int suffix = longestSuffix(innerStr, outerStr, outerSuffixMap);
            if (suffix > maxSuffix) {
                maxSuffix = suffix;
                matchPrefix = innerStr;
            }

            //Try opposite direction
            int prefix = longestSuffix(outerStr, innerStr, innerSuffixMap);
            if (prefix > maxPrefix) {
                maxPrefix = prefix;
                matchSuffix = innerStr;
            }
        }

        if (maxSuffix > 0 && maxSuffix > maxPrefix) {
            std::cout << "Best prefix for " << outerStr << " is a " << matchPrefix << " with " << maxSuffix << " common characters" << std::endl;
            std::cout << "Leftover of matched string is: " << std::string(matchPrefix.begin()+maxSuffix, matchPrefix.end()) << std::endl;

            result += outerStr;
            result.append(matchPrefix.begin()+maxSuffix, matchPrefix.end());
            stringSuffixes.erase(std::remove_if(stringSuffixes.begin(), stringSuffixes.end(), [&matchPrefix](auto item) { return std::get<0>(item) == matchPrefix; }), stringSuffixes.end());
        } else if (maxPrefix > 0) {
            std::cout << "Best suffix for " << outerStr << " is a " << matchSuffix << " with " << maxPrefix << " common characters" << std::endl;
            std::cout << "Leftover of matched string is: " << std::string(outerStr.begin()+maxPrefix, outerStr.end()) << std::endl;

            result += matchSuffix;
            result.append(outerStr.begin()+maxPrefix, outerStr.end());
            stringSuffixes.erase(std::remove_if(stringSuffixes.begin(), stringSuffixes.end(), [&matchSuffix](auto item) { return std::get<0>(item) == matchSuffix; }), stringSuffixes.end());
        } else {
            result += outerStr;
        }

        ++outer;
    }

    std::cout << "Super string length is " << result.size() << std::endl;
    std::cout << "Super string is " << result << std::endl;
    return 0;
}