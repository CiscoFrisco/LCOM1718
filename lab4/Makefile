# Makefile for lab4

COMPILER_TYPE= gnu

CC= gcc

PROG= lab4
SRCS= lab4.c test4.c mouse.c timer.c

CFLAGS= -Wall

DPADD+= ${LIBDRIVER} ${LIBSYS}
LDADD+= -ldriver -lsys

LDFLAGS+= -L .
CPPFLAGS += -D LAB4

MAN=

.include <bsd.prog.mk>
.include <bsd.gcc.mk>