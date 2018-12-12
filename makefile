all: tele.o
	gcc -o tele tele.o
tele.o: tele.c
	gcc -c tele.c
