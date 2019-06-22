# The problem

The problem is known as 'Shortest superstring' and was described by 

# The approach

Two ways: greedy solution for Set cover problen and genetic algorithm for TSP 

## Greedy solution

We reduce superstring problem to the set cover problem by finding matching prefixes-suffixes on all pairs and then
finding a shortest string at best effort using greedy algorithm.

All pair suffix-prefix matching is a NP problem on its own, so i'm using greedy algorithm for the APSP too. 
The approach is simple:
 * Strings are sorted by their length
 * We built a map of suffixes for each string, keeping for each character at the string a position, where it is located.
   Complexity of that step is roughly O(s\*n) where s is number of words and n - average length of a word.
 * Then we will run two cycles over words set (inner and outer), checking, that inner word prefix can be a suffix of a outer word
   and leaving only best match pair for each outer word (best match is a longer suffix). At the same step we eliminate
   (inner) words which are contained in the outer words. 
   Suffix matching is implemented using map from the step one - we take first char of the inner word and retrieve all 
   suffixes for it of the outer word. Then for each found suffix we check if inner word begins with it, starting from the
   beginning. As suffix array is sorted, first matched suffix is the best one.
   Matching step complexity is O(n^2) and total complexity is (s^2\*n^2).
   I also tried suffix matching with suffix tree which is faster, but tree preparation plus binary search is slower, comparing
   to that naive aproach.
 * The cycle above includes set cover greedy algorithm. As input strings are sorted by their lengths, we add every string, 
   we are processing and add best prefix-suffix pair for it if any. The paired string is removed at that step. Thus, outer
   cycle is executed once, inner cycle is executed from current outer position to the end, giving us complexity O(N^3/2)).
   
Advantage of that approach is a speed and predicable calculation time. For example, dataset of 370k words can be processed
in about 15 minutes. On the other hand, it doesn't gives a good solution for the problem. 

Presented solution is working good enough, but can be improved: unnecessary string copying could be eliminated, parallel
outer cycle processing can be added. 