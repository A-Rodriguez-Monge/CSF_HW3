//Alejandro Rodriguez - arodri75
//Pamela Li           - pli36

#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include "Helper.h"

using std::cerr;
using std::isdigit;
using std::cin;
/*
  1- number of sets in the cache (a positive power 2)
  2- number of blocks in each set (a positive power of 2)
  3- number of bytes in each block (a positive power of 2, at least 4)
  4- write-allocate or no-write-allocate
  5- write-through or write-back
  6- lru (least recently used) or fifo evictions
*/

int main(int argc, char **argv) {
  char action[1];
  char address[8];
  int result = checkArgs(argc, argv);

  if (result != 0) {
    cerr << "Invalid Command Line Argument(s)";
    exit(1);
  }
  
  int numSets = atoi(argv[1]);
  int numBlocks = atoi(argv[2]);
  int numBytes = atoi(argv[3]);

  Cache* cache;
  cache->sets.reserve(numSets);

  /*  for(vector<Set>::iterator it = std::begin(cache->sets); it != std::end(cache->sets); ++it) {
    it.numBlocks = numBlocks;
    it.blocks = Block[numBlocks];
}
  cache->sets.numBlocks = numBlocks;
  */

  int read = 0;

  while (read != EOF) {
    read = readLine(cache, action, address);
  }
  
  
  //checkTextArgs(); //add function to break up main
  //check file name, need to fix : bytes are within sets are within your cache
  //struct within a struct
  //make a function within a struct just adding 100 cycles and not doing anything else
  //not accessing gcc.trace, piping will automatically 
  
  return 0;
}
