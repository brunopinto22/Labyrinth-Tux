#include "jogoUI_func.h"
#include "style.h"

// vars globais
#define PID getpid()
int command = CLEAR;


void closeMotor(){
  // fechar janela
  closeWindow();

  printf("\n%s> O Motor fechou%s\n",C_MOTOR, C_CLEAR);
  command = KICKED;
}

void closeKick(){
  // fechar janela
  closeWindow();

  printf("\n%s> O Motor expulsou-o%s\n",C_FATAL_ERROR, C_CLEAR);
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
  int result, commandMotor = CLEAR;

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
  user.coords.x = NUM_LINES;
  user.coords.y = NUM_COLS/2;

  // iniciar a UI
	if(!ini(&fd, data)){
	  printf("\n%s\n\n", getError());
	  return 1;
	}

  // iniciar ncurses
  setupWindow();

  // handlers de sinais
	signal(SIGUSR1, closeMotor);
	signal(SIGUSR2, closeKick);

  printWindow();
  printTitle(getError());
	do{

		// limpar a vars
		FD_ZERO(&fds);
    cmd.command[0] = 0;
		cmd.args[0] = 0;
    user.pid = PID;
    strcpy(user.name, argv[1]);
    data.user = user;

		// canais para estar atento
		FD_SET(0, &fds);
		FD_SET(fd, &fds);
		timeout.tv_sec = 1; // Timeout de 1 segundo

		res = select(fd + 1, &fds, NULL, NULL, &timeout);

		if (res == -1 && command != EXIT && command != KICKED) {
      // fechar janela
      closeWindow();

			printf("\n%sERRO - Occoreu um erro no select%s\n\n", C_FATAL_ERROR, C_CLEAR);

			if(!closeUI(&fd, data, command == KICKED))
        printf("%s\nERRO - programa nao foi bem fechado\n\n%s", C_FATAL_ERROR, C_CLEAR);
      return 1;
		}
		else if (res > 0 && FD_ISSET(0, &fds) && command != EXIT && command != KICKED) { // ler os comandos da UI
			
      command = readKeyboard(&data.cmd);

      switch (command) {
      case CMD_ERROR:
        printOutput(getError(), true);
      break;

      case UP:
        printOutput("andei para cima", false);
        user.coords.x -= 1;
        printMap(data.level);
        printUserOnMap(&user);
      break;

      case DOWN:
        printOutput("andei para baixo", false);
        user.coords.x += 1;
        printMap(data.level);
        printUserOnMap(&user);
      break;

      case RIGHT:
        printOutput("andei para a direita", false);
        user.coords.y += 1;
        printMap(data.level);
        printUserOnMap(&user);
      break;

      case LEFT:
        printOutput("andei para a esquerda", false);
        user.coords.y -= 1;
        printMap(data.level);
        printUserOnMap(&user);
      break;

      case MSG:
        
        // guardar o destinatario
        char name[MAX_STRING];
        sscanf(data.cmd.args, "%s", name);
        
        // verificar se o destinatario e o proprio utilizador
        if(strcmp(name, data.user.name) == 0){
          strcpy(data.error, "ERRO - nao pode enviar uma mensagem para si proprio");
          printOutput(data.error, true);
          break;
        }

        result = sendTo(data, MOTOR_FIFO);
        sprintf(data.error, "mensagem enviada para \'%s\'", name);
        if(result == 1)
          sprintf(data.error, "ERRO - nao foi possivel abrir %s", MOTOR_FIFO);
        else if(result == -1)
          strcpy(data.error, "ERRO - falha no envio");
        printOutput(data.error, result != 0);
        
      break;

      case HELP:
        printHelp();
      break;

      case EXIT:
        // fechar janela
        closeWindow();
      break;
      
      default:
        printf("\n%sERRO - nao devia de ter chegado aqui%s\n\n", C_FATAL_ERROR, C_CLEAR);
        command = EXIT;
      break;
      }


		}
		else if (res > 0 && FD_ISSET(fd, &fds)  && command != EXIT && command != KICKED) { // ler o FIFO

			if (read(fd, &data, sizeof(data)) > 0) {
        
        commandMotor = checkCMD(&data.cmd);

        switch (commandMotor) {
        case MSG:
          if(data.result)
            printMessage(data.error);
          else
            printOutput(data.error, true);
          
        break;

        case BEGIN:
          printTitle("O Jogo Comecou");
          printMap(data.level);
          printUserOnMap(&user);
        break;
        
        default:
          break;
        }
				
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