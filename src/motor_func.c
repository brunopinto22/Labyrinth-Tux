#include "motor_func.h"

char error[MAX_STRING] = "No Error yet lol";


char* getError(){ return error; }


bool ini(int* fd){
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
    sprintf(error, "%serro de formatacao: %sBEGIN%s", C_ERROR, C_FERROR, C_CLEAR);
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