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
typedef struct userInfo userInfo;
struct userInfo{
	pid_t pid;
	char name[MAX_STRING];
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

#endif //TP_GLOBAL_H