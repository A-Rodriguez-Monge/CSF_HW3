//Alejandro Rodriguez - arodri75
//Pamela Li           - pli36

#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <math.h>
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
  int result = checkArgs(argc, argv);

  if (result != 0) {
    cerr << "Invalid Command Line Argument(s)";
    exit(1);
  }
  
  Cache* cache = new Cache;  
  cache->sets.reserve(atoi(argv[1])); //numSets

  for (int i = 0; i < atoi(argv[1]); i++) {
    Set tempSet;
    cache->sets.push_back(tempSet);
  }
    
  cache->numBlocks = atoi(argv[2]);
  cache->blockBytes = atoi(argv[3]);
  cache->numIndexBits = log2(cache->sets.capacity());
  cache->numOffsetBits = log2(cache->blockBytes);
  cache->missPolicy = argv[4];
  cache->writePolicy = argv[5];
  cache->evictPolicy = argv[6];

  int read = 0;
  while (read != -1) {
    read = processLine(cache);
  }
  printCache(cache);
  delete cache;
  exit(0);
}
