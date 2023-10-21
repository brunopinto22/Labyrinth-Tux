#include "motor_func.h"

#define PID getpid()

int fd, command;

int main(int argc, char** argv){

	prompt cmd;
	char string[MAX_STRING];

  sharedData sharedCmd;

	// select
	int res;
	fd_set fds;
	struct timeval timeout;

	if(!ini(&fd)){
	  printf("\n%s\n", getError());
	  return 1;
	}

	printf("\n%s⋉  Motor [%d] ⋊%s\n\n>> ", C_MOTOR, PID, C_CLEAR);
	do{

		// limpar a vars
    fflush(stdin);
		FD_ZERO(&fds);
    command = CLEAR;
    cmd.command[0] = 0;
		cmd.args[0] = 0;

		// canais para estar atento
		FD_SET(0, &fds);
		FD_SET(fd, &fds);
		timeout.tv_sec = 1; // Timeout de 1 segundo

		res = select(fd + 1, &fds, NULL, NULL, &timeout);

		if (res == -1) {
			printf("Occoreu um erro no select\n");
			close(fd);
			unlink(MOTOR_FIFO);
			return 1;

		}
		else if (res > 0 && FD_ISSET(0, &fds)) { // ler os comandos do ADMIN
			scanf("%[^\n]", string);

      sscanf(string, "%s %s", cmd.command, cmd.args);

      command = checkCMD(&cmd);

      if(command == CMD_ERROR)
        printf("\n%s\n",getError());

		}
		else if (res > 0 && FD_ISSET(fd, &fds)) { // ler os comandos do FIFO

			if (read(fd, &sharedCmd, sizeof(sharedData)) > 0) {
				printf("\n>> ");
			}

		}

	} while(command != END);

  printf("\n%s⋉  Bye Bye ⋊%s\n\n>> ", C_MOTOR, C_CLEAR);
	return 0;
}