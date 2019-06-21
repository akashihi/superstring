#include <algorithm>
#include <fstream>
#include <iterator>
#include <iostream>
#include <string>
#include <vector>

#include "suffix_array.h"

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
    std::cout << "Loaded " << inputStrings.size() << " words" << std::endl;

    //1 - Calculate suffix map for each dataset entry
    std::vector<t_SuffixMap> stringSuffixes;
    std::transform(std::begin(inputStrings), std::end(inputStrings), std::back_inserter(stringSuffixes), [](const std::string& str) {
        return buildSuffixMap(str);
    });
    return 0;
}