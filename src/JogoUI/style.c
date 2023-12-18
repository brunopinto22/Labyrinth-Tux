#include "style.h"

WINDOW* title;
WINDOW* scoreBoard;
WINDOW* map;
WINDOW* input;
WINDOW* output;
WINDOW* messages;

int count_msg = 0;

void setupWindow(){

  initscr(); // Obrigatorio e sempre a primeira operação de ncurses
  raw();  // desativa o buffer de input, cada tecla é lida imediatamente
  noecho();  // desliga o echo no ecrã, para voltar ativar devem utilizar a função echo();
  keypad(stdscr, TRUE);  // habilita as teclas  especiais
  attrset(A_DIM);  //altera o brilho no print das janelas

  start_color();  // inicar cor
  // definir cores
  init_pair(ERROR_COLOR, COLOR_RED, COLOR_BLACK);
  init_pair(TITLE_COLOR, COLOR_CYAN, COLOR_BLACK); 

  title = newwin(3, WN_SIZE_COLS, 0, 0);

  scoreBoard = newwin(MAP_LINES+WN_SIZE_IN_OUT, WN_SIZE_SIDES, 3, 0);

  map = newwin(MAP_LINES, MAP_COLS, 3, WN_SIZE_SIDES);
  input = newwin(WN_SIZE_IN_OUT, MAP_COLS, MAP_LINES+3, WN_SIZE_SIDES);
  output = newwin(WN_SIZE_IN_OUT, WN_SIZE_COLS, 3+MAP_LINES+WN_SIZE_IN_OUT, 0);

  messages = newwin(MAP_LINES+WN_SIZE_IN_OUT, WN_SIZE_SIDES, 3, WN_SIZE_SIDES+MAP_COLS);

  keypad(map, TRUE);
  wmove(input, 2, 2);

}

void printOneWindow(WINDOW* window, bool lock, bool border){
  if(lock)
    scrollok(window, TRUE); // liga o scroll na "janela".
  else {
    keypad(window, TRUE); // para ligar as teclas de direção (aplicar à janela)
    wclear(window);// limpa a janela
  }

  keypad(window, TRUE);

  if(border)
    wborder(window, '|', '|', '-', '-', '+', '+', '+', '+'); // Desenha uma borda. Nota importante: tudo o que escreverem, devem ter em conta a posição da borda

  refresh(); // necessário para atualizar a janela
  wrefresh(window); // necessário para atualizar a janela
}

void printWindow(){

  printOneWindow(title, false, true);
  printOneWindow(scoreBoard, false, true);
  printOneWindow(output, false, true);
  printOneWindow(messages, false, true);

  printOneWindow(map, true, false);
  printOneWindow(input, false, true);

  wmove(input, 2, 2);

}

int readKeyboard(prompt* prmt){

  printOneWindow(input, false, true);
  mvwprintw(input, 1, 1, ">> ");
  wrefresh(input);

  int key = wgetch(map);
  char string[100];
  prmt->command[0] = 0;
  prmt->args[0] = 0;

  if (key == KEY_UP)
    return UP;
  else if (key == KEY_RIGHT)
    return RIGHT;
  else if (key == KEY_LEFT)
    return LEFT;
  else if (key == KEY_DOWN)
    return DOWN;
  else if (key == ' '){
    echo();
    wgetstr(input, string);
    noecho();
    wrefresh(input);

    sscanf(string, "%s %[^\n]", prmt->command, prmt->args);

    return checkCMD(prmt);
  }
  
  setError("essa tecla nao e reconhecida pelo sistema");
  return CMD_ERROR;

}

void printTitle(char* message){

  printOneWindow(title, false, true);
  mvwprintw(title, 1, WN_SIZE_COLS/2-strlen(message)/2, "%s", message);
  wrefresh(title);
  wmove(input, 1, 1);

}

void printOutput(char* message, bool isError){

  if(isError)
    wattrset(output, COLOR_PAIR(COLOR_RED));
  else
    wattrset(output, COLOR_PAIR(COLOR_BLACK));

  printOneWindow(output, false, true);
  mvwprintw(output, 1, 1, "%s", message);
  wrefresh(output);
  wmove(input, 1, 1);

}

void printMessage(char* message){

  if(++count_msg == MAP_LINES+WN_SIZE_IN_OUT-2){
    count_msg = 1;
    printOneWindow(messages, false, true);
  }
  mvwprintw(messages, count_msg, 1, "%s", message);
  wrefresh(messages);
  wmove(input, 1, 1);

}

void printMap(gameLevel current_level){
  
  for (int lin=0; lin < MAP_LINES-1; lin++)
    for(int col=0; col < NUM_COLS; col++)
      mvwprintw(map, lin+1, col+1, "%c", current_level.map[col][lin]);

  wrefresh(map);
  wmove(input, 2, 1);
}

void printUserOnMap(userInfo* user){

  mvwprintw(map, user->coords.x+1, user->coords.y+1, "%c", user->name[0]);
  wmove(map, user->coords.x+1, user->coords.y+1);
  wrefresh(map);

}

void closeWindow(){
  echo();

  wclear(title);
  wrefresh(title);
  delwin(title);

  wclear(scoreBoard);
  wrefresh(scoreBoard);
  delwin(scoreBoard);

  wclear(output);
  wrefresh(output);
  delwin(output);

  wclear(messages);
  wrefresh(messages);
  delwin(messages);

  wclear(map);
  wrefresh(map);
  delwin(map);

  wclear(input);
  wrefresh(input);
  delwin(input);

  endwin();
}