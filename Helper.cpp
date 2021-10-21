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
  if (strcmp("write-through", argv[5]) == 0) {
    if (strcmp("no-write-allocate", argv[4]) != 0) {
      cerr << "Invalid Combo\n";
      exit(1);
    }
  }

  if (strcmp("write-back", argv[5]) == 0) {
    if (strcmp("write-allocate", argv[4]) != 0) {
      cerr << "Invalid Combo\n";
      exit(1);
    }
  }

  return 0;

}

int readLine(Cache* cache){//, char *action, char* address){

  //printf("total hits: %d\n", cache->loadHits);
  
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
  
  /*
  std::string store;

  getline(cin, store);
  
  if (store.empty()) {
    printf("EMPTY\n");
    return -1;
  }
  
  cin>>action;
  //std::cout<<action<<" ";
  /*while (true){
    cin>>action;
    std::cout<<action<<"\n";
  }
  

  if (strcmp(action, "l") == 0 || strcmp(action, "s") == 0){
    //  cin>>action;   
    //action[1] = 0;
    //printf("before: %s\n", action);
    cin>>address;
    cin>>store;

    //std::cout << "STORE: "<< store << std::endl;
    
    //printf("after: %s\n", action);
    std::cout<<action<<" "<<address<<" "<<store<<"\n";
    return 1;
  }
  
  return -1;
  */
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
  /*
  printf("convAddress: %x\n", convAddress);

  printf("indexBits %u\toffsetBits %u\n", numIndexBits, numOffsetBits);
  */
  unsigned tag = getTag(convAddress, numIndexBits, numOffsetBits);
  unsigned index = getIndex(convAddress >> numOffsetBits, numIndexBits);
  /*
  printf("tag: %x\n", tag);
  printf("index: %x\n", index);
  */
  //check if hit or miss
  int blockIdx = findBlock(cache, tag, index);
  Set *currSet = &cache->sets.at(index);
  if (strcmp(action, "l") == 0) {
    if (blockIdx > -1){ //load hits
      //  printf("LOAD HIT\n");
      cache->loadHits = cache->loadHits + 1;
       cache->totalCycles++;
    } else{ //load misses
      //printf("LOAD MISS\n");

       cache->loadMisses = cache->loadMisses + 1;
        Block newBlock;
	newBlock.tag = tag;
	cache->totalCycles++;

	if (currSet->numBlocks > cache->numBlocks){ //if you can add more blocks
	  evictBlock(currSet, cache->evictPolicy, cache);
	  //  printf("We need to evict a block\n");
	}
	updateTime(currSet, cache->evictPolicy, &newBlock);
	cache->totalCycles += (cache->blockBytes)/4 * 100;
	currSet->numBlocks++;
	//	printf("current size of set: %u\n", currSet->numBlocks);
	

    }
    // loadFunc(cache, tag, index); 
  } else if(strcmp(action, "s") == 0) {
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
  unsigned n = 32 - numIndexBits;
  return (address << n) >> n;
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

void updateTime(Set *currSet, char* tFormat, Block *currBlock){
  //std::vector<Block>::iterator it;
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
   currSet->numBlocks--;
 }

void storeHitFunc(Set *currSet, Cache *cache, int blockIdx){
  if ((strcmp(cache->writePolicy, "write-through") == 0)&& (strcmp(cache->missPolicy, "write-allocate") == 0)){
    cache->totalCycles += 100;
    currSet->blocks.at(blockIdx).isDirty = true;
    cache->totalCycles += (cache->blockBytes)/4 * 100;
  }else if ((strcmp(cache->writePolicy, "write-through") == 0)&& (strcmp(cache->missPolicy, "no_write-allocate") == 0)){
    cache->totalCycles += 100;
    currSet->blocks.at(blockIdx).isDirty = true;
  }else {
    currSet->blocks.at(blockIdx).isDirty = true;
  }
}

void storeMissFunc(Set *currSet, Cache *cache, unsigned tag){
    if ((strcmp(cache->writePolicy, "write-through") == 0)&& (strcmp(cache->mi\
ssPolicy, "write-allocate") == 0)){
    cache->totalCycles += 100;
    // currSet->blocks.at(blockIdx).isDirty = true;
    cache->totalCycles += (cache->blockBytes)/4 * 100;
  }else if ((strcmp(cache->writePolicy, "write-through") == 0)&& (strcmp(cac\
he->missPolicy, "no_write-allocate") == 0)){
    cache->totalCycles += 100;
    //currSet->blocks.at(blockIdx).isDirty = true;
  }else {
      //currSet->blocks.at(blockIdx).isDirty = true;
  }
}

void printCache(Cache *cache){
  std::cout<<"Total loads: "<<cache->totalLoads<<"\n";
  std::cout<<"Total stores: "<<cache->totalStores<<"\n";
  std::cout<<"Loads hits: "<<cache->loadHits<<"\n";
  std::cout<<"Loads misses: "<<cache->loadMisses<<"\n";
  std::cout<<"Store hits: "<<cache->storeHits<<"\n";
  std::cout<<"Store misses: "<<cache->storeMisses<<"\n";
  std::cout<<"Total cycles: "<<cache->totalCycles<<"\n";
}
  



