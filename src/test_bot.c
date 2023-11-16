#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "global.h"

int main(){
  
  int x,y;
  
  srand(time(NULL));
  x=1+rand()%NUM_LINES;
  y=1+rand()%NUM_COLS;
  
  printf("%d %d",x,y);
  
}