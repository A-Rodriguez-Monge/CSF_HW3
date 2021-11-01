#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <cstring>
#include <algorithm>
#include "Helper.h"

using std::cerr;
using std::isdigit;
using std::cin;
using std::find;
using std::vector;

int checkArgs(int argc, char **argv) {
  //check number of args
  if (argc != 7) {
    cerr << "Invalid Input: Incorrect Number of Arguments\n";
    exit(1);
  }
  
  checkArgs1to4(argv);
  checkArgs4to6(argv);
  return 0;
  
}

void checkArgs1to4(char **argv){
  //check args 1-4
  for (int i = 1; i < 4; i++) {
    if((isdigit(argv[i] != 0) || (atoi(argv[i]) <= 0))) {
      cerr << "Invalid Input: values must be positive\n";
      exit(1);
    }
    
    int temp = atoi(argv[i]);
    
    if ((temp & (temp-1)) != 0) {
      cerr << "Invalid Input: Invalid Input: values must be powers of two\n";
      exit(1);
    }
    
    if ((i == 3) && (atoi(argv[i]) < 4)) {
      cerr << "Invalid Input: number of bytes must be at least 4\n";
      exit(1);
    }
  }
}

void checkArgs4to6(char **argv){
   //check args 4-6
  if ((strcmp("write-allocate", argv[4]) != 0) && (strcmp("no-write-allocate", argv[4]) != 0)) {
      cerr << "Invalid Input: 4th argument is invalid\n";
      exit(1);
  }

  if ((strcmp("write-through", argv[5]) !=0) && (strcmp("write-back", argv[5]) != 0)) {
      cerr << "Invalid Input: 5th argument is invalid\n";
      exit(1);
  }

  if ((strcmp("lru", argv[6]) !=0) && (strcmp("fifo", argv[6]) != 0)) {
      cerr << "Invalid Input: 5th argument is invalid - must be lru or  fifo\n";
      exit(1);
  }
  if (strcmp("write-back", argv[5]) == 0) {
    if (strcmp("no-write-allocate", argv[4]) == 0) {
      cerr << "Invalid Combo\n";
      exit(1);
    }
  }
}


int processLine(Cache* cache){
  std::string curLine;
  getline(cin, curLine);

  if (curLine.empty()) {
    return EOF;
  }

  char cur[curLine.length() + 1];
  strcpy(cur, curLine.c_str());

  char* action = strtok(cur, " ");
  char* address = strtok(NULL, " ");
 
  hitOrMiss(cache, action, address);
  
  return 0;
}

void hitOrMiss(Cache* cache, char* action, char* address){
  if (strcmp(action, "l") == 0) {
    cache->totalLoads = cache->totalLoads + 1;
  } else if(strcmp(action, "s") == 0) {
    cache->totalStores = cache->totalStores + 1;
  }

  unsigned convAddress = strtoul(address, NULL, 16);
  unsigned tag = getTag(convAddress, cache->numIndexBits, cache->numOffsetBits);
  unsigned tagIndexAddress = convAddress >> cache->numOffsetBits;
  unsigned index = getIndex(tagIndexAddress, cache->numIndexBits);

  //check if hit or miss
  int blockIdx = findBlock(cache, tag, index);
  Set *currSet = &cache->sets.at(index);
  if (strcmp(action, "l") == 0) { //deals with loads
    loadFunc(cache, blockIdx, currSet, tag);
  }
  else if(strcmp(action, "s") == 0) { //deals with stores
    storeFunc(cache, blockIdx, currSet, tag);
  }
}

unsigned getTag(unsigned address, unsigned numIndexBits, unsigned numOffsetBits) {
  return address >> (numIndexBits + numOffsetBits);
}

unsigned getIndex(unsigned address, unsigned numIndexBits) {
  
  if (numIndexBits == 0) {
    return 0;
  }

  unsigned n = 32 - numIndexBits;
  
  unsigned long long temp = (address << n) >> n;
  
  return temp;
}

int findBlock(Cache *cache, unsigned tag, unsigned index){
  Set &currSet = cache->sets.at(index);
  if (currSet.numBlocks == 0){
    return -1;
  }

  int counter = 0;
  for(Block &b :currSet.blocks) {
    if(b.tag == tag){
      return counter;
    }
    counter++;
  }
  return -1;
}

