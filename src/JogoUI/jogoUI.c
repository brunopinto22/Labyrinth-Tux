#include <pthread.h>
#include "jogoUI_func.h"
#include "style.h"

// vars globais
#define PID getpid()
int command = CLEAR;

// temporizador
int current_level = 0;
int level_timer = 0;
int best_time = 0;
bool gameStarted = false;
bool won = false;
bool levelTimerExists = false;
void *levelTimer(void *arg) {
  char text[MAX_STRING];
  levelTimerExists = true;
  while (1) {

    sleep(1);
    level_timer--;

    if(!won)
      best_time++;

    sprintf(text, "Nivel: %d | Tempo: %d | Tempo restante: %d", current_level, best_time, level_timer);
    printTitle(text);

  }
  level_timer = 0;
  best_time = 0;
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

			if(!closeUI(&fd, data, false))
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

      case PLAYERS:

        strcpy(data.cmd.command, "players");

        // avisar motor
        result = sendTo(data, MOTOR_FIFO);
        if(result == 1)
          sprintf(data.error, "ERRO - nao foi possivel abrir %s", MOTOR_FIFO);
        else if(result == -1)
          strcpy(data.error, "ERRO - falha no envio");
        printOutput(data.error, result != 0);

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

        for(int i=0; i < data.users_count; i++)
          if(data.user_list[i].inGame && data.user_list[i].pid != PID)
            printUserOnMap(&data.user_list[i]);

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
          sprintf(data.error, "Acabou o Nivel em %ds", best_time);
          printTitle(data.error);

          // avisar o Motor
          strcpy(data.cmd.command, "won");
          sprintf(data.cmd.args, "%d", best_time);

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

        case PLAYERS:

          printScore(data.error, data.result);

        break;

        case BEGIN:
          best_time = 0;
          level_timer = data.level.level_time;
          current_level = data.level.level;
          
          won = false;
          printMap(data.level);
          
          user.coords.x = NUM_LINES-1;
          user.coords.y = NUM_COLS/2-1;
          printUserOnMap(&user);

          if(gameStarted)
            break;


          gameStarted = true;
          // criar thread para o temporizador
          if(pthread_create(&th_LevelTimer, NULL, levelTimer, NULL) != 0) {
            // fechar janela
            closeWindow();

            if(!closeUI(&fd, data, false))
              printf("%s\nERRO - programa nao foi bem fechado\n\n%s", C_FATAL_ERROR, C_CLEAR);
            
            printf("%s\nERRO - nao foi possivel criar a thread de tempo\n\n%s", C_FATAL_ERROR, C_CLEAR);
            return 1;
          }

          
        break;

        case MOVE:

          printMap(data.level);

          for(int i=0; i < data.users_count; i++)
            if(data.user_list[i].inGame)
              printUserOnMap(&data.user_list[i]);
          
        break;

        case ENDGAME:
          // terminar a thread
          if (pthread_cancel(th_LevelTimer) != 0) {
            printf("%s\nERRO - nao foi possivel terminar a thread de tempo\n\n%s", C_FATAL_ERROR, C_CLEAR);
            return 1;
          }
          levelTimerExists = false;
          
          // limpa o mapa
          clearMap();

          printTitle(data.error);

          level_timer = 0;
          best_time = 0;
          gameStarted = false;

        break;
        
        default:
          break;
        }
				
			}

		}

	} while(command != EXIT && command != KICKED);

  if(!closeUI(&fd, data, command == KICKED))
    printf("%s\nERRO - programa nao foi bem fechado\n\n%s", C_FATAL_ERROR, C_CLEAR);

  // terminar a thread
  if(levelTimerExists)
    if (pthread_cancel(th_LevelTimer) != 0)
      printf("%s\nERRO - nao foi possivel terminar a thread de tempo\n\n%s", C_FATAL_ERROR, C_CLEAR);

  printf("\n%s⋉  Bye Bye ⋊%s\n\n", C_UI, C_CLEAR);
	return 0;
}