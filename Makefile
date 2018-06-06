CC = gcc

OUTFILE	 = seeshell
INFILES = $(wildcard *.c)
CFLAGS  = -g -Wall 
LFLAGS = -lreadline 
install:
	$(CC) $(CFLAGS) $(INFILES) -o $(OUTFILE) $(LFLAGS)
