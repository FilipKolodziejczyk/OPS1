OUT_FILES=prog1 prog2 prog3 prog4 prog5 prog6 prog7 prog8 prog9 prog10 prog11 prog12

all: ${OUT_FILES}

prog1: prog1.c
	gcc -Wall -fsanitize=address,undefined -o prog1 prog1.c

prog2: prog2.c
	gcc -Wall -fsanitize=address,undefined -o prog2 prog2.c

.PHONY: clean_all all

clean_all:
	rm ${OUT_FILES}