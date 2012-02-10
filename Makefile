CC=gcc
FLAGS=-Wall -Wextra -c -std=c89
DEBUG_FLAGS=${FLAGS} -DDEBUG -g

all:
	${CC} ${FLAGS} proc.c
	${CC} ${FLAGS} sema.c

test:
	${CC} ${DEBUG_FLAGS} proc.c
	${CC} ${DEBUG_FLAGS} tp1test.c
	${CC} -o tp1test proc.o tp1test.o
