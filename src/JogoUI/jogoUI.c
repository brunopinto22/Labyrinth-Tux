#include <pthread.h>
#include "jogoUI_func.h"
#include "style.h"

// vars globais
#define PID getpid()
int command = CLEAR;

// temporizador
int level_timer = 0;
void *levelTimer(void *arg) {
  while (1) {
    sleep(1);
    level_timer++;
  }
  return NULL;
}


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

  // threads
  pthread_t th_LevelTimer;

  // vars da UI
  int fd;
  userInfo user;
  bool gameStarted = false;
  bool won = false;

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
  user.coords.x = NUM_LINES-1;
  user.coords.y = NUM_COLS/2-1;
  data.user = user;

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
      printOutput("", false);

      switch (command) {
      case CMD_ERROR:
        printOutput(getError(), true);
      break;

      case UP:
      case DOWN:
      case RIGHT:
      case LEFT:

        if(!gameStarted){
          printOutput("Erro - nao se pode movimentar ainda", true);
          break;
        }
        if(won){
          printOutput("Erro - ja completou o nivel", true);
          break;
        }

        if(!movePlayer(command, &user, data.level))
          break;

        printMap(data.level);
        printUserOnMap(&user);

        // avisar o Motor
        strcpy(data.cmd.command, "move");
        result = sendTo(data, MOTOR_FIFO);
        if(result == 1){
          sprintf(data.error, "ERRO - nao foi possivel abrir %s", MOTOR_FIFO);
          printOutput(data.error, result != 0);

        } else if(result == -1){
          strcpy(data.error, "ERRO - falha no envio");
          printOutput(data.error, result != 0);
        }
        


        // verifica se ja acabou o nivel
        if(checkWon(&user, &won)){
          sprintf(data.error, "Acabou o Nivel em %ds", level_timer);
          printTitle(data.error);

          // avisar o Motor
          strcpy(data.cmd.command, "won");
          sprintf(data.cmd.args, "%d", level_timer);

          result = sendTo(data, MOTOR_FIFO);
          if(result == 1)
            sprintf(data.error, "ERRO - nao foi possivel abrir %s", MOTOR_FIFO);
          else if(result == -1)
            strcpy(data.error, "ERRO - falha no envio");
          printOutput(data.error, result != 0);

        }

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
        // fechar janela
        closeWindow();

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
          level_timer = 0;
          gameStarted = true;
          printTitle("O Jogo Comecou");
          printMap(data.level);

          // criar thread para o temporizador
          if (pthread_create(&th_LevelTimer, NULL, levelTimer, NULL) != 0) {
            // fechar janela
            closeWindow();

            if(!closeUI(&fd, data, false)){
              printf("%s\nERRO - programa nao foi bem fechado\n\n%s", C_FATAL_ERROR, C_CLEAR);
              return 1;
            }
            printf("%s\nERRO - nao foi possivel criar a thread de tempo\n\n%s", C_FATAL_ERROR, C_CLEAR);
            return 1;
          }

          printUserOnMap(&user);
        break;

        case MOVE:
          sprintf(data.error, "%d - moveu se para %d %d", data.user.pid, data.user.coords.x, data.user.coords.y);
          printOutput(data.error, false);
        break;
        
        default:
          break;
        }
				
			}

		}

	} while(command != EXIT && command != KICKED);

  if(!closeUI(&fd, data, command == KICKED)){
    // terminar a thread
    if (pthread_cancel(th_LevelTimer) != 0) {
      printf("%s\nERRO - nao foi possivel terminar a thread de tempo\n\n%s", C_FATAL_ERROR, C_CLEAR);
      return 1;
    }
    printf("%s\nERRO - programa nao foi bem fechado\n\n%s", C_FATAL_ERROR, C_CLEAR);
    return 1;
  }

  // terminar a thread
  if (pthread_cancel(th_LevelTimer) != 0) {
    printf("%s\nERRO - nao foi possivel terminar a thread de tempo\n\n%s", C_FATAL_ERROR, C_CLEAR);
    return 1;
  }

  printf("\n%s⋉  Bye Bye ⋊%s\n\n", C_UI, C_CLEAR);
	return 0;
}