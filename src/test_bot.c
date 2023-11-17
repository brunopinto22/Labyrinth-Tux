#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "global.h"

int main(){
  
  int x,y,t;
  
  srand(time(NULL));
  x=1+rand()%NUM_LINES;
  y=1+rand()%NUM_COLS;
  t=1+rand()%MAX_TIME_BOT;
  
  printf("%d %d %d",x,y,t);
  
}