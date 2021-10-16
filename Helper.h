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
}Block;

typedef struct{
  Block* blocks;
  unsigned numBlocks;
} Set;

typedef struct{
  vector<Set> sets;
  unsigned totalLoads = 0;
  unsigned totalStores = 0;
  unsigned loadHits = 0;
  unsigned loadMisses = 0;
  unsigned storeHits  = 0;
  unsigned storeMisses = 0;
  unsigned totalCycles = 0;
} Cache;

int readLine(Cache* cache, char* action, char* address);



int checkArgs(int argc, char **argv);

#endif
