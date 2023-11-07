#include "global.h"

int sendTo(sharedData data, char* destiny){

  int fd = open(destiny, O_WRONLY);
	if(fd == -1) // nao foi possivel abrir o fifo
		return 1;

  // envia a mensagem
	int s = write(fd, &data, sizeof(data));
	close(fd);

  // verifica o envio
	if(s <= 0)
		return -1;

	return 0;

}