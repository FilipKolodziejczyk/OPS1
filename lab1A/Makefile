CC=gcc
C_FLAGS=-Wall -fsanitize=address,undefined

largest_file: largest_file.c
	${CC} ${C_FLAGS} -o largest_file.out largest_file.c

.PHONY: clean
clean:
	rm -rf largest_file.out ${L1_LOGFILE}
