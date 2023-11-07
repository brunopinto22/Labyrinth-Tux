#include "jogoUI_func.h"

// vars globais
#define PID getpid()
int command = CLEAR;

void closeSig(){
  command = EXIT;
}

void closeMotor(){
  printf("\n\n%s> O Motor fechou%s\n",C_MOTOR, C_CLEAR);
  closeSig();
}

void closeKick(){
  printf("\n\n%s> O Motor expulsou-o%s\n",C_FATAL_ERROR, C_CLEAR);
  closeSig();
}


int main(int argc, char** argv){

  setbuf(stdout, NULL);
	setbuf(stdin, NULL);

  // vars da UI
  int fd;
  userInfo user;

  // vars de comunicacao
	prompt cmd;
	char string[MAX_STRING];
  sharedData data;
  int result;

	// select
	int res;
	fd_set fds;
	struct timeval timeout;

  // verificacao da formatacao
	if (argc != 2) {
		printf("\n%sERRO na inicializacao: ./FrontEnd <username>%s\n", C_FATAL_ERROR, C_CLEAR);
		return 1;
	}

  // guardar informacao da UI
  user.pid = PID;
  strcpy(user.name, argv[1]);
  data.user = user;

  // iniciar a UI
	if(!ini(&fd, user)){
	  printf("\n%s\n\n", getError());
	  return 1;
	}

  // handlers de sinais
	signal(SIGINT, closeSig);
	signal(SIGUSR1, closeMotor);
	signal(SIGUSR2, closeKick);

	printf("\n%s⋉  JogoUI [%d] ⋊%s\n\n>> ", C_UI, PID, C_CLEAR);
	do{

		// limpar a vars
		FD_ZERO(&fds);
    cmd.command[0] = 0;
		cmd.args[0] = 0;

		// canais para estar atento
		FD_SET(0, &fds);
		FD_SET(fd, &fds);
		timeout.tv_sec = 1; // Timeout de 1 segundo

		res = select(fd + 1, &fds, NULL, NULL, &timeout);

		if (res == -1 && command != EXIT) {
			printf("\n%sERRO - Occoreu um erro no select%s\n\n", C_FATAL_ERROR, C_CLEAR);
			if(!closeUI(&fd))
        printf("%s\nERRO - programa nao foi bem fechado\n\n%s", C_FATAL_ERROR, C_CLEAR);
      return 1;
		}
		else if (res > 0 && FD_ISSET(0, &fds) && command != EXIT) { // ler os comandos do ADMIN
			scanf("%[^\n]", string);
      setbuf(stdin, NULL);

      sscanf(string, "%s %[^\n]", cmd.command, cmd.args);

      command = checkCMD(&cmd);

      switch (command) {
      case CMD_ERROR:
        printf("\n%s\n\n", getError());
      break;

      case UP:
        printf("andei para cima");
      break;

      case DOWN:
        printf("andei para baixo");
      break;

      case RIGHT:
        printf("andei para a direita");
      break;

      case LEFT:
        printf("andei para a esquerda");
      break;

      case CHANGE_RD_WR:
        printf("mudar para modo de escrita");
      break;

      case MSG:
        char pl[MAX_STRING], msg[MAX_STRING];
        sscanf(cmd.args, "%s %[^\n]", pl, msg);
        printf("mandei '%s' para [%s]", msg, pl);
      break;

      case HELP:
        printHelp();
      break;

      case EXIT:
        // sair
      break;
      
      default:
        printf("\n%sERRO - nao devia de ter chegado aqui%s\n\n", C_FATAL_ERROR, C_CLEAR);
        command = EXIT;
      break;
      }

      if(command != EXIT)
        printf("\n>> ");

		}
		else if (res > 0 && FD_ISSET(fd, &fds)  && command != EXIT) { // ler os comandos do FIFO

			if (read(fd, &data, sizeof(data)) > 0) {
				printf("\n>> ");
			}

		}

	} while(command != EXIT);

  if(!closeUI(&fd)){
    printf("%s\nERRO - programa nao foi bem fechado\n\n%s", C_FATAL_ERROR, C_CLEAR);
    return 1;
  }

  // avisar o motor
  strcpy(data.cmd.command, "exit");
  result = sendTo(data, MOTOR_FIFO);
  if(result == 1)
    printf("%sERRO - nao foi possivel abrir %s\n%s", C_ERROR, MOTOR_FIFO, C_CLEAR);
  else if(result == -1)
    printf("%sERRO - falha no envio\n%s", C_ERROR, C_CLEAR);

  printf("\n%s⋉  Bye Bye ⋊%s\n", C_UI, C_CLEAR);
	return 0;
}