#ifndef TP_MOTOR_FUNC_H
#define TP_MOTOR_FUNC_H

#include "../global.h"

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

void printUsers(userInfo* users_list, int users_count);
// imprime os utilizadores ligados ao motor

pid_t userExists(char* user_to_find, userInfo* users_list, int users_count);
// verifica se o utilizador existe no sistema
// retorna:     o pid do utilizador
//              0 - em caso de algum erro

bool addUser(sharedData* data, userInfo* users_list, int* users_count, int* inGameUsers, bool gameStarted);
// adiciona um novo utilizador ao motor
// retorna:     true - adicione
//              false - em caso de algum erro

int kickUser(char* user_name, userInfo* users_list, int* users_count, int* inGameUsers);
// expulsa um utilizador do motor
// retorna:     o pid do utilizador expulso
//              0 - em caso de algum erro

void updateUsersMove(sharedData* data, userInfo* users_list, int users_count);
// atualiza a posicao do utilizador e avisa os outros do seu movimento

void closeUIs(userInfo* users, int users_count);
// fecha todas as UIs ligadas ao motor

bool setGameSettings(envVariables* gameSettings);
// define as variaveis do jogo
// retorna:     true - defina
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

bool begin(bool* gameStarted, userInfo* users, int users_count, gameLevel levels[MAX_LEVELS], int currentGame);
// inicia o jogo
// retorna:     true - inicie
//              false - caso de algum erro

void runGame(gameLevel levels[MAX_LEVELS], envVariables* gameSettings, userInfo* users, int users_count);
// roda o Jogo


#endif //TP_MOTOR_FUNC_H