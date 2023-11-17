all: motor jogoUI testBot

clean:
	rm motor jogoUI testbot FIFO*

motor:
	gcc -o motor src/Motor/motor.c src/global.c src/Motor/motor_func.c 

jogoUI:
	gcc -o jogoUI src/JogoUI/jogoUI.c src/JogoUI/style.c src/global.c src/JogoUI/jogoUI_func.c -lncurses

testBot:
	gcc -o testbot src/test_bot.c
