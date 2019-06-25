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

#ifndef NDEBUG
#define DEBUG(MSG) std::cout << MSG << std::endl;
#else
#define DEBUG(MSG)
#endif

void progress(int current, int total) {
    float progress = static_cast<float>(current)/total;
    int barWidth = 70;
    std::cout << "[";
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << " %\r";
    std::cout.flush();
}

int main(int argc, char** argv) {
    if (argc<2) {
        std::cerr << "Usage: greedy <file_with_strings>" << std::endl;
        return 1;
    }

    //Progress indicator variables
    int totalWords = 0, seenWords = 0;

    //0 - Load the file
    std::ifstream inputFile(argv[1]);
    std::vector<std::string> inputStrings;
    std::copy(std::istream_iterator<std::string>(inputFile),
              std::istream_iterator<std::string>(),
              std::back_inserter(inputStrings));
    boost::sort::pdqsort(std::begin(inputStrings), std::end(inputStrings), [](const std::string& left, const std::string right) { return left.size() > right.size(); });
    std::cout << "Loaded " << inputStrings.size() << " words" << std::endl;

    //1 - Drop shorter words which are contained in a longer ones
    std::cout << "Dropping contained words" << std::endl;
    totalWords = inputStrings.size();
    std::set<std::string> contained;
    for(auto outer = inputStrings.cbegin(); outer != inputStrings.cend(); ++outer) {
        ++seenWords;
        progress(seenWords, totalWords);
        for (auto inner = outer+1; inner != inputStrings.cend(); ++inner ) {
            if (boost::algorithm::contains(*outer, *inner)) {
                DEBUG(*outer << " contains " << *inner)
                contained.insert(*inner);
            }
        }
    }
    inputStrings.erase(std::remove_if(inputStrings.begin(), inputStrings.end(), [&contained](auto item) { return contained.find(item) != contained.end(); }), inputStrings.end());
    std::cout << std::endl;

    //2 - Calculate suffix map for each dataset entry
    std::cout << "Building suffix map" << std::endl;
    totalWords = inputStrings.size();
    seenWords = 0;
    std::vector<t_StringWithSuffix> stringSuffixes;
    std::transform(std::begin(inputStrings), std::end(inputStrings), std::back_inserter(stringSuffixes), [&seenWords, totalWords](const std::string& str) {
        ++seenWords;
        progress(seenWords, totalWords);
        return std::make_tuple(str, buildSuffixMap(str));
    });
    std::cout << std::endl;

    //3 - Greedy solver
    std::cout << "Making a superstring" << std::endl;
    totalWords = stringSuffixes.size();
    seenWords = 0;

    std::string result;
    for(auto outer = stringSuffixes.cbegin(); outer != stringSuffixes.cend();) {
        auto [outerStr, outerSuffixMap] = *outer;

        // Progress bar stuff
        totalWords = stringSuffixes.size();
        ++seenWords;
        progress(seenWords, totalWords);

        //We will keep our best choices here
        std::string matchPrefix;
        int maxSuffix = 0;
        std::string matchSuffix;
        int maxPrefix = 0;

        //Iterate over the words below us and find one with longest matching prefix or sufffix.
        for (auto inner = outer+1; inner != stringSuffixes.cend(); ++inner ) {
            auto [innerStr, innerSuffixMap] = *inner;
            DEBUG("Outer is: " << outerStr << ", inner is: " << innerStr)
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
            DEBUG("Best prefix for " << outerStr << " is a " << matchPrefix << " with " << maxSuffix << " common characters")
            DEBUG("Leftover of matched string is: " << std::string(matchPrefix.begin()+maxSuffix, matchPrefix.end()))

            result += outerStr;
            result.append(matchPrefix.begin()+maxSuffix, matchPrefix.end());
            stringSuffixes.erase(std::remove_if(stringSuffixes.begin(), stringSuffixes.end(), [&matchPrefix](auto item) { return std::get<0>(item) == matchPrefix; }), stringSuffixes.end());
        } else if (maxPrefix > 0) {
            DEBUG("Best suffix for " << outerStr << " is a " << matchSuffix << " with " << maxPrefix << " common characters")
            DEBUG("Leftover of matched string is: " << std::string(outerStr.begin()+maxPrefix, outerStr.end()))

            result += matchSuffix;
            result.append(outerStr.begin()+maxPrefix, outerStr.end());
            stringSuffixes.erase(std::remove_if(stringSuffixes.begin(), stringSuffixes.end(), [&matchSuffix](auto item) { return std::get<0>(item) == matchSuffix; }), stringSuffixes.end());
        } else {
            result += outerStr;
        }

        ++outer;
    }
    std::cout << std::endl;

    std::cout << "Super string length is " << result.size() << std::endl;
    DEBUG("Super string is " << result)
    return 0;
}