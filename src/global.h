#ifndef TP_GLOBAL_H
#define TP_GLOBAL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>
#include <time.h>

#include "constants.h"


// estruturas
typedef struct coordinates coordinates;
struct coordinates{
	int x;
  int y;
};

typedef struct userInfo userInfo;
struct userInfo{
	pid_t pid;
	char name[MAX_STRING];
  coordinates coords;
};

typedef struct prompt prompt;
struct prompt{
	char command[MAX_STRING];
	char args[MAX_STRING];    
};

typedef struct sharedData sharedData;
struct sharedData{
	userInfo user;
	prompt cmd;
};

typedef struct envVariables envVariables;
struct envVariables{
  int reg_time;       // tempo de inscricao
  int min_players;    // num. minimo de jogares
  int timer;          // duracao de cada nivel
  int timer_dc;       // decremento por nivel
};

typedef struct gameLevel gameLevel;
struct gameLevel{
  int level;
  int level_time;
  char map[NUM_COLS][NUM_LINES];
};


#endif //TP_GLOBAL_H