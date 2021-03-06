#ifndef _HELPER_H
#define _HELPER_H

#include <vector>

using std::string;
using std::vector;


typedef struct{
  unsigned tag;
  bool isDirty = false; 
} Block;

typedef struct{
  vector<Block> blocks;
  unsigned numBlocks = 0; 
} Set;

typedef struct{
  unsigned numBlocks = 0;
  unsigned numIndexBits = 0;
  unsigned numOffsetBits = 0;
  unsigned blockBytes = 0;
  
  vector<Set> sets;
  unsigned totalLoads = 0;
  unsigned totalStores = 0;
  unsigned loadHits = 0;
  unsigned loadMisses = 0;
  unsigned storeHits  = 0;
  unsigned storeMisses = 0;
  unsigned totalCycles = 0;

  char* missPolicy;
  char* writePolicy;
  char* evictPolicy;
} Cache;

int processLine(Cache* cache);

void hitOrMiss(Cache* cache, char* action, char* address);

unsigned getTag(unsigned address, unsigned numIndexBits, unsigned numOffsetBits);

unsigned getIndex(unsigned Address, unsigned numIndexBits);

int checkArgs(int argc, char **argv);

void checkArgs1to4(char **argv);

void checkArgs4to6(char **argv);

int findBlock(Cache *cache, unsigned tag, unsigned index);

void storeFunc(Cache* cache,int blockIdx, Set* currSet, unsigned tag);

void addBlock(Set *currSet, char* tFormat, Block *currBlock); 

void updateTime(Set *currSet, Cache *cache, int blockIdx);

void evictBlock(Set *currSet, char* tFormat, Cache* cache);

void loadFunc(Cache* cache,int blockIdx, Set* currSet, unsigned tag);

void storeHitFunc(Set *currSet, Cache *cache, int blockIdx);

void storeMissFunc(Set *currSet, Cache *cache, unsigned tag);

void printCache(Cache *cache);

#endif
