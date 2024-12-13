#ifndef TP_CONSTANTS_H
#define TP_CONSTANTS_H

// sistema
#define MOTOR_FIFO "FIFO_MOTOR"
#define UI_FIFO "FIFO_UI%d"
#define DEFAULT_LEVEL_FILE "LEVEL_DEFAULT"
#define LEVEL_FILE "LEVEL%d"
#define MAX_USERS 5
#define MAX_BOTS 10
#define MAX_PEDRAS 50
#define MAX_LEVELS 3
#define MAX_BMOV 5
#define NUM_COLS 40
#define NUM_LINES 17
#define MAX_TIME_BOT 60

// vars de ambiente
#define TIME_REG "INSCRICAO"
#define MIN_USERS "NPLAYERS"
#define TIMER "DURACAO"
#define TIMER_DECREMENT "DECREMENTO"

// funcional
#define MAX_STRING 100
#define MAX_ERROR MAX_STRING*3
#define RD 0
#define WR 1

// comandos
#define CMD_ERROR -1
#define HELP 11
#define WON 12
#define MOVE 13
#define TIME 14
#define CLEAR 22
// motor:
#define USERS 1
#define KICK 2
#define BOTS 3
#define BMOV 4
#define RBM 5
#define BEGIN 6
#define SETTINGS 7
#define MAP 8
#define END 0
// ui:
#define LOGIN 1
#define CHANGE_RD_WR 2
#define PLAYERS 3
#define MSG 4
#define UP 5
#define DOWN 6
#define RIGHT 7
#define LEFT 8
#define KICKED 9
#define ENDGAME 10
#define EXIT 0

// cores
#define C_CLEAR "\033[0m"
#define C_ERROR "\033[0;31m"
#define C_FERROR "\033[3;31m"
#define C_FATAL_ERROR "\033[4;31m"
#define C_ONLINE "\033[0;32m"
#define C_IDLE "\033[0;34m"
#define C_MESSAGE "\033[0;33m"
#define C_MOTOR "\033[0;35m"
#define C_UI "\033[0;36m"


#endif //TP_CONSTANTS_H