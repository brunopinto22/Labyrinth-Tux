#include "motor_func.h"

char error[MAX_ERROR] = "No Error yet lol";


char* getError(){ return error; }


bool ini(int* fd, envVariables* gameSettings, gameLevel* levels){

  // variaveis de ambiente
  if(!setGameSettings(gameSettings))
    return false;

  // carregar mapas
  if(!loadMaps(levels, gameSettings))
    return false;
  
  // abertura do fifo do motor para leitura
	if (mkfifo(MOTOR_FIFO, 0666) == -1) {
		if (errno == EEXIST)
      sprintf(error, "%sERRO - Servidor em execucao%s", C_FATAL_ERROR, C_CLEAR);
    else
      sprintf(error, "%sERRO - Nao foi possivel criar o FIFO%s", C_FATAL_ERROR, C_CLEAR);
		return false;
	}

	*fd = open(MOTOR_FIFO, O_RDWR);
	if (*fd == -1) {
    sprintf(error, "%sERRO - Nao foi possivel abrir o FIFO%s", C_FATAL_ERROR, C_CLEAR);
    closeMotor(fd);
		return false;
	}

  // limpar ecra
  system("clear");
  return true;
}

bool loadMaps(gameLevel levels[MAX_LEVELS], envVariables* gameSettings){

  FILE* file;
  char file_name[MAX_STRING];
  char aux[MAX_STRING];

  for(int i=0; i<MAX_LEVELS; i++){

    // definir o nome do ficheiro a abrir
    sprintf(file_name, LEVEL_FILE, i+1);
    // abrir o ficheiro
    file = fopen(file_name, "r");
    if(file == NULL){ // caso nao exista usa o nivel default
      file = fopen(DEFAULT_LEVEL_FILE, "r");
      if(file == NULL){
        sprintf(error, "%sERRO - Nao foi possivel abrir o ficheiro '%s' %s", C_FATAL_ERROR, DEFAULT_LEVEL_FILE, C_CLEAR);
        return false;
      }
    }

    // guardar mapa
    for (int lin=0; lin < NUM_LINES; lin++){
      fgets(aux, sizeof(aux), file);
      for(int col=0; col < NUM_COLS; col++)
        levels[i].map[col][lin] = aux[col];
    }
    fclose(file);
    
    // definir definicoes do niveis
    levels[i].level = i+1;
    levels[i].level_time = gameSettings->timer - (gameSettings->timer_dc * i);

  }

  return true;

}

bool closeMotor(int* fd){
  
  close(*fd);
	unlink(MOTOR_FIFO);

  return true;
}

void printUsers(userInfo* users_list, int users_count){
  char state[MAX_STRING];

  if(users_count == 0){
    printf("\n%s> Nao existem Users online%s\n", C_ERROR, C_CLEAR);
    return;
  }


  printf("\n%s> Lista de Users%s\n", C_MESSAGE, C_CLEAR);

  for(int i=0; i < users_count; i++){
    strcpy(state, C_IDLE);
    if(users_list[i].inGame)
      strcpy(state, C_ONLINE);

    printf("\t%s● %s≻ %s%s [%d] : x=%d y=%d\n", state, C_UI, users_list[i].name, C_CLEAR, users_list[i].pid, users_list[i].coords.x, users_list[i].coords.y);

  }

}

pid_t userExists(char* user_to_find, userInfo* users_list, int users_count){

  for(int i=0; i < users_count; i++)
    if(strcmp(users_list[i].name, user_to_find) == 0)
      return users_list[i].pid;
  return 0;

}

bool addUser(sharedData* data, userInfo* users_list, int* users_count, int* inGameUsers, bool gameStarted){

  // verificar se tem espaco para mais um jogador         
  if(*users_count >= MAX_USERS){
    sprintf(data->error, "%sNumero maximo de jogadores permitidos atingido%s", C_FATAL_ERROR, C_CLEAR);
    return false;
  }

  // verifica se o username ja existe no sistema
  if(userExists(data->user.name, users_list, *users_count) != 0){
    sprintf(data->error, "%sJa existe um Jogador com o nome \'%s\'%s", C_FATAL_ERROR, data->user.name, C_CLEAR);
    return false;
  }

  // define o estado do jogador
  data->user.inGame = !gameStarted;

  if(!gameStarted)
    *inGameUsers += 1;
  
  // adiciona o utilizador
  users_list[*users_count] = data->user;
  users_list[*users_count].coords.x = NUM_LINES-1;
  users_list[*users_count].coords.y = NUM_COLS/2-1;
  *users_count +=1;

  // verifica o estado do jogador e define a mensagem conforme
  if(!gameStarted)
    strcpy(data->error, "O jogo comecera em pouco tempo");
  else
    strcpy(data->error, "Espere que o jogo atual acabe");

  return true;

}

