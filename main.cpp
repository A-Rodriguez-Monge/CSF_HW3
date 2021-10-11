//Alejandro Rodriguez - arodri75
//Pamela Li           - pli36

#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using std::cerr;
using std::isdigit;

/*
  1- number of sets in the cache (a positive power 2)
  2- number of blocks in each set (a positive power of 2)
  3- number of bytes in each block (a positive power of 2, at least 4)
  4- write-allocate or no-write-allocate
  5- write-through or write-back
  6- lru (least recently used) or fifo evictions
*/


int main(int argc, char **argv) {

  //check numb args
  if (argc != 7) {
    cerr << "Invalid Input: Incorrect Number of Arguments\n";
    exit(1);
  }
  
  for (int i = 1; i < 4; i++) {
    if((isdigit(argv[i] != 0) || (atoi(argv[i]) <= 0))) {
      cerr << "Invalid Input: values must be positive\n";
      exit(1);
    }
    
    if ((i == 3) && (atoi(argv[i]) < 4)) {
      cerr << "Invalid Input: number of bytes must be at least 4\n";
      exit(1);
    }
  }

  int numSets = atoi(argv[1]);
  int numBlocks = atoi(argv[2]);
  int numBytes = atoi(argv[3]);
  
  return 0;
}
