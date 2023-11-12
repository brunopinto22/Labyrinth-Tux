#include "style.h"

WINDOW* title;
WINDOW* score;
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

  title = newwin(3, WN_SIZE_COLS, 0, 0);

  score = newwin(NUM_LINES+WN_SIZE_IN_OUT*2, WN_SIZE_SIDES, 3, 0);

  map = newwin(NUM_LINES, NUM_COLS, 3, WN_SIZE_SIDES);
  input = newwin(WN_SIZE_IN_OUT, NUM_COLS, NUM_LINES+3, WN_SIZE_SIDES);
  output = newwin(WN_SIZE_IN_OUT, NUM_COLS, NUM_LINES+WN_SIZE_IN_OUT+3, WN_SIZE_SIDES);

  messages = newwin(NUM_LINES+WN_SIZE_IN_OUT*2, WN_SIZE_SIDES, 3, WN_SIZE_SIDES+NUM_COLS);

  keypad(map, TRUE);
  wmove(input, 1, 1);

}

void printOneWindow(WINDOW* window, bool lock){
  if(lock)
    scrollok(window, TRUE); // liga o scroll na "janela".
  else {
    keypad(window, TRUE); // para ligar as teclas de direção (aplicar à janela)
    wclear(window);// limpa a janela
  }

  wborder(window, '|', '|', '-', '-', '+', '+', '+', '+'); // Desenha uma borda. Nota importante: tudo o que escreverem, devem ter em conta a posição da borda
  refresh(); // necessário para atualizar a janela
  wrefresh(window); // necessário para atualizar a janela
}

void printWindow(sharedData* data, bool reading){

  printOneWindow(title, false);
  printOneWindow(score, false);
  printOneWindow(output, false);
  printOneWindow(messages, false);

  printOneWindow(map, !reading);
  printOneWindow(input, reading);

  wmove(input, 2, 2);

}

int readKeyboard(){

  printOneWindow(input, false);
  wrefresh(input);
  wmove(input, 1, 1);

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
    wprintw(input, ">> ");
    wgetstr(input, string);
    noecho();
    wrefresh(input);

    sscanf(string, "%s %[^\n]", cmd.command, cmd.args);

    printOutput(cmd.command);

    return checkCMD(&cmd);
  }

  return CMD_ERROR;

}

void printOutput(char* message){

  //printOneWindow(output, false);
  wprintw(output, "%s", message);
  wrefresh(output);

}

void closeWindow(){
  wclear(title);
  wrefresh(title);
  delwin(title);

  wclear(score);
  wrefresh(score);
  delwin(score);

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