int kickUser(char* user_name, userInfo* users_list, int* users_count, int* inGameUsers){
  union sigval val;
  int pid = 0;

  for (int i=0, y=0; i < *users_count; i++, y++){
		if(strcmp(user_name, users_list[i].name) == 0){
      pid = users_list[i].pid;
			y++;
      if(users_list[i].inGame)
        *inGameUsers -= 1;
      sigqueue(users_list[i].pid, SIGUSR2, val);
		}
		strcpy(users_list[i].name, users_list[y].name);
		users_list[i].pid = users_list[y].pid;
	}


  if(pid == 0)
    sprintf(error, "%sERRO - Nao foi encontrado nenhum utilizador chamado '%s'%s", C_ERROR, user_name,C_CLEAR);
  else
    *users_count -= 1;

  return pid;
}

void updateUsersMove(sharedData* data, userInfo* users_list, int users_count){

  char fifoUi[MAX_STRING];
  int result;

  for(int i=0; i < users_count; i++){
    if(users_list[i].pid != data->user.pid && users_list[i].inGame){
      sprintf(fifoUi, UI_FIFO, users_list[i].pid);
      result = sendTo(*data, fifoUi);
      if(result == 1)
        printf("%s\nERRO - nao foi possivel abrir %s\n%s", C_ERROR, fifoUi, C_CLEAR);
      else if(result == -1)
        printf("%s\nERRO - falha no envio%s\n", C_ERROR, C_CLEAR);

    }
  }

}

void closeUIs(userInfo* users, int users_count){

  union sigval val;
  for(int i=0; i < users_count; i++)
		sigqueue(users[i].pid, SIGUSR1, val);

}


bool setGameSettings(envVariables* gameSettings){
  char* env = getenv(TIMER);
  if(env != NULL){
    if(sscanf(env, "%d", &gameSettings->timer) != 1){
      sprintf(error, "%sERRO - a variavel de ambiente %s tem um valor nao int%s", C_FATAL_ERROR, TIMER,C_CLEAR);
		  return false;
    }
  } else {
    sprintf(error, "%sERRO - Nao foi possivel encontrar a variavel de ambiente %s%s", C_FATAL_ERROR, TIMER,C_CLEAR);
		return false;
  }

  env = getenv(TIME_REG);
  if(env != NULL){
    if(sscanf(env, "%d", &gameSettings->reg_time) != 1){
      sprintf(error, "%sERRO - a variavel de ambiente %s tem um valor nao int%s", C_FATAL_ERROR, TIME_REG,C_CLEAR);
		  return false;
    }
  } else {
    sprintf(error, "%sERRO - Nao foi possivel encontrar a variavel de ambiente %s%s", C_FATAL_ERROR, TIME_REG,C_CLEAR);
		return false;
  }

  env = getenv(TIMER_DECREMENT);
  if(env != NULL){
    if(sscanf(env, "%d", &gameSettings->timer_dc) != 1){
      sprintf(error, "%sERRO - a variavel de ambiente %s tem um valor nao int%s", C_FATAL_ERROR, TIMER_DECREMENT,C_CLEAR);
		  return false;
    }
  } else {
    sprintf(error, "%sERRO - Nao foi possivel encontrar a variavel de ambiente %s%s", C_FATAL_ERROR, TIMER_DECREMENT,C_CLEAR);
		return false;
  }

  env = getenv(MIN_USERS);
  if(env != NULL){
    if(sscanf(env, "%d", &gameSettings->min_players) != 1){
      sprintf(error, "%sERRO - a variavel de ambiente %s tem um valor nao int%s", C_FATAL_ERROR, MIN_USERS,C_CLEAR);
		  return false;
    }
    } else {
      sprintf(error, "%sERRO - Nao foi possivel encontrar a variavel de ambiente %s%s", C_FATAL_ERROR, MIN_USERS,C_CLEAR);
		return false;
  }

  return true;
}

