#include "jogoUI_func.h"
#include "style.h"

// vars globais
#define PID getpid()
int command = CLEAR;

void closeSig(){
  // fechar janela
  closeWindow();
    
  command = EXIT;
}

void closeMotor(){
  // fechar janela
  closeWindow();

  printf("\n\n%s> O Motor fechou%s\n",C_MOTOR, C_CLEAR);
  command = KICKED;
}

void closeKick(){
  // fechar janela
  closeWindow();

  printf("\n\n%s> O Motor expulsou-o%s\n",C_FATAL_ERROR, C_CLEAR);
  command = KICKED;
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
	if(!ini(&fd, data)){
	  printf("\n%s\n\n", getError());
	  return 1;
	}

  // iniciar ncurses
  setupWindow();

  // handlers de sinais
	signal(SIGINT, closeSig);
	signal(SIGUSR1, closeMotor);
	signal(SIGUSR2, closeKick);

  printWindow(&data);
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

		if (res == -1 && command != EXIT && command != KICKED) {
			printf("\n%sERRO - Occoreu um erro no select%s\n\n", C_FATAL_ERROR, C_CLEAR);
			if(!closeUI(&fd, data, command == KICKED))
        printf("%s\nERRO - programa nao foi bem fechado\n\n%s", C_FATAL_ERROR, C_CLEAR);
      return 1;
		}
		else if (res > 0 && FD_ISSET(0, &fds) && command != EXIT && command != KICKED) { // ler os comandos da UI
			
      command = readKeyboard();

      switch (command) {
      case CMD_ERROR:
        printOutput(getError(), true);
      break;

      case UP:
        printOutput("andei para cima", false);
      break;

      case DOWN:
        printOutput("andei para baixo", false);
      break;

      case RIGHT:
        printOutput("andei para a direita", false);
      break;

      case LEFT:
        printOutput("andei para a esquerda", false);
      break;

      case MSG:
        printOutput("mandei msg", false);
      break;

      case HELP:
        printHelp();
      break;

      case EXIT:
      break;
      
      default:
        printf("\n%sERRO - nao devia de ter chegado aqui%s\n\n", C_FATAL_ERROR, C_CLEAR);
        command = EXIT;
      break;
      }

		}
		else if (res > 0 && FD_ISSET(fd, &fds)  && command != EXIT && command != KICKED) { // ler o FIFO

			if (read(fd, &data, sizeof(data)) > 0) {
				
			}

		}

	} while(command != EXIT && command != KICKED);

  if(!closeUI(&fd, data, command == KICKED)){
    printf("%s\nERRO - programa nao foi bem fechado\n\n%s", C_FATAL_ERROR, C_CLEAR);
    return 1;
  }

  printf("\n%s⋉  Bye Bye ⋊%s\n\n", C_UI, C_CLEAR);
	return 0;
}