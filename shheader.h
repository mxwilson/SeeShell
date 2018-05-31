#ifndef SHHEADER_H
#define SHHEADER_H

/* prototypes for each additional function/file */
int history(char line[1024], int builtin, int histoption, int reccount); /* history.c */
int prompt(void); /* prompt.c */
int arg_checker(char line[1024]); /* argchecker.c */
int builtin_checker(char* token[99], int argc); /* builtins.c */

/* other prototypes */
int pipe_write(char* cmdtorun[20], int fd[2], int cmdcnt);
int pipe_read(char* cmdtorun[20], int fd[2]);

/* Global vars from main */
extern char* HISTFILE;
extern const char bindir[];

#endif
