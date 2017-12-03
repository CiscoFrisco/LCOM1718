# Makefile for lab3

COMPILER_TYPE= gnu

CC= gcc

PROG= lab3
SRCS= lab3.c keyboard.c test3.c timer.c ass.S

CFLAGS= -Wall

DPADD+= ${LIBDRIVER} ${LIBSYS}
LDADD+= -ldriver -lsys

LDFLAGS+= -L .
CPPFLAGS += -D LAB3

MAN=

.include <bsd.prog.mk>
.include <bsd.gcc.mk>