#ifndef TP_MOTOR_FUNC_H
#define TP_MOTOR_FUNC_H

#include "global.h"

// funcoes
char* getError();
// retorna o ultimo erro ocorrido no sistema

bool ini(int* fd, envVariables* gameSettings, gameLevel* maps);
// inicia o motor
// retorna:     true - inicie
//              false - em caso de algum erro

bool setGameSettings(envVariables* gameSettings);
// inicia as definicoes do jogo
// retorna:     true - inicie
//              false - em caso de algum erro

bool loadMaps(gameLevel levels[MAX_LEVELS], envVariables* gameSettings);
// inicia os maps
// retorna:     true - inicie
//              false - em caso de algum erro

bool closeMotor(int* fd);
// fecha o motor
// retorna:     true - feche
//              false - em caso de algum erro

int checkCMD(prompt* prmt);
// verifica o comando pedido pelo Admin
// retorna:     o codigo do comando
//              CMD_ERROR, em caso de algum erro

int checkCMD_UI(prompt* prmt);
// verifica o comando pedido pela UI
// retorna:     o codigo do comando
//              CMD_ERROR, em caso de algum erro

void printMap(int level, gameLevel* levels);
// imprime o mapa com as suas respetivas definicoes

void printSettings(envVariables* gameSettings);
// imprime os valores das definicoes do jogo

void printHelp();
// imprime a lista de comandos

bool sendTo(sharedData* data, userInfo* user);
// envia para uma UI dados
// retorna:     true - envie
//              false - caso falhe

#endif //TP_MOTOR_FUNC_H