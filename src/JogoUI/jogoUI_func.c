#include "jogoUI_func.h"

char error[MAX_ERROR] = "No Error yet lol";
char fifo[MAX_STRING];


char* getError(){ return error; }

void setError(char* message){ strcpy(error, message); }

bool ini(int* fd, sharedData data){

  // guardar nome da fifo
  sprintf(fifo, UI_FIFO, data.user.pid);

  // criacao do fifo da ui
	if (mkfifo(fifo, 0666) == -1) {
    sprintf(error, "%sERRO - Nao foi possivel criar o FIFO%s", C_FATAL_ERROR, C_CLEAR);
		return false;
	}
  // abertura do fifo da ui para leitura
	*fd = open(fifo, O_RDWR);
	if (*fd == -1) {
    sprintf(error, "%sERRO - Nao foi possivel abrir o FIFO%s", C_FATAL_ERROR, C_CLEAR);
    closeUI(fd, data, false);
		return false;
	}

  // abrir fifo do motor
	int fd_motor = open(MOTOR_FIFO, O_WRONLY);
	if (fd_motor == -1){
    sprintf(error, "%sERRO - o Motor nao esta a correr%s", C_FATAL_ERROR, C_CLEAR);
    closeUI(fd, data, false);
		return false;
	}
  close(fd_motor);


  // registar no motor
  strcpy(data.cmd.command, "login");
  // enviar para o motor
  int result = sendTo(data, MOTOR_FIFO);
  if(result != 0){
    closeUI(fd, data, false);
    if(result == 1)
      sprintf(error, "%sERRO - nao foi possivel abrir o FIFO do Motor%s", C_FATAL_ERROR, C_CLEAR);
    else
      sprintf(error, "%sERRO - falha no envio%s", C_FATAL_ERROR, C_CLEAR);
    return false;
  }

  // receber resposta
	read(*fd, &data, sizeof(data));
  strcpy(error, data.error);
  if(!data.result){
    closeUI(fd, data, true);
    return false;
  }

  // limpar ecra
  system("clear");
  return true;

}

bool closeUI(int* fd, sharedData data, bool kicked){

  // avisar o motor
  if(!kicked){
    strcpy(data.cmd.command, "exit");
    int result = sendTo(data, MOTOR_FIFO);
    if(result == 1)
      printf("%sERRO - nao foi possivel abrir %s\n%s", C_ERROR, MOTOR_FIFO, C_CLEAR);
    else if(result == -1)
      printf("%sERRO - falha no envio\n%s", C_ERROR, C_CLEAR);
  }

  close(*fd);
	unlink(fifo);

  return true;
}

int checkCMD(prompt* prmt){

  if(strcmp(prmt->command, "msg") == 0){

    char pl[MAX_STRING], msg[MAX_STRING];

    if(sscanf(prmt->args, "%s %[^\n]", pl, msg) > 0);
      return MSG;
    strcpy(error, "erro de formatacao: msg <jogador> <mensagem>");
    return CMD_ERROR;

  } else if(strcmp(prmt->command, "help") == 0){

    if(strcmp(prmt->args, "") == 0)
      return HELP;
    strcpy(error, "erro de formatacao: help");
    return CMD_ERROR;

  } else if(strcmp(prmt->command, "exit") == 0){

    if(strcmp(prmt->args, "") == 0)
      return EXIT;
    strcpy(error, "erro de formatacao: exit");
    return CMD_ERROR;

  } else if(strcmp(prmt->command, "begin_motor") == 0){
    return BEGIN;

  } else if(strcmp(prmt->command, "move") == 0){
    return MOVE;

  } else {
    strcpy(error, "erro de formatacao: o comando nao existe");
    return CMD_ERROR; 
  }

}

bool checkWon(userInfo* user, bool* won){
  if(user->coords.x+1 == 0)
    *won = true;

  return *won;
}

bool movePlayer(int direction, userInfo* user, gameLevel level){

  switch (direction) {

  case UP:
    if(level.map[user->coords.y][user->coords.x-1] == 'X')
      return false;

  user->coords.x -= 1;
  return true;

  case DOWN:
    if(user->coords.x+1 == 0)
      return false;

    if(level.map[user->coords.y][user->coords.x+1] == 'X')
      return false;

  user->coords.x += 1;
  return true;

  case RIGHT:
    if(user->coords.y+1 == NUM_COLS)
      return false;

    if(level.map[user->coords.y+1][user->coords.x] == 'X')
      return false;

  user->coords.y += 1;
  return true;

  case LEFT:
    if(user->coords.y+1 == 0)
      return false;

    if(level.map[user->coords.y-1][user->coords.x] == 'X')
      return false;

  user->coords.y -= 1;
  return true;
  
  default:
  return false;
  }

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