#include <algorithm>
#include <fstream>
#include <iterator>
#include <iostream>
#include <string>
#include <vector>

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
    return 0;
}