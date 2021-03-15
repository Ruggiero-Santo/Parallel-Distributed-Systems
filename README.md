# Parallel-Distributed-Systems
Final project done for the Parallel and Distributed Systems: paradigms and models (2019).

Implementation in C++ of the "Divide and Conquer" paradigm using only the standard libraries.
The implemented code gives the possibility to execute in parallel, with a custom number of threads, any problem that can be solved with this paradigm. The problem must be defined by four basic functions that model the problem and the resolution steps:
1. std::vector<inType> Divide(inType) : Divides the problem into two or more simpler sub-problems
2. bool BaseCaseTest(inType) : Checks if the input problem is the base case
3. outType BaseCaseSolution(inType) : Solves the base problem
4. outType ConquerFunc(std::vector<outType>&) : Merges two solutions

Thanks to the use of templates, functions can handle any type in both input and output (must be consistent between functions).

:warning: WARNING
```diff
- If you want to test the code make sure you have enough physical cores otherwise the high number of threads will only produce hoverheads.
```