#ifndef _HELPER_H
#define _HELPER_H

#include <vector>

using std::string;
using std::vector;


typedef struct{
  unsigned tag;
  unsigned loadTime;
  unsigned accessTime;
  bool isDirty = false; 
} Block;

typedef struct{
  vector<Block> blocks;
  unsigned numBlocks; //not sure how to access and if we need it
} Set;

typedef struct{

  unsigned numBlocks = 0;
  unsigned numIndexBits = 0;
  unsigned numOffsetBits = 0;
  
  vector<Set> sets;
  unsigned totalLoads = 0;
  unsigned totalStores = 0;
  unsigned loadHits = 0;
  unsigned loadMisses = 0;
  unsigned storeHits  = 0;
  unsigned storeMisses = 0;
  unsigned totalCycles = 0;
} Cache;

int readLine(Cache* cache);//, char* action, char* address);

void hitOrMiss(Cache* cache, char* action, char* address);

unsigned getTag(unsigned address, unsigned numIndexBits, unsigned numOffsetBits);

unsigned getIndex(unsigned Address, unsigned numIndexBits);

int checkArgs(int argc, char **argv);

#endif
