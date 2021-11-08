prog1: prog1.o
	gcc prog1.o -o prog1 

prog1.o: prog1.c
	gcc -c prog1.c -o prog1.o

.PHONE: clean clean_all

clean:
	-rm -f prog1.o

clean_all:
	-rm -f prog1.o prog1