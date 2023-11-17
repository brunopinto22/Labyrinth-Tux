#include "style.h"

WINDOW* title;
WINDOW* scoreBoard;
WINDOW* map;
WINDOW* input;
WINDOW* output;
WINDOW* messages;

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

  scoreBoard = newwin(NUM_LINES+WN_SIZE_IN_OUT, WN_SIZE_SIDES, 3, 0);

  map = newwin(NUM_LINES, NUM_COLS, 3, WN_SIZE_SIDES);
  input = newwin(WN_SIZE_IN_OUT, NUM_COLS, NUM_LINES+3, WN_SIZE_SIDES);
  output = newwin(WN_SIZE_IN_OUT, WN_SIZE_COLS, 3+NUM_LINES+WN_SIZE_IN_OUT, 0);

  messages = newwin(NUM_LINES+WN_SIZE_IN_OUT, WN_SIZE_SIDES, 3, WN_SIZE_SIDES+NUM_COLS);

  keypad(map, TRUE);
  wmove(input, 2, 2);

}

void printOneWindow(WINDOW* window, bool lock){
  if(lock)
    scrollok(window, TRUE); // liga o scroll na "janela".
  else {
    keypad(window, TRUE); // para ligar as teclas de direção (aplicar à janela)
    wclear(window);// limpa a janela
  }

  keypad(window, TRUE);
  wborder(window, '|', '|', '-', '-', '+', '+', '+', '+'); // Desenha uma borda. Nota importante: tudo o que escreverem, devem ter em conta a posição da borda
  refresh(); // necessário para atualizar a janela
  wrefresh(window); // necessário para atualizar a janela
}

void printWindow(sharedData* data){

  printOneWindow(title, false);
  printOneWindow(scoreBoard, false);
  printOneWindow(output, false);
  printOneWindow(messages, false);

  printOneWindow(map, true);
  printOneWindow(input, false);

  printTitle(data->user, 0, 0);

  wmove(input, 2, 2);

}

int readKeyboard(){

  printOneWindow(input, false);
  mvwprintw(input, 1, 1, ">> ");
  wrefresh(input);

  int key = wgetch(map);
  char string[100];
  prompt cmd;
  cmd.command[0] = 0;
  cmd.args[0] = 0;

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

    sscanf(string, "%s %[^\n]", cmd.command, cmd.args);

    return checkCMD(&cmd);
  }

  return CMD_ERROR;

}

void printTitle(userInfo user, int time, int score){

  printOneWindow(title, false);
  mvwprintw(title, 1, 1, "\t%s[%d]\tTimer: %d\tScore: %d", user.name, user.pid, time, score);
  wrefresh(title);
  wmove(input, 2, 2);

}

void printOutput(char* message, bool isError){

  printOneWindow(output, false);
  mvwprintw(output, 1, 1, "%s", message);
  wrefresh(output);
  wmove(input, 1, 1);

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