#ifndef TP_MOTOR_FUNC_H
#define TP_MOTOR_FUNC_H

#include "global.h"

// funcoes
char* getError();
// retorna o ultimo erro ocorrido no sistema

bool ini(int* fd, envVariables* gameSettings);
// inicia o servidor
// retorna:     true - inicie
//              false - em caso de algum erro

bool closeMotor(int* fd);
// fecha o servidor
// retorna:     true - feche
//              false - em caso de algum erro

int checkCMD(prompt* prmt);
// verifica o comando pedido pelo Admin
// retorna:     CMD_ERROR
//              USERS
//              KICK
//              BOTS
//              BMOV
//              RBM
//              BEGIN
//              END

int checkCMD_UI(prompt* prmt);
// verifica o comando pedido pela UI
// retorna:     CMD_ERROR
//              LOGIN
//              CHANGE_RD_WR
//              PLAYERS
//              MSG
//              EXIT
//              UP
//              DOWN
//              RIGHT
//              LEFT

void printSettings(envVariables* gameSettings);
// imprime os valores das definicoes do jogo

void printHelp();
// imprime a lista de comandos

bool sendTo(sharedData* data, userInfo* user);
// envia para uma UI dados
// retorna:     true - envie
//              false - caso falhe

#endif //TP_MOTOR_FUNC_H