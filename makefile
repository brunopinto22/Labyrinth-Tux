all: motor jogoUI

clean:
	rm motor jogoUI FIFO*

motor:
	gcc src/motor.c src/global.c src/motor_func.c -o motor

jogoUI:
	gcc -o jogoUI src/jogoUI.c src/global.c src/jogoUI_func.c -lncurses


