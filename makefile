# Motor
MOTOR_OBJS=src/Motor/motor.c src/global.c src/Motor/motor_func.c

# JogoUI
JOGOUI_OBJS=src/JogoUI/jogoUI.c src/JogoUI/style.c src/global.c src/JogoUI/jogoUI_func.c


all: motor jogoUI

clean:
	rm motor jogoUI FIFO*

motor: $(MOTOR_OBJS)
	gcc -o motor $(MOTOR_OBJS)

jogoUI: $(JOGOUI_OBJS)
	gcc -o jogoUI $(JOGOUI_OBJS) -lncurses