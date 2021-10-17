#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <cstring>
#include "Helper.h"

using std::cerr;
using std::isdigit;
using std::cin;

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
  printf("hitOrMiss: \n");

  printf("total hits: %d\n", cache->loadHits);
  
  printf("strlen(action): %ld\t action: %s\n", strlen(action), action);
  printf("strlen(address): %ld\t address: %s\n", strlen(address), address);
  
  if (strcmp(action, "l") == 0) {
    cache->totalLoads = cache->totalLoads + 1;
  } else if(strcmp(action, "s") == 0) {
    cache->totalStores = cache->totalStores + 1;
    
  }
}