void addBlock(Set *currSet, char* tFormat, Block *currBlock){
  currSet->numBlocks++;
  if (strcmp(tFormat, "lru") == 0){
    currSet->blocks.push_back(*currBlock); //last used is in the front
  } else if (strcmp(tFormat, "fifo") == 0){ //oldest is the back
    
    currSet->blocks.insert(currSet->blocks.begin(), 1, *currBlock);
  }
} 

 void evictBlock(Set *currSet, char* tFormat, Cache* cache){
   if (strcmp(tFormat, "lru") == 0){
     if (currSet->blocks.at(0).isDirty){ //check last one
       cache->totalCycles += (cache->blockBytes)/4 * 100;
     }
     currSet->blocks.erase(currSet->blocks.begin()); 
   } else if (strcmp(tFormat, "fifo") == 0){ //the back was the first in
     if (currSet->blocks.at(cache->numBlocks - 1).isDirty){ //check last one
       cache->totalCycles += (cache->blockBytes)/4 * 100;
     }
     currSet->blocks.pop_back();
   }
   cache->totalCycles++;
   currSet->numBlocks--;
 }

void updateTime(Set *currSet, Cache *cache, int blockIdx){
  Block tempBlock =  currSet->blocks.at(blockIdx);
  if (strcmp(cache->evictPolicy, "lru") == 0){
    currSet->blocks.erase(currSet->blocks.begin() + blockIdx);
    currSet->blocks.push_back(tempBlock);
   }
}

void loadFunc(Cache* cache, int blockIdx, Set* currSet, unsigned tag){
  if (blockIdx > -1){ //load hits
    cache->loadHits = cache->loadHits + 1;
    cache->totalCycles++;
    updateTime(currSet, cache, blockIdx);
  } else{ //load misses
    
    cache->loadMisses = cache->loadMisses + 1;
    Block newBlock;
    newBlock.tag = tag;
    cache->totalCycles++;
    
    if (currSet->numBlocks >= cache->numBlocks){ //if you can add more blocks
      evictBlock(currSet, cache->evictPolicy, cache);
    }
    addBlock(currSet, cache->evictPolicy, &newBlock);
    cache->totalCycles += (cache->blockBytes)/4 * 100; // make 100 a global var
  }
}

void storeFunc(Cache* cache, int blockIdx, Set* currSet, unsigned tag){
  if (blockIdx >-1){ //store hits
    
    cache->storeHits = cache->storeHits + 1;
    storeHitFunc(currSet, cache, blockIdx);
    cache->totalCycles++;
  } else{ //store  misses
    
    cache->storeMisses = cache->storeMisses + 1;
    storeMissFunc(currSet, cache, tag);
  }
}


void storeHitFunc(Set *currSet, Cache *cache, int blockIdx){
  if ((strcmp(cache->writePolicy, "write-through") == 0)&& (strcmp(cache->missPolicy, "write-allocate") == 0)){
    cache->totalCycles += 100;
  }else if ((strcmp(cache->writePolicy, "write-through") == 0)&& (strcmp(cache->missPolicy, "no-write-allocate") == 0)){
    cache->totalCycles += 100;
  }else { // write-back and write-allocate
    currSet->blocks.at(blockIdx).isDirty = true;
  }
  updateTime(currSet, cache, blockIdx);
}

void storeMissFunc(Set *currSet, Cache *cache, unsigned tag){
  Block newBlock;
  newBlock.tag = tag;
      
  if ((strcmp(cache->writePolicy, "write-through") == 0)&& (strcmp(cache->missPolicy, "write-allocate") == 0)){
    if (currSet->numBlocks >= cache->numBlocks){ //if you can add more blocks
      evictBlock(currSet, cache->evictPolicy, cache);
    }
    addBlock(currSet, cache->evictPolicy, &newBlock);
    cache->totalCycles += 100;
    cache->totalCycles++;
    cache->totalCycles += (cache->blockBytes)/4 * 100;
  }else if ((strcmp(cache->writePolicy, "write-back") == 0)&& (strcmp(cache->missPolicy, "write-allocate") == 0)){
    if (currSet->numBlocks >= cache->numBlocks){ //if you can add more blocks
      evictBlock(currSet, cache->evictPolicy, cache);
    }
    cache->totalCycles++;
    cache->totalCycles += (cache->blockBytes)/4 * 100;
    addBlock(currSet, cache->evictPolicy, &newBlock);
  }else { // write-back and write-allocate
    cache->totalCycles += 100;
  }
}

void printCache(Cache *cache){
  std::cout<<"Total loads: "<<cache->totalLoads<<std::endl;
  std::cout<<"Total stores: "<<cache->totalStores<<std::endl;
  std::cout<<"Load hits: "<<cache->loadHits<<std::endl;
  std::cout<<"Load misses: "<<cache->loadMisses<<std::endl;
  std::cout<<"Store hits: "<<cache->storeHits<<std::endl;
  std::cout<<"Store misses: "<<cache->storeMisses<<std::endl;
  std::cout<<"Total cycles: "<<cache->totalCycles<<std::endl;
}
