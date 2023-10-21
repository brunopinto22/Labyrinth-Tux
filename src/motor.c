#include "motor_func.h"

// vars globais
#define PID getpid()
int fd, command = CLEAR;


void closeSig(){
  command = END;
}


int main(int argc, char** argv){

  setbuf(stdout, NULL);
	setbuf(stdin, NULL);

  // vars de comunicacao
	prompt cmd;
	char string[MAX_STRING];
  sharedData sharedCmd;

  // vars de jogo
  envVariables gameSettings;

	// select
	int res;
	fd_set fds;
	struct timeval timeout;

  // iniciar o motor
	if(!ini(&fd, &gameSettings)){
	  printf("\n%s\n", getError());
	  return 1;
	}

  // handlers de sinais
	signal(SIGINT, closeSig);

	printf("\n%s⋉  Motor [%d] ⋊%s\n\n>> ", C_MOTOR, PID, C_CLEAR);
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

		if (res == -1 && command != END) {
			printf("\n%sERRO - Occoreu um erro no select%s\n", C_FATAL_ERROR, C_CLEAR);
			close(fd);
			unlink(MOTOR_FIFO);
			return 1;

		}
		else if (res > 0 && FD_ISSET(0, &fds) && command != END) { // ler os comandos do ADMIN
			scanf("%[^\n]", string);
      setbuf(stdin, NULL);

      sscanf(string, "%s %[^\n]", cmd.command, cmd.args);

      command = checkCMD(&cmd);

      switch (command) {
      case CMD_ERROR:
        printf("\n%s\n",getError());
      break;

      case USERS:
        printf(">> lista users <<\n");
        // printUsers();
      break;

      case KICK:
        printf(">> kick %s<<\n", cmd.args);
        // kick(&cmd);
      break;

      case BOTS:
        printf(">> lista bots <<\n");
        // printBots();
      break;

      case BMOV:
      printf(">> inserir bloqueio movel <<\n");
        // rmMv();
      break;

      case RBM:
      printf(">> remover bloqueio movel <<\n");
        // rmBm();
      break;

      case BEGIN:
      printf(">> comecar jogo <<\n");
        // begin();
      break;

      case END:
        // sair
      break;
      
      default:
        printf("\n%sERRO - nao devia de ter chegado aqui%s\n", C_FATAL_ERROR, C_CLEAR);
        command = END;
        break;
      }

      printf("\n>> ");

		}
		else if (res > 0 && FD_ISSET(fd, &fds)  && command != END) { // ler os comandos do FIFO

			if (read(fd, &sharedCmd, sizeof(sharedData)) > 0) {
				printf("\n>> ");
			}

		}

	} while(command != END);

  if(!closeMotor(&fd))
    printf("%s\nERRO - programa nao foi bem fechado\n%s", C_FATAL_ERROR, C_CLEAR);

  printf("\n%s⋉  Bye Bye ⋊%s\n", C_MOTOR, C_CLEAR);
	return 0;
}