int checkCMD(prompt* prmt){

  if(strcmp(prmt->command, "users") == 0){

    if(strcmp(prmt->args, "") == 0)
      return USERS;
    sprintf(error, "%serro de formatacao: %susers%s", C_ERROR, C_FERROR, C_CLEAR);
    return CMD_ERROR;

  } else if(strcmp(prmt->command, "kick") == 0){

    if(strcmp(prmt->args, "") != 0)
      return KICK;
    sprintf(error, "%serro de formatacao: %skick <nome>%s", C_ERROR, C_FERROR, C_CLEAR);
    return CMD_ERROR;

  } else if(strcmp(prmt->command, "bots") == 0){

    if(strcmp(prmt->args, "") == 0)
      return BOTS;
    sprintf(error, "%serro de formatacao: %sbots%s", C_ERROR, C_FERROR, C_CLEAR);
    return CMD_ERROR;

  } else if(strcmp(prmt->command, "bmov") == 0){

    if(strcmp(prmt->args, "") == 0)
      return BMOV;
    sprintf(error, "%serro de formatacao: %sbmov%s", C_ERROR, C_FERROR, C_CLEAR);
    return CMD_ERROR;

  } else if(strcmp(prmt->command, "rbm") == 0){

    if(strcmp(prmt->args, "") == 0)
      return RBM;
    sprintf(error, "%serro de formatacao: %srbm%s", C_ERROR, C_FERROR, C_CLEAR);
    return CMD_ERROR;

  } else if(strcmp(prmt->command, "begin") == 0){

    if(strcmp(prmt->args, "") == 0)
      return BEGIN;
    sprintf(error, "%serro de formatacao: %sbegin%s", C_ERROR, C_FERROR, C_CLEAR);
    return CMD_ERROR;

  } else if(strcmp(prmt->command, "settings") == 0){

    if(strcmp(prmt->args, "") == 0)
      return SETTINGS;
    sprintf(error, "%serro de formatacao: %ssettings%s", C_ERROR, C_FERROR, C_CLEAR);
    return CMD_ERROR;

  } else if(strcmp(prmt->command, "help") == 0){

    if(strcmp(prmt->args, "") == 0)
      return HELP;
    sprintf(error, "%serro de formatacao: %shelp%s", C_ERROR, C_FERROR, C_CLEAR);
    return CMD_ERROR;

  } else if(strcmp(prmt->command, "map") == 0){
    
    int lvl;

    if(strcmp(prmt->args, "") == 0){
      sprintf(error, "%serro de formatacao: %smap <nivel>%s", C_ERROR, C_FERROR, C_CLEAR);
      return CMD_ERROR;
    }

    if(sscanf(prmt->args, "%d", &lvl) != -1){
      if(lvl <= 0 || lvl > MAX_LEVELS){
        sprintf(error, "%serro de formatacao: %so nivel tem de ser um valor entre [ %d , %d ]%s", C_ERROR, C_FERROR, 1, MAX_LEVELS, C_CLEAR);
        return CMD_ERROR;
      }
      return MAP;
    }

    sprintf(error, "%serro de formatacao: %smap <nivel>%s", C_ERROR, C_FERROR, C_CLEAR);
    return CMD_ERROR;

  } else if(strcmp(prmt->command, "time") == 0){
    return TIME;

  } if(strcmp(prmt->command, "end") == 0){

    if(strcmp(prmt->args, "") == 0)
      return END;
    sprintf(error, "%serro de formatacao: %sexit%s", C_ERROR, C_FERROR, C_CLEAR);
    return CMD_ERROR;

  } else {
    sprintf(error, "%serro de formatacao: %so comando nao existe%s", C_ERROR, C_FERROR, C_CLEAR);
    return CMD_ERROR;

  }

}

int checkCMD_UI(prompt* prmt){

  if(strcmp(prmt->command, "login") == 0){
    return LOGIN;

  } else if(strcmp(prmt->command, "move") == 0){

    if(strcmp(prmt->args, "") == 0)
      return MOVE;
    return CMD_ERROR;

  } else if(strcmp(prmt->command, "msg") == 0){

    char pl[MAX_STRING], msg[MAX_STRING];

    if(sscanf(prmt->args, "%s %[^\n]", pl, msg) > 0);
      return MSG;
    return CMD_ERROR;

  } else if(strcmp(prmt->command, "won") == 0){
      return WON;

  } else if(strcmp(prmt->command, "exit") == 0){
      return EXIT;

  } else 
    return CMD_ERROR; 

}

