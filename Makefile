CC = gcc

OUTFILE	 = seeshell
INFILES = $(wildcard *.c)
CFLAGS  = -g -Wall

install:
	$(CC) $(CFLAGS) $(INFILES) -o $(OUTFILE)
