#include "motor_func.h"

char error[MAX_STRING] = "No Error yet lol";


char* getError(){ return error; }


bool ini(int* fd){
  // abertura do fifo do backend para leitura
	if (mkfifo(MOTOR_FIFO, 0666) == -1) {
		if (errno == EEXIST)
      sprintf(error, "%sERRO - Servidor em execucao%s", C_ERROR, C_CLEAR);
    else
      sprintf(error, "%sERRO - Nao foi possivel criar o FIFO%s", C_ERROR, C_CLEAR);
		return false;
	}

	*fd = open(MOTOR_FIFO, O_RDWR);
	if (*fd == -1) {
    sprintf(error, "%sERRO - Nao foi possivel abrir o FIFO%s", C_ERROR, C_CLEAR);
		return false;
	}

  return true;
}


int checkCMD(prompt* prmt){

  if(strcmp(prmt->command, "users") == 0){

  } else if(strcmp(prmt->command, "kick") == 0){

  } else if(strcmp(prmt->command, "bots") == 0){

  } else if(strcmp(prmt->command, "bmov") == 0){

  } else if(strcmp(prmt->command, "rbm") == 0){

  } else if(strcmp(prmt->command, "begin") == 0){

  } else if(strcmp(prmt->command, "end") == 0){
    if(strcmp(prmt->args, "") == 0)
      return END;
    sprintf(error, "%serro de formatacao - %sexit%s", C_ERROR, C_FERROR, C_CLEAR);
    return CMD_ERROR;
  } else
    return CMD_ERROR;

}