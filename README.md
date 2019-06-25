# Shortest superstring problem solver

Informally shortest superstring problem can be described as 

> Given a set of words, pack them into the shortest possible string so that all words are still referencible by offset & length. 

It was shown by [Esko Ukkonen](http://www.sciencedirect.com/science/article/pii/030439758190075X) that this problem is NP-complete and therefore it can't be solved in a polynomial time on a determenistic Turing machine, i.e common PC.
That makes problem harder, as naive approach will not work and finding perfect solution may not be achievable. Because of that
it is usually advisable to find not the best one answer, but approximately best and use it in practice as best one.  

## Solutions

Generally speaking, there are two huge groups of possible approaches for finding approximate solution for NP problems: best-effort and probabilistic.
With best effort you try to use most promising input data first and hope for the best. It may not give you result even close to the best,
but will give you some not so bad result in a reasonable amount of time. On the other hand, probabilistic solutions are
using stochastic approach to move through possible solutions space in the direction of a (local) extremum. They are, usually,
slower, but can give you a better result.

This repository includes two solutions, written in both ways: greedy algorithm in C++ and genetic algorithm in Scala.

### Greedy algorithm

Shortest superstring problem can be reduced to a Cover set problem if we find best all pair prefixes sufixes (APSP) which
is also a NP-complete problem. But if we combine a APSP and greedy cover set algorithm, we can reduce APSP to O(N^2*(N-1)/2)~O(N^3)
which is not that bad.

I implemented that algorithm using C++ and Boost library

#### Building from source

You will need:

* C++17 compiler (tested with GCC 9)
* CMake
* Boost 1.70

Install all the prerequisites and issue following commands:

```$shell
mkdir build 
cd build
cmake /path/to/the/sources/greedy
make && make test
``` 

In case you would like to see more verbose output, please, configure the app with debug mode during
cmake stage:

```$shell
cmake -DCMAKE_BUILD_TYPE=Debug /path/to/the/sources/greedy
```

#### Running

You need a list of words in a file, one word per line. Some examples are provided within the repository.
To run the solver execute the following command:

```$shell
./greedy <file_with_words>
```

And wait couple of minutes (or hours if the file is too big). You can expect the following result at the end:

```$shell
./greedy ../../short.txt
Loaded 6 words
Dropping contained words
[======================================================================] 100 %
Building suffix map
[======================================================================] 100 %
Making a superstring
[======================================================================] 100 %
Super string length is 29
```

The app will print the length of found super string

### Genetic algorithm

A shortest superstring problem can be reduced to the Traveling Salesman Problem, if we treat words as the cities and
length of a pair of adjacent words as the distance (don't forget to remove the common suffix). One of the best solutions
to the TSP is a genetic algorithm, that make a population of possible strings and starts evolving it, by breeding new strings and
killing worst ones.

I implemented that algorithm using Scala and built-in scala abilities to parallelize processing.

#### Building and running

You well need the [SBT](https://www.scala-sbt.org/) tool to compile and run the application. To run it, issue the following command:

```$shell
sbt "run <path_to_file_with_words>
``` 

It consumes same word files as greedy solution.

The execution time for that solution is much longer, but it gives you better results. For example on a 'words5.txt' it can 
find a solution which is 50 bytes shorter.

You can expect the following output from the solver:

```$shell
>sbt "run ../short.txt"
[info] Running superstring.Genetic ../short.txt
Loaded 6 words
Dropping contained words
Building suffix map
Generation: 20, top score pair is 48, 48, string length is 29
Best string length is 29
```

The app will print the length of found super string

## Authors

* **Denis Chaplygin** - *Initial work* - [akashihi](https://github.com/akashihi)

## License

This project is licensed under the GPLv3 License - see the LICENSE file for details.
