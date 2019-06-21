# The problem

The problem is known as 'Shortest superstring' and was described by 

# The approach

Two ways: greedy solution for Set cover problen and genetic algorithm for TSP 

## Greedy solution

We reduce superstring problem to the set cover problem by finding matching prefixes-suffixes on all pairs and then
finding a shoertes string at best effort using greedy algorithm.

All pair suffix-prefix matching is a NP problem on its own, so i'm using greedy algorithm for the APSP too. 
The approach is simple:
 * We built a map of suffixes for each string, keeping for each character at the string a position, where it is located.
   Complexity of that step is roughly O(s\*n) where s is number of words and n - average length of a word.
 * Then we will run two cycles over words set (inner and outer), checking, that inner word prefix can be a suffix of a outer word
   and leaving only best match pair for each outer word (best match is a longer suffix). At the same step we eliminate
   (outer) words which are contained in the inner words. 
   Suffix matching is implemented using map from the step one - we take first char of the inner word and retrieve all 
   suffixes for it of the outer word. Then for each found suffix we check if inner word begins with it, starting from the
   beginning. As suffix array is sorted, first matched suffix is the best one.
   Matching step complexity is O(n^2) and total complexity is (s^2\*n^2).
   I also tried suffix matching with suffix tree which is faster, but tree preparation plus binary search is slower, comparing
   to that naive aproach.
 * Matching pairs are sorted by their suffix length, the best one pair is concatenated to a single word, replacing existing 
   two words and process starts over until there is no pairs left.  To improves that process all words with no pair are 
   excluded from it and stored in a separate array. They will be added after process completion.