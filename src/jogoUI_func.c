#include "jogoUI_func.h"

char error[MAX_STRING] = "No Error yet lol";
char fifo[MAX_STRING];


char* getError(){ return error; }

bool ini(int* fd, pid_t PID){
  // guardar nome da fifo
  sprintf(fifo, UI_FIFO, PID);

  // abertura do fifo da ui para leitura
	if (mkfifo(fifo, 0666) == -1) {
    sprintf(error, "%sERRO - Nao foi possivel criar o FIFO%s", C_FATAL_ERROR, C_CLEAR);
		return false;
	}

	*fd = open(fifo, O_RDWR);
	if (*fd == -1) {
    sprintf(error, "%sERRO - Nao foi possivel abrir o FIFO%s", C_FATAL_ERROR, C_CLEAR);
    closeUI(fd);
		return false;
	}

  return true;

}

bool closeUI(int* fd){
  close(*fd);
	unlink(fifo);

  return true;
}

int checkCMD(prompt* prmt){

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

  } else if(strcmp(prmt->command, "space") == 0){

    if(strcmp(prmt->args, "") == 0)
      return CHANGE_RD_WR;
    return CMD_ERROR;

  } else if(strcmp(prmt->command, "msg") == 0){

    char pl[MAX_STRING], msg[MAX_STRING];

    if(sscanf(prmt->args, "%s %[^\n]", pl, msg) > 0);
      return MSG;
    return CMD_ERROR;

  } else if(strcmp(prmt->command, "help") == 0){

    if(strcmp(prmt->args, "") == 0)
        return HELP;
    return CMD_ERROR;

  } else if(strcmp(prmt->command, "exit") == 0){

    if(strcmp(prmt->args, "") == 0)
      return EXIT;
    return CMD_ERROR;

  } else 
    return CMD_ERROR; 

}

void printHelp(){
  printf("\n\t%s↑ ≻%s andar para cima", C_MESSAGE, C_CLEAR);
  printf("\n\t%s↓ ≻%s andar para baixo", C_MESSAGE, C_CLEAR);
  printf("\n\t%s→ ≻%s andar para a direita", C_MESSAGE, C_CLEAR);
  printf("\n\t%s← ≻%s andar para a esquerda", C_MESSAGE, C_CLEAR);
  printf("\n\t%sespaco ≻%s mudar para modo de escrita", C_MESSAGE, C_CLEAR);
  printf("\n\t%smsg ≻%s envia uma mensagem para um utilizador", C_MESSAGE, C_CLEAR);
  printf("\n\t%sexit ≻%s fecha o programa\n", C_MESSAGE, C_CLEAR);
}