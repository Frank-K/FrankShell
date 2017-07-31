all: fshell

fshell : fshell.o
	gcc -Wall -o fshell fshell.o 
	
fshell.o : fshell.c
	gcc -Wall -c $<
