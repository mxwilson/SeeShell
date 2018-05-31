/*
seeshell.c - bash-like unix shell replacement
 
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
No warranty. Software provided as is.
Copyright Matthew Wilson, 20116.
*/

#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<pwd.h>
#include<fcntl.h>
#include<signal.h>
#include<sys/wait.h>
#include"shheader.h" 

#define READ_END 0
#define WRITE_END 1

char* HISTFILE = "/home/mx/code/SeeShell-new/.shellhistory";
const char bindir[1048] = "/shell/bin/";

// if in 1st child, close read end
int pipe_write(char* cmdtorun[20], int fd[2], int cmdcnt) {
        //printf("in child:%d process:%d\n", 1, getpid());
	// if just single command, do not do piping routine  
	if (cmdcnt > 1) { 
		close(fd[READ_END]);
        	dup2(fd[WRITE_END], STDOUT_FILENO);
        	close(fd[WRITE_END]);
 	}
	if (execvp(cmdtorun[0], cmdtorun) == -1) {
		printf("error: %s\n", strerror(errno));
		_exit(EXIT_FAILURE);
	}
  	else {
		_exit(EXIT_SUCCESS);
	}
}

// if in 2nd child, close write end
int pipe_read(char* cmdtorun[20], int fd[2]) {
        //printf("in child:%d process:%d\n", 2, getpid());
        close(fd[WRITE_END]);
        dup2(fd[READ_END], STDIN_FILENO);
        close(fd[READ_END]);
        
	if (execvp(cmdtorun[0], cmdtorun) == -1) {
		printf("error: %s\n", strerror(errno));
		_exit(EXIT_FAILURE);
	}
	else {	
	        _exit(EXIT_SUCCESS);
        }
}

// some routine tests before starting
int program_init() {
	// start with the history file
	FILE* fh;

	fh = fopen(HISTFILE, "a+");
	
	if (fh == NULL) {
		printf("history file error: %s\n%s\n", HISTFILE, strerror(errno));
		return(1);
	}		
	fclose(fh);
	return(0);
}

int main(int argc, char* argv[]) {
	if (program_init() == 1) {
		exit(EXIT_FAILURE);
	}
	printf("Welcome to seeshell\n");
	while(1) {
		if (prompt() == 0) {
			exit(EXIT_SUCCESS);
		}
		else {
			printf("fail!\n");
			exit(EXIT_FAILURE);
		}
	}
}
