Download our Makefile (https://gitlab.eecs.umich.edu/eecs281/makefile),
and you can use the commands "make alltests" or "make testPQ" to try out
the priority queue tester.  Look at all of the TODO comments inside of
testPQ.cpp, and update that file to do more testing than it starts with.

There are two versions of the Unordered priority queue: UnorderedPQ.h and
UnorderedFastPQ.h.  The 'fast' version uses a mutable member variable to
change the majority of calls to pop() from O(n) to O(1).  Both versions
are complete, and can be used as an example of how to use this->compare.
