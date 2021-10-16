#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string.h>
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

int readLine(Cache* cache, char* action, char* address){
  std::string store;
  cin>>action;
  //std::cout<<action<<" ";
  /*while (true){
    cin>>action;
    std::cout<<action<<"\n";
  }
  */

  if (strcmp(action, "l") == 0 || strcmp(action, "s") == 0){
    //  cin>>action;
    cin>>address;
    cin>>store;
    std::cout<<action<<" "<<address<<" "<<store<<"\n";
    return 1;
  }
  return EOF;
  
}




