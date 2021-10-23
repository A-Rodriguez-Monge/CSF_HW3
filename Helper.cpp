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
  
  //check args 1-4
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

  //validate arg combos
  /*if (strcmp("write-through", argv[5]) == 0) {
    if (strcmp("no-write-allocate", argv[4]) != 0) {
      cerr << "Invalid Combo\n";
      exit(1);
    }
  }*/

    if (strcmp("write-back", argv[5]) == 0) {
      if (strcmp("no-write-allocate", argv[4]) == 0) {
	cerr << "Invalid Combo\n";
	exit(1);
    }
  }

  return 0;

}

int readLine(Cache* cache){//, char *action, char* address){
  
  std::string curLine;
  getline(cin, curLine);

  if (curLine.empty()) {
    return EOF;
  }

  char cur[curLine.length() + 1];
  strcpy(cur, curLine.c_str());

  char *action = strtok(cur, " ");
  char *address = strtok(NULL, " ");
  
  //std::cout<<*action<<" "<<address<<"\n";


  //printf("READLINE: \n");
  //  printf("tag (0,0): %u \n", cache->sets.at(0).blocks.at(0).tag);
  // printf("tag (0,1): %u \n", cache->sets.at(0).blocks.at(1).tag);
  // printf("tag (1,0): %u \n", cache->sets.at(1).blocks.at(0).tag);
  //printf("tag (1,1): %u \n", cache->sets.at(1).blocks.at(1).tag);
  
  hitOrMiss(cache, action, address);
  
  return 0;
}

void hitOrMiss(Cache* cache, char* action, char* address){
  /*  printf("HITORMISS: \n");
  
  printf("strlen(action): %ld\t action: %s\n", strlen(action), action);
  printf("strlen(address): %ld\t address: %s\n", strlen(address), address);
  */
  //convert address to hex
  
  if (strcmp(action, "l") == 0) {
    cache->totalLoads = cache->totalLoads + 1;
  } else if(strcmp(action, "s") == 0) {
    cache->totalStores = cache->totalStores + 1;
  }
  /*
  printf("total stores: %d\n", cache->totalStores);
  printf("total loads: %d\n", cache->totalLoads);
  printf("store hits: %d\n", cache->storeHits);
  printf("load hits: %d\n", cache->loadHits);
  */
  unsigned convAddress = strtoul(address, NULL, 16);
  unsigned numIndexBits = cache->numIndexBits;
  unsigned numOffsetBits = cache->numOffsetBits;
  
  //printf("convAddress: %x\n", convAddress);
  
  //printf("indexBits %u\toffsetBits %u\n", numIndexBits, numOffsetBits);
  
  unsigned tag = getTag(convAddress, numIndexBits, numOffsetBits);
  unsigned tagIndexAddress = convAddress >> numOffsetBits;

  unsigned index = getIndex(tagIndexAddress, numIndexBits);

  /*
  printf("tag: %x\n", tag);
  printf("index: %x\n", index);

  printf("Index (unsigned) %u\n\n", index);
  */
  //check if hit or miss
  int blockIdx = findBlock(cache, tag, index);
  Set *currSet = &cache->sets.at(index);
  if (strcmp(action, "l") == 0) { //deals with loads
    if (blockIdx > -1){ //load hits
      //  printf("LOAD HIT\n");
      cache->loadHits = cache->loadHits + 1;
      cache->totalCycles++;
      updateTime(currSet, cache, blockIdx);
    } else{ //load misses
      //printf("LOAD MISS\n");
      cache->loadMisses = cache->loadMisses + 1; //if set is fully empty, need to populate set. need to access memory to load set
        Block newBlock;
	newBlock.tag = tag;

	//may access memory which means total cycles is different
	cache->totalCycles++;
	/*if (currSet->numBlocks == cache->numBlocks){
	  printf("currSet is max capacity\n"); 
    }*/	
	if (currSet->numBlocks >= cache->numBlocks){ //if you can add more blocks
	  evictBlock(currSet, cache->evictPolicy, cache);
	  //  printf("We need to evict a block\n");
	}
	addBlock(currSet, cache->evictPolicy, &newBlock);
	cache->totalCycles += (cache->blockBytes)/4 * 100; // make 100 a global var
	//currSet->numBlocks++;
	//	printf("current size of set: %u\n", currSet->numBlocks);
    }
    // loadFunc(cache, tag, index); 
  } else if(strcmp(action, "s") == 0) { //deals with stores 
    if (blockIdx >-1){ //store hits
      // printf("STORE HIT\n");
      cache->storeHits = cache->storeHits + 1;
      storeHitFunc(currSet, cache, blockIdx);
      cache->totalCycles++;
    } else{ //store  misses
      //printf("STORE MISSE\n");
      cache->storeMisses = cache->storeMisses + 1;
      storeMissFunc(currSet, cache, tag);
	//cache->totalCycles++;
    }
    //   storeFunc(cache, tag, index);
  }

  //printf("This is the total number of cycles: %u\n\n", cache->totalCycles);
  
}

