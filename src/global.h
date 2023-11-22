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
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <time.h>

#include "constants.h"


// estruturas
typedef struct gameLevel gameLevel;
struct gameLevel{
  int level;
  int level_time;
  char map[NUM_COLS][NUM_LINES];
};

typedef struct coordinates coordinates;
struct coordinates{
	int x;
  int y;
};

typedef struct score score;
struct score{
  int time;
  char player[MAX_STRING];
};

typedef struct userInfo userInfo;
struct userInfo{
	pid_t pid;
	char name[MAX_STRING];
  coordinates coords;
  bool inGame;
};

typedef struct prompt prompt;
struct prompt{
	char command[MAX_STRING];
	char args[MAX_STRING];    
};

typedef struct sharedData sharedData;
struct sharedData{
  // resultado da operacao
  bool result;
  char error[MAX_ERROR];

	userInfo user;
	prompt cmd;

  // informacao do nivel
  gameLevel level;

  // pontuacoes globais
  bool newScore;
  score latest_score;
};

typedef struct envVariables envVariables;
struct envVariables{
  int reg_time;       // tempo de inscricao
  int min_players;    // num. minimo de jogares
  int timer;          // duracao de cada nivel
  int timer_dc;       // decremento por nivel
};

// funcoes
int sendTo(sharedData data, char* destiny);
// Envia informacao para um fifo indicado
// Retorna      0 se correr tudo bem
//              1 se nao conseguir abrir o fifo
//             -1 se houver algum erro no envio


#endif //TP_GLOBAL_H