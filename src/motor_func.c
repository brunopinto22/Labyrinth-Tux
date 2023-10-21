#include "motor_func.h"

char error[MAX_STRING] = "No Error yet lol";


char* getError(){ return error; }


bool ini(int* fd, envVariables* gameSettings){

  // variaveis de ambiente

  char* env = getenv(TIMER);
  if(env != NULL){
    if(sscanf(env, "%d", &gameSettings->timer) != 1){
      sprintf(error, "%sERRO - a variavel de ambiente %s tem um valor nao nao int%s\n", C_FATAL_ERROR, TIMER,C_CLEAR);
		  return false;
    }
  } else {
    sprintf(error, "%sERRO - Nao foi possivel encontrar a variavel de ambiente %s%s\n", C_FATAL_ERROR, TIMER,C_CLEAR);
		return false;
  }

  env = getenv(TIME_REG);
  if(env != NULL){
    if(sscanf(env, "%d", &gameSettings->reg_time) != 1){
      sprintf(error, "%sERRO - a variavel de ambiente %s tem um valor nao nao int%s\n", C_FATAL_ERROR, TIME_REG,C_CLEAR);
		  return false;
    }
  } else {
    sprintf(error, "%sERRO - Nao foi possivel encontrar a variavel de ambiente %s%s\n", C_FATAL_ERROR, TIME_REG,C_CLEAR);
		return false;
  }

  env = getenv(TIMER_DECREMENT);
  if(env != NULL){
    if(sscanf(env, "%d", &gameSettings->timer_dc) != 1){
      sprintf(error, "%sERRO - a variavel de ambiente %s tem um valor nao nao int%s\n", C_FATAL_ERROR, TIMER_DECREMENT,C_CLEAR);
		  return false;
    }
  } else {
    sprintf(error, "%sERRO - Nao foi possivel encontrar a variavel de ambiente %s%s\n", C_FATAL_ERROR, TIMER_DECREMENT,C_CLEAR);
		return false;
  }

  env = getenv(MIN_USERS);
  if(env != NULL){
    if(sscanf(env, "%d", &gameSettings->min_players) != 1){
      sprintf(error, "%sERRO - a variavel de ambiente %s tem um valor nao nao int%s\n", C_FATAL_ERROR, MIN_USERS,C_CLEAR);
		  return false;
    }
    } else {
      sprintf(error, "%sERRO - Nao foi possivel encontrar a variavel de ambiente %s%s\n", C_FATAL_ERROR, MIN_USERS,C_CLEAR);
		return false;
  }
  

  // abertura do fifo do backend para leitura
	if (mkfifo(MOTOR_FIFO, 0666) == -1) {
		if (errno == EEXIST)
      sprintf(error, "%sERRO - Servidor em execucao%s", C_FATAL_ERROR, C_CLEAR);
    else
      sprintf(error, "%sERRO - Nao foi possivel criar o FIFO%s", C_FATAL_ERROR, C_CLEAR);
		return false;
	}

	*fd = open(MOTOR_FIFO, O_RDWR);
	if (*fd == -1) {
    sprintf(error, "%sERRO - Nao foi possivel abrir o FIFO%s", C_FATAL_ERROR, C_CLEAR);
		return false;
	}

  return true;
}

bool closeMotor(int* fd){
  
  close(*fd);
	unlink(MOTOR_FIFO);

  return true;
}


