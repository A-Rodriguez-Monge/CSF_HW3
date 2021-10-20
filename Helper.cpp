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

  printf("total hits: %d\n", cache->loadHits);
  
  std::string curLine;
  getline(cin, curLine);

  if (curLine.empty()) {
    return EOF;
  }

  char cur[curLine.length() + 1];
  strcpy(cur, curLine.c_str());

  char *action = strtok(cur, " ");
  char *address = strtok(NULL, " ");
  
  std::cout<<*action<<" "<<address<<"\n";


  printf("READLINE: \n");
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
  printf("HITORMISS: \n");
  
  printf("strlen(action): %ld\t action: %s\n", strlen(action), action);
  printf("strlen(address): %ld\t address: %s\n", strlen(address), address);

  //convert address to hex
  
  if (strcmp(action, "l") == 0) {
    cache->totalLoads = cache->totalLoads + 1;
  } else if(strcmp(action, "s") == 0) {
    cache->totalStores = cache->totalStores + 1;
  }
  
  printf("total stores: %d\n", cache->totalStores);
  printf("total loads: %d\n", cache->totalLoads);
  printf("store hits: %d\n", cache->storeHits);
  printf("load hits: %d\n", cache->loadHits);

  unsigned convAddress = strtoul(address, NULL, 16);
  unsigned numIndexBits = cache->numIndexBits;
  unsigned numOffsetBits = cache->numOffsetBits;
  
  printf("convAddress: %x\n", convAddress);

  printf("indexBits %u\toffsetBits %u\n", numIndexBits, numOffsetBits);
  
  unsigned tag = getTag(convAddress, numIndexBits, numOffsetBits);
  unsigned index = getIndex(convAddress >> numOffsetBits, numIndexBits);
  
  printf("tag: %x\n", tag);
  printf("index: %x\n\n", index);

  //check if hit or miss
  bool hasBlock = findBlock(cache, tag, index);
  
  if (strcmp(action, "l") == 0) {
    if (findBlock){ //load hits
      cache->loadHits = cache->loadHits + 1;
    } else{ //load misses
       cache->loadMisses = cache->loadMisses + 1;
    }
    loadFunc(cache, tag, index);
  } else if(strcmp(action, "s") == 0) {
    if (findBlock){ //store hits
      cache->storeHits = cache->storeHits + 1;
    } else{ //store  misses
      cache->storeMisses = cache->storeMisses + 1;
    }
    //   storeFunc(cache, tag, index);
  }

  
}

unsigned getTag(unsigned address, unsigned numIndexBits, unsigned numOffsetBits) {
  return address >> (numIndexBits + numOffsetBits);
}

unsigned getIndex(unsigned address, unsigned numIndexBits) {
  unsigned n = 32 - numIndexBits;
  return (address << n) >> n;
}

bool findBlock(Cache *cache, unsigned tag, unsigned index){
  Set currSet = cache->sets.at(index);
  if (currSet.numBlocks == 0){
    return false;
  }
  for(Block &b :currSet.blocks) {
    if(b.tag == tag){
      return true;
    }
  }
  return false;
}

void loadFunc(Cache *cache, unsigned tag, unsigned index){
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
}

void updateTime(Set currSet, char* tFormat, Block currBlock){
  std::vector<Block>::iterator it;
  if (strcmp(tFormat, "lru") == 0){
    currSet.blocks.push_back(currBlock);
  } else if (strcmp(tFormat, "fifo") == 0){
    currSet.blocks.insert(it, 0, currBlock);
  }
  


} 

  



