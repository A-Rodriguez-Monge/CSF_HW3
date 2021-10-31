# CSF_HW3

Alejandro Rodriguez - arodri75
Pamela Li 	    - pli36

Pamela:
- LRU

Alejandro:
- main.cpp
- argument validation
- FIFO implementation

Together:
- data structures

Which cache config is the most effective?

set-associative caches : n sets of m blocks

The most effective caches are those with the most hits and therefore, lowest cycles.
In our case, this means a set-associative cache with write-allocate, write-back, fifo, and
a small number of bytes in each block. By having a large number of sets and lower number of
blocks in each set, we can more quickly reach a block holding an address. This is because
we will be able to quickly find and access an address' set in  O(1) time and quickly find
the address's block which is in O(m) time (m is number of blocks).
