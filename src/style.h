#ifndef TP_STYLE_H
#define TP_STYLE_H

#include "global.h"
#include "jogoUI_func.h"
#include <ncurses.h>

#define WN_SIZE_SIDES 30
#define WN_SIZE_COLS (NUM_COLS+WN_SIZE_SIDES*2)
#define WN_SIZE_IN_OUT 7

void setupWindow();
// inicia ncurses

void printWindow(sharedData* data, bool reading);
// imprime as janelas

int readKeyboard(); 
// ler o teclado

void printOutput(char* message);
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