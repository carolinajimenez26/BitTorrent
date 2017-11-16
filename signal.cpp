// Taken from https://stackoverflow.com/questions/1641182/how-can-i-catch-a-ctrl-c-event-c


#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h> //  our new library 
using namespace std;

volatile sig_atomic_t flag = 0;

void my_function(int sig){ // can be called asynchronously
  flag = 1; // set flag
}

int main(){
  // Register signals 
  signal(SIGINT, my_function); 
  //      ^          ^
  //  Which-Signal   |-- which user defined function registered
  while(1) { 
    if(flag){ // my action when signal set it 1
        cout << SIGINT << endl; 
        if (SIGINT == 2) {
          printf("OUT presing ctrl+c\n");
        	exit(1); 
        }
        flag = 0;
    }
   }     
  return 0;
}  