int checkCMD(prompt* prmt){

  if(strcmp(prmt->command, "users") == 0){

    if(strcmp(prmt->args, "") == 0)
      return USERS;
    sprintf(error, "%serro de formatacao: %susers%s", C_ERROR, C_FERROR, C_CLEAR);
    return CMD_ERROR;

  } else if(strcmp(prmt->command, "kick") == 0){

    if(strcmp(prmt->args, "") != 0)
      return KICK;
    sprintf(error, "%serro de formatacao: %skick%s", C_ERROR, C_FERROR, C_CLEAR);
    return CMD_ERROR;

  } else if(strcmp(prmt->command, "bots") == 0){

    if(strcmp(prmt->args, "") == 0)
      return BOTS;
    sprintf(error, "%serro de formatacao: %sbots%s", C_ERROR, C_FERROR, C_CLEAR);
    return CMD_ERROR;

  } else if(strcmp(prmt->command, "bmov") == 0){

    if(strcmp(prmt->args, "") == 0)
      return BMOV;
    sprintf(error, "%serro de formatacao: %sbmov%s", C_ERROR, C_FERROR, C_CLEAR);
    return CMD_ERROR;

  } else if(strcmp(prmt->command, "rbm") == 0){

    if(strcmp(prmt->args, "") == 0)
      return RBM;
    sprintf(error, "%serro de formatacao: %srbm%s", C_ERROR, C_FERROR, C_CLEAR);
    return CMD_ERROR;

  } else if(strcmp(prmt->command, "begin") == 0){

    if(strcmp(prmt->args, "") == 0)
      return BEGIN;
    sprintf(error, "%serro de formatacao: %sbegin%s", C_ERROR, C_FERROR, C_CLEAR);
    return CMD_ERROR;

  } else if(strcmp(prmt->command, "settings") == 0){

    if(strcmp(prmt->args, "") == 0)
      return SETTINGS;
    sprintf(error, "%serro de formatacao: %ssettings%s", C_ERROR, C_FERROR, C_CLEAR);
    return CMD_ERROR;

  } else if(strcmp(prmt->command, "help") == 0){

    if(strcmp(prmt->args, "") == 0)
      return HELP;
    sprintf(error, "%serro de formatacao: %shelp%s", C_ERROR, C_FERROR, C_CLEAR);
    return CMD_ERROR;

  } else if(strcmp(prmt->command, "end") == 0){

    if(strcmp(prmt->args, "") == 0)
      return END;
    sprintf(error, "%serro de formatacao: %sexit%s", C_ERROR, C_FERROR, C_CLEAR);
    return CMD_ERROR;

  } else {
    sprintf(error, "%serro de formatacao: %so comando nao existe%s", C_ERROR, C_FERROR, C_CLEAR);
    return CMD_ERROR;

  }

}


int checkCMD_UI(prompt* prmt){

  if(strcmp(prmt->command, "up") == 0){

    if(strcmp(prmt->args, "") == 0)
      return UP;
    return CMD_ERROR;

  } else if(strcmp(prmt->command, "down") == 0){

    if(strcmp(prmt->args, "") == 0)
      return DOWN;
    return CMD_ERROR;

  } else if(strcmp(prmt->command, "right") == 0){

    if(strcmp(prmt->args, "") == 0)
      return RIGHT;
    return CMD_ERROR;

  } else if(strcmp(prmt->command, "left") == 0){

    if(strcmp(prmt->args, "") == 0)
      return LEFT;
    return CMD_ERROR;

  } else if(strcmp(prmt->command, "msg") == 0){

    char pl[MAX_STRING], msg[MAX_STRING];

    if(sscanf(prmt->args, "%s %[^\n]", pl, msg) > 0);
      return MSG;
    return CMD_ERROR;

  } else if(strcmp(prmt->command, "exit") == 0){

    if(strcmp(prmt->args, "") == 0)
      return EXIT;
    return CMD_ERROR;

  } else 
    return CMD_ERROR; 

}

void printSettings(envVariables* gameSettings){
  printf("\nDuracao de cada nivel: %d\nDecremento: %d\nTempo de Registo: %d\nMinimo de Jogadores: %d\n", gameSettings->timer, gameSettings->timer_dc, gameSettings->reg_time, gameSettings->min_players);
}

void printHelp(){
  printf("\nusers - imprime a lista de todos os jogadores na plantaforma\nkick - expulsa um jogador\nbots - lista os bots\nbmov - insere um bloqueio movel\nrbm - remove um bloqueio movel\nbegin - comeca o jogo\nsettings - mostra as definicoes do jogo\nend - fecha o programa\n");
}