unsigned getTag(unsigned address, unsigned numIndexBits, unsigned numOffsetBits) {
  return address >> (numIndexBits + numOffsetBits);
}

unsigned getIndex(unsigned address, unsigned numIndexBits) {
  /*
    unsigned address = 2726;//without offset
    unsigned a = 32;
    unsigned index = 2726 & (a-1);
    unsigned tag = address - index >> 5; //5 = log2(a);

  */
  
  if (numIndexBits == 0) {
    return 0;
  }

  unsigned n = 32 - numIndexBits;
  
  unsigned long long temp = (address << n) >> n;
  
  return temp;
}

int findBlock(Cache *cache, unsigned tag, unsigned index){
  Set currSet = cache->sets.at(index);
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

/*void loadFunc(Cache *cache, unsigned tag, unsigned index){
  Set currSet = cache->sets.at(index);
  if (findBlock(cache, tag, index)){
    cache->totalCycles++; 
  } else { //(currSet.numBlocks < cache->numBlocks){ //check if there are still space in set
    Block newBlock;
    newBlock.tag = tag;
    updateTime(currSet, cache->evictPolicy, newBlock);
    cache->totalCycles++;
    //currSet.blocks.push_back(newBlock);
  } 
  }*/

void addBlock(Set *currSet, char* tFormat, Block *currBlock){
  //std::vector<Block>::iterator it;
  currSet->numBlocks++;
  if (strcmp(tFormat, "lru") == 0){
    currSet->blocks.push_back(*currBlock); //last used is in the front
  } else if (strcmp(tFormat, "fifo") == 0){ //oldest is the back 
    currSet->blocks.insert(currSet->blocks.begin(), 0, *currBlock);
  }

} 

 void evictBlock(Set *currSet, char* tFormat, Cache* cache){
   /* if (currSet->blocks.at(blockIdx).isDirty){
     cache->totalCycles += (cache->blockBytes)/4 * 100; 
     }*/
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
     //currSet->blocks.insert(it, 0, *currBlock);
   }
   cache->totalCycles++;
   currSet->numBlocks--;
 }

void updateTime(Set *currSet, Cache *cache, int blockIdx){
  Block tempBlock =  currSet->blocks.at(blockIdx);
  if (strcmp(cache->evictPolicy, "lru") == 0){
    currSet->blocks.erase(currSet->blocks.begin() + blockIdx);
    currSet->blocks.push_back(tempBlock);
   } else if (strcmp(cache->evictPolicy, "fifo") == 0){ //the back was the first in
    /* if (currSet->blocks.at(cache->numBlocks - 1).isDirty){ //check last one
       cache->totalCycles += (cache->blockBytes)/4 * 100;
     }
     currSet->blocks.pop_back();*/
     //currSet->blocks.insert(it, 0, *currBlock);
   }
}

void storeHitFunc(Set *currSet, Cache *cache, int blockIdx){
  if ((strcmp(cache->writePolicy, "write-through") == 0)&& (strcmp(cache->missPolicy, "write-allocate") == 0)){
    cache->totalCycles += 100;
    currSet->blocks.at(blockIdx).isDirty = true;
    //cache->totalCycles += (cache->blockBytes)/4 * 100;
  }else if ((strcmp(cache->writePolicy, "write-through") == 0)&& (strcmp(cache->missPolicy, "no_write-allocate") == 0)){
    cache->totalCycles += 100;
    currSet->blocks.at(blockIdx).isDirty = true;
  }else {
    currSet->blocks.at(blockIdx).isDirty = true;
  }
  updateTime(currSet, cache, blockIdx);
}

void storeMissFunc(Set *currSet, Cache *cache, unsigned tag){
  Block newBlock;
  newBlock.tag = tag;
      
  if ((strcmp(cache->writePolicy, "write-through") == 0)&& (strcmp(cache->missPolicy, "write-allocate") == 0)){
    if (currSet->numBlocks >= cache->numBlocks){ //if you can add more bl \
						ocks
      evictBlock(currSet, cache->evictPolicy, cache);
      //  printf("We need to evict a block\n");
    }
    addBlock(currSet, cache->evictPolicy, &newBlock);
    cache->totalCycles += 100;
    cache->totalCycles++;
    // currSet->blocks.at(blockIdx).isDirty = true;
    cache->totalCycles += (cache->blockBytes)/4 * 100;
  }else if ((strcmp(cache->writePolicy, "write-back") == 0)&& (strcmp(cache->missPolicy, "write-allocate") == 0)){
    if (currSet->numBlocks >= cache->numBlocks){ //if you can add more bl \
                                                ocks
      evictBlock(currSet, cache->evictPolicy, cache);
      //  printf("We need to evict a block\n");
    }
    cache->totalCycles++;
    cache->totalCycles += (cache->blockBytes)/4 * 100; 
    addBlock(currSet, cache->evictPolicy, &newBlock);
    
    //currSet->blocks.at(blockIdx).isDirty = true;
  }else {
    cache->totalCycles += 100;
  }
  //write-through + no-write-allocate

  /*else {
   //currSet->blocks.at(blockIdx).isDirty = true;
   }*/
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
  



