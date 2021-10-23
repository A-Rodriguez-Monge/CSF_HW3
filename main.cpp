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
  char action[]  = "";
  char address[] = "";// = "0x00000000";
  int result = checkArgs(argc, argv);

  if (result != 0) {
    cerr << "Invalid Command Line Argument(s)";
    exit(1);
  }
  
  int numSets = atoi(argv[1]);
  int numBlocks = atoi(argv[2]);
  int blockSize = atoi(argv[3]);

  unsigned numIndexBits = log2(numSets);
  // printf("numIndexBits: %d\n", numIndexBits);

  unsigned numOffsetBits = log2(blockSize);

  Cache* cache = new Cache;  
  cache->sets.reserve(numSets);
  //printf("About to initialize the sets\n");
  for (int i = 0; i < numSets; i++) {
    Set tempSet;
    
    /* for (int j = 0; j < numBlocks; j++) {
      Block tempBlock;
      tempSet.blocks.push_back(tempBlock);
      }*/
    cache->sets.push_back(tempSet);
  }
    
  cache->numBlocks = numBlocks;
  cache->blockBytes = blockSize;
  //printf("numBlocks: %d\n", cache->numBlocks);
  cache->numIndexBits = numIndexBits;
  cache->numOffsetBits = numOffsetBits;
  cache->missPolicy = argv[4];
  cache->writePolicy = argv[5];
  cache->evictPolicy = argv[6];
   
  //printf("load hits: %d\n", cache->loadHits);

  
  /*  for(vector<Set>::iterator it = std::begin(cache->sets); it != std::end(cache->sets); ++it) {
    it.numBlocks = numBlocks;
    it.blocks = Block[numBlocks];
}
  cache->sets.numBlocks = numBlocks;
  */

  int read = 0;


  //printf("MAIN: total hits: %u\n", cache->totalLoads);
  //printf("About to read lines\n");

   while (read != -1) {
     read = readLine(cache);//, action, address);
   }
  
  
  //checkTextArgs(); //add function to break up main
  //check file name, need to fix : bytes are within sets are within your cache
  //struct within a struct
  //make a function within a struct just adding 100 cycles and not doing anything else
  //not accessing gcc.trace, piping will automatically 
   printCache(cache);
   delete cache;
   exit(0);
}
