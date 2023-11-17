#ifndef TP_STYLE_H
#define TP_STYLE_H

#include <ncurses.h>
#include "jogoUI_func.h"
#include "../global.h"

#define WN_SIZE_SIDES 30
#define WN_SIZE_COLS (NUM_COLS+WN_SIZE_SIDES*2)
#define WN_SIZE_IN_OUT 7

#define ERROR_COLOR 1
#define TITLE_COLOR 2

void setupWindow();
// inicia ncurses

void printWindow(sharedData* data);
// imprime as janelas

int readKeyboard(); 
// ler o teclado

void printTitle(userInfo user, int time, int score);
// imprime o titulo

void printOutput(char* message, bool isError);
// imprime o output

void printMessage(char* message);
// imprime a mensagem recebida

void printScore(char* score);
// imprime a pontuacao

void clearWindow();
// limpa as janelas

void closeWindow();
// fecha as janelas

#endif //TP_STYLE_H