void printMap(int level, gameLevel levels[MAX_LEVELS]){

  printf("\n%sNivel:%s %d\n", C_MESSAGE, C_CLEAR, levels[level-1].level);
  printf("%sDuracao:%s %d\n", C_MESSAGE, C_CLEAR, levels[level-1].level_time);

  for(int lin=0; lin < NUM_LINES; lin++){
    for(int col=0; col < NUM_COLS; col++)
      putchar(levels[level-1].map[col][lin]); 
    putchar('\n');
  }

}

void printSettings(envVariables* gameSettings){
  printf("\n\t%sDuracao de cada nivel ≻%s %d", C_MESSAGE, C_CLEAR, gameSettings->timer);
  printf("\n\t%sDecremento ≻%s %d", C_MESSAGE, C_CLEAR, gameSettings->timer_dc);
  printf("\n\t%sTempo de Registo ≻%s %d", C_MESSAGE, C_CLEAR, gameSettings->reg_time);
  printf("\n\t%sMinimo de Jogadores ≻%s %d\n", C_MESSAGE, C_CLEAR, gameSettings->min_players);
}

void printHelp(){
  printf("\n\t%susers ≻%s imprime a lista de todos os jogadores na plantaforma", C_MESSAGE, C_CLEAR);
  printf("\n\t%skick ≻%s expulsa um jogador", C_MESSAGE, C_CLEAR);
  printf("\n\t%sbots ≻%s lista os bots", C_MESSAGE, C_CLEAR);
  printf("\n\t%sbmov ≻%s insere um bloqueio movel", C_MESSAGE, C_CLEAR);
  printf("\n\t%srbm ≻%s remove um bloqueio movel", C_MESSAGE, C_CLEAR);
  printf("\n\t%sbegin ≻%s comeca o jogo", C_MESSAGE, C_CLEAR);
  printf("\n\t%ssettings ≻%s mostra as definicoes do jogo", C_MESSAGE, C_CLEAR);
  printf("\n\t%smap ≻%s mostra o mapa com as suas respetivas definicoes", C_MESSAGE, C_CLEAR);
  printf("\n\t%send ≻%s fecha o programa\n", C_MESSAGE, C_CLEAR);
}


bool begin(bool* gameStarted, userInfo* users, int users_count, gameLevel levels[MAX_LEVELS], int currentGame){
  if(*gameStarted)
    return false;

  // avisa os UIs todos
  sharedData data;
  strcpy(data.cmd.command, "begin_motor");
  data.level.level = levels[currentGame].level;
  data.level.level_time = levels[currentGame].level_time;

  for (int lin=0; lin < NUM_LINES; lin++)
    for(int col=0; col < NUM_COLS; col++)
      data.level.map[col][lin] = levels[currentGame].map[col][lin];

  char fifoUi[MAX_STRING];
  for(int i=0; i < users_count; i++){
    sprintf(fifoUi, UI_FIFO, users[i].pid);
		int result = sendTo(data, fifoUi);
    if(result == 1)
      printf("%s\nERRO - nao foi possivel abrir %s\n%s", C_ERROR, fifoUi, C_CLEAR);
    else if(result == -1)
      printf("%s\nERRO - falha no envio%s\n", C_ERROR, C_CLEAR);
  }

  *gameStarted = true;
  return true;

}

void endGame(userInfo* users, int users_count){

  // avisa os UIs todos
  sharedData data;
  strcpy(data.cmd.command, "end_game");
  strcpy(data.error, "O jogo comecera em pouco tempo");

  char fifoUi[MAX_STRING];
  for(int i=0; i < users_count; i++){
    if(users[i].inGame){

      sprintf(fifoUi, UI_FIFO, users[i].pid);
      int result = sendTo(data, fifoUi);
      if(result == 1)
        printf("%s\nERRO - nao foi possivel abrir %s\n%s", C_ERROR, fifoUi, C_CLEAR);
      else if(result == -1)
        printf("%s\nERRO - falha no envio%s\n", C_ERROR, C_CLEAR);

    }
    
  }

}
