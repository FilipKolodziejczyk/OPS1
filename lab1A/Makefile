CC=gcc
C_FLAGS=-Wall -fsanitize=address,undefined -Wno-unused-variable
FILES=l1solution.o l1solution_util.o

l1solution: ${FILES}
	${CC} ${C_FLAGS} -o l1solution ${FILES}
	rm -f ${FILES}

l1solution.o: l1solution.c l1solution.h
	${CC} ${C_FLAGS} -o l1solution.o -c l1solution.c

l1solution_util.o: l1solution_util.c l1solution.h
	${CC} ${C_FLAGS} -o l1solution_util.o -c l1solution_util.c

.PHONY: clean
clean:
	rm -rf l1solution ${FILES}
