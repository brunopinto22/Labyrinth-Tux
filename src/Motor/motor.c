#include "motor_func.h"

// vars globais
#define PID getpid()
int command = CLEAR;

void closeSig(){
  command = END;
}


int main(int argc, char** argv){

  setbuf(stdout, NULL);
	setbuf(stdin, NULL);

  // vars do motor
  int fd;
  int usersCount = 0;
  userInfo users[MAX_USERS];

  // vars de comunicacao
	prompt cmd;
	char string[MAX_STRING];
  sharedData data;
  int result, commandUI = CLEAR;
  char fifoUi[MAX_STRING];

  // vars de jogo
  bool isGameStarted = false;
  int inGameUsers = 0;
  envVariables gameSettings;
  gameLevel levels[MAX_LEVELS];

	// select
	int res;
	fd_set fds;
	struct timeval timeout;

  // iniciar o motor
	if(!ini(&fd, &gameSettings, levels)){
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
			if(!closeMotor(&fd))
        printf("%s\nERRO - programa nao foi bem fechado\n%s", C_FATAL_ERROR, C_CLEAR);
      return 1;
		}
		else if (res > 0 && FD_ISSET(0, &fds) && command != END) { // ler os comandos do ADMIN
			scanf("%[^\n]", string);
      setbuf(stdin, NULL);

      // verificar comando
      sscanf(string, "%s %[^\n]", cmd.command, cmd.args);
      command = checkCMD(&cmd);

      switch (command) {
      case CMD_ERROR:
        printf("\n%s\n",getError());
      break;

      case USERS:
        printUsers(users, usersCount);
      break;

      case KICK:
        result = kickUser(cmd.args, users, &usersCount, &inGameUsers);
        if(result == 0)
          printf("\n%s\n", getError());
        else
          printf("\n%s[%d] : Foi expulso %s\n", C_MESSAGE, result, C_CLEAR);
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
        if(begin(&isGameStarted, users, usersCount, levels)){
          printf("\n%s> A comecar o Jogo%s\n", C_ONLINE, C_CLEAR);
          //runGame(levels, &gameSettings, users, usersCount);
        }
        else
          printf("\n%sO Jogo ja comecou%s\n", C_ERROR, C_CLEAR);

      break;

      case SETTINGS:
        printSettings(&gameSettings);
      break;

      case MAP:
        int lvl;
        sscanf(cmd.args, "%d", &lvl);
        printMap(lvl, levels);
      break;

      case HELP:
        printHelp();
      break;

      case TESTBOT:
        int x,y,t;
        char buffer[MAX_STRING];
        ssize_t bytes;

        int pipefd[2];
        if(pipe(pipefd) == -1){
          printf("\n%sERRO - a criacao do pipe falhou%s\n", C_FATAL_ERROR, C_CLEAR);
          command = END;
        }

        pid_t p = fork();
        if(p == -1){
          printf("\n%sERRO - o fork falhou%s\n", C_FATAL_ERROR, C_CLEAR);
          command = END;
        }

        if(p == 0){ // processo filho
          close(pipefd[0]);
          dup2(pipefd[1], STDOUT_FILENO);
          close(pipefd[1]);
          
          // executar o bot de teste
          execl("./testbot", "testbot", NULL);

          // caso nao execute
          printf("\n%sERRO - a execucao do testbot falhou%s\n", C_FATAL_ERROR, C_CLEAR);
          command = END;

        } else { // processo pai

          close(pipefd[1]);

          // esperar para receber valores
          while((bytes = read(pipefd[0], buffer, sizeof(buffer))) > 0){}

          close(pipefd[0]);
          wait(NULL);
        }
        
        // imprimir valor recebido
        sscanf(buffer, "%d %d %d", &x, &y, &t);
        printf("\nRecebi x=%d y=%d durancao=%d\n", x, y, t);

      break;

      case END:
        // sair
      break;
      
      default:
        printf("\n%sERRO - nao devia de ter chegado aqui%s\n", C_FATAL_ERROR, C_CLEAR);
        command = END;
      break;
      }

      if(command != END)
        printf("\n>> ");

		}
		else if (res > 0 && FD_ISSET(fd, &fds) && command != END) { // ler os comandos do FIFO
			if (read(fd, &data, sizeof(data)) > 0) {
        // guardar destinatario
        sprintf(fifoUi, UI_FIFO, data.user.pid);
        data.result = true;

        // verificar comando
        commandUI = checkCMD_UI(&data.cmd);

        switch (commandUI) {
        case LOGIN: 

            if(addUser(&data, users, &usersCount, &inGameUsers, isGameStarted))
              printf("\n\n%s[%d] : Fez login como '%s'%s\n", C_ONLINE, data.user.pid, data.user.name, C_CLEAR);
            else
              data.result = false;
        break;

        case MSG:
          // guardar o destinatario e a mensagem
          char name[MAX_STRING], text[MAX_STRING];
          sscanf(data.cmd.args, "%s %[^\n]", name, text);

          // guardar mensagem
          sprintf(data.error, "[%s] : %s", data.user.name, text);

          // verificar se utilizador existe
          result = userExists(name, users, usersCount);
          if(result == 0){
            data.result = false;
            sprintf(data.error, "O utilizador \'%s\' nao existe", name);
            strcpy(data.cmd.command, "msg");

          } else {
            sprintf(fifoUi, UI_FIFO, result);
            printf("\n\n%s[%d] : Enviou para \'%s\' \"%s\" %s\n", C_MESSAGE, data.user.pid, name, text, C_CLEAR);
          }
          
        break;

        case WON:
          printf("\n\n%s[%d] : Acabou o Nivel em %ss %s\n", C_MESSAGE, data.user.pid, data.cmd.args, C_CLEAR);
        break;

        case EXIT:
          // remove o utilizador do motor
          result = kickUser(data.user.name, users, &usersCount, &inGameUsers);
          printf("\n\n%s[%d] : Saiu %s\n", C_ERROR, result, C_CLEAR);
        break;
        
        default:
          // so deve chegar aqui caso algo esteja mal com o proprio codigo
          printf("\n\n%sERRO - nao devia de ter chegado aqui%s\n", C_FATAL_ERROR, C_CLEAR);
          commandUI = CMD_ERROR;
        break;
        }
        
        // enviar resultado
        if(commandUI != CMD_ERROR && commandUI != EXIT){
          result = sendTo(data, fifoUi);
          if(result == 1)
            printf("%s\nERRO - nao foi possivel abrir %s\n%s", C_ERROR, fifoUi, C_CLEAR);
          else if(result == -1)
            printf("%s\nERRO - falha no envio%s\n", C_ERROR, C_CLEAR);
        }

        printf("\n>> ");
        
			}
		}

	} while(command != END);

  if(!closeMotor(&fd)){
    printf("%s\nERRO - programa nao foi bem fechado\n%s", C_FATAL_ERROR, C_CLEAR);
    return 1;
  }

  closeUIs(users, usersCount);

  printf("\n%s⋉  Bye Bye ⋊%s\n", C_MOTOR, C_CLEAR);
	return 0;
}