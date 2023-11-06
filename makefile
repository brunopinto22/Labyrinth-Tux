all: motor jogoUI

clean:
	rm motor jogoUI FIFO*

motor:
	gcc src/motor.c src/motor_func.c -o motor

jogoUI:
	gcc src/jogoUI.c src/jogoUI_func.c -o jogoUI


