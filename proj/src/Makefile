# Makefile for proj

COMPILER_TYPE= gnu

CC= gcc

PROG= proj
SRCS= proj.c timer.c rtc.c video_gr.c mouse.c menu.c keyboard.c vbe.c read_xpm.c bitmap.c game.c highscores.c mouse_ih.S rtc_ih.S uart_ih.S uart.c init_game.c

CFLAGS= -Wall

DPADD+= ${LIBDRIVER} ${LIBSYS}
LDADD+= -llm -ldriver -lsys

LDFLAGS+= -L .

MAN=

BINDIR?= /usr/sbin

.include <bsd.prog.mk>
.include <bsd.gcc.mk>