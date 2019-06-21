#include "suffix_array.h"

// SuffixTree builder is based on Akash Kumar work

namespace {
    struct suffix {
        unsigned int index; //Original suffix index
        int rank[2]; //Rank value and next rank
    };

// A comparison function used by sort() to compare two suffixes
// Compares two pairs, returns 1 if first pair is smaller
    int rankCmp(struct suffix a, struct suffix b) {
        return (a.rank[0] == b.rank[0])? (a.rank[1] < b.rank[1] ?1: 0):
               (a.rank[0] < b.rank[0] ?1: 0);
    }
}

// This is the main function that takes a string 'txt' of size n as an
// argument, builds and return the suffix array for the given string
std::vector<int> buildSuffixArray(const std::string& str) {
    auto len = str.size();
    // A structure to store suffixes and their indexes
    struct suffix suffixes[str.size()];

    // Store suffixes and their indexes in an array of structures.
    // The structure is needed to sort the suffixes alphabatically
    // and maintain their old indexes while sorting
    for (int i = 0; i < len; i++)
    {
        suffixes[i].index = i;
        suffixes[i].rank[0] = str[i] - 'a';
        suffixes[i].rank[1] = ((i+1) < len)? (str[i + 1] - 'a'): -1;
    }

    // Sort the suffixes using the comparison function
    // defined above.
    std::sort(suffixes, suffixes+len, rankCmp);

    // At this point, all suffixes are sorted according to first
    // 2 characters.  Let us sort suffixes according to first 4
    // characters, then first 8 and so on
    int ind[len];  // This array is needed to get the index in suffixes[]
    // from original index.  This mapping is needed to get
    // next suffix.
    for (int k = 4; k < 2*len; k = k*2)
    {
        // Assigning rank and index values to first suffix
        int rank = 0;
        int prev_rank = suffixes[0].rank[0];
        suffixes[0].rank[0] = rank;
        ind[suffixes[0].index] = 0;

        // Assigning rank to suffixes
        for (int i = 1; i < len; i++)
        {
            // If first rank and next ranks are same as that of previous
            // suffix in array, assign the same new rank to this suffix
            if (suffixes[i].rank[0] == prev_rank &&
                suffixes[i].rank[1] == suffixes[i-1].rank[1])
            {
                prev_rank = suffixes[i].rank[0];
                suffixes[i].rank[0] = rank;
            }
            else // Otherwise increment rank and assign
            {
                prev_rank = suffixes[i].rank[0];
                suffixes[i].rank[0] = ++rank;
            }
            ind[suffixes[i].index] = i;
        }

        // Assign next rank to every suffix
        for (int i = 0; i < len; i++)
        {
            int nextindex = suffixes[i].index + k/2;
            suffixes[i].rank[1] = (nextindex < len)?
                                  suffixes[ind[nextindex]].rank[0]: -1;
        }

        // Sort the suffixes according to first k characters
        std::sort(suffixes, suffixes+len, rankCmp);
    }

    // Store indexes of all sorted suffixes in the suffix array
    auto result = std::vector<int>(len);
    for (int i = 0; i < len; i++)
        result[i] = suffixes[i].index;

    return  result;
}

t_SuffixMap buildSuffixMap(const std::string& str) {
    t_SuffixMap result;

    for(int indx = 0; indx < str.size(); ++indx) {
        result[str.at(indx)].push_back(indx);
    }

    return result;
}