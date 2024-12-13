#ifndef TP_JOGOUI_FUNC_H
#define TP_JOGOUI_FUNC_H

#include "../global.h"

// funcoes
char* getError();
// retorna o ultimo erro ocorrido no sistema

void setError(char* message);
// reescreve o erro

bool ini(int* fd, sharedData data);
// inicia a UI
// retorna:     true - inicie
//              false - em caso de algum erro

bool closeUI(int* fd, sharedData data, bool kicked);
// fecha a UI
// retorna:     true - feche
//              false - em caso de algum erro

int checkCMD(prompt* prmt);
// verifica o comando pedido pelo Admin
// retorna:     o codigo do comando
//              CMD_ERROR, em caso de algum erro

bool checkWon(userInfo* user, bool* won);
// verifica se o Jogador ja acabou o nivel
// retorna:   true - acabou
//            false - caso ainda nao tenha acabado

bool movePlayer(int direction, userInfo* user, gameLevel level);
// move o Jogador
// retorna:   true - mova
//            false - caso nao possa se mover

void printHelp();
// imprime a lista de comandos

#endif //TP_JOGOUI_FUNC_H
