/*
argchecker.c - part of seeshell - bash-like unix shell replacement
 
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
No warranty. Software provided as is.
Copyright Matthew Wilson, 20116.
*/

#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<sys/wait.h>
#include"shheader.h" 
	
// check to see if the command exists. if not, return NULL, go back to prompt
char* cmd_existor(char* tempcmd[20]) {
        int a;
        //char bindir[1048] = "/xxx/shell/bin/";
        // bindir: home of our coreutils - now global variable
	char tempbindir[1048];
	char* backupbindirs[4] = {"/bin/", "/usr/bin/", "/usr/sbin/", "/sbin/"};
        char prog_to_run[1048];
       
	strcpy(tempbindir, bindir);	
	strcat(tempbindir, tempcmd[0]);
        strcpy(prog_to_run, tempbindir);
     
	// first try our coreutils. if not exist, look for system-provided util
        if (access(prog_to_run, F_OK) == 0) {
		//printf("progrun: %s\n", prog_to_run);
		tempcmd[0] = strdup(prog_to_run);	
		return(tempcmd[0]);
	}
	else {	
		for (a = 0; a < 4; a++) {
                	prog_to_run[0] = '\0';
                        strcat(prog_to_run, backupbindirs[a]);
                        strcat(prog_to_run, tempcmd[0]);

                        if (access(prog_to_run, F_OK) == 0) {
                                //printf("progrun: %s\n", prog_to_run);
				tempcmd[0] = strdup(prog_to_run);
				return(tempcmd[0]);
			}
                        else {
                                continue;
                        }
                }
		return(NULL);
	}
}

int arg_checker(char line[1024]) {
	pid_t pid = 0;
	int fd[2];

	char user_input[1024];
	char liline[1024];
	char* token[99];
	int cnt = 0;
	int x;
	int i;
	int len;
	int whitespace;

	// error if line starts with a pipe
	if (strncmp(line, "|", 1) == 0) {	
		printf("syntax error: unexpected token '|'\n");
		return(1);
	}

	// first pass removes leading whitespace
	strcpy(user_input, line);

	token[0] = strtok(user_input, "|");
	while (token[cnt] != NULL) {
		cnt++;
		token[cnt] = strtok(NULL, "|");
	}
	
	for (x = 0; x < cnt; x++) {
		whitespace = 0;
		strcpy(liline, token[x]);
		len = strlen(liline);
	
		for (i = 0; i < len; i++) {
			if (liline[i] == ' ') {
				whitespace++;
			}
			if (liline[i] != ' ') { // stop at first whitespace occurance
				break;
			}	
		}
		if (whitespace > 0) {
			token[x] = strdup(liline + whitespace);
		}	
	}

	// second pass now remove trailing whitespace
	for (x = 0; x < cnt; x++) {
		whitespace = 0;
		char templine[150] = { 0 };
		strcpy(liline, token[x]);
		len = strlen(liline);

		// go backwards thru string until non-whitespace is found
		for (i = len - 1; i >= 0; --i) {
			if (liline[i] == ' ') {
				whitespace++;				
			}
			if (liline[i] != ' ') {
				break;
			}
		}
		if (whitespace > 0) {
			strncat(templine, liline, len - whitespace);
			token[x] = strdup(templine);
		}	
	}

	// this will print out resulting commands
	//for (x = 0; x < cnt; x++) {
	//	printf("%d *%s*\n", x, token[x]); 
	//}

	//printf("num of commands: %d num of pipes: %d\n", cnt, cnt-1);
	// reconstructing this format: char *cmd[] = { "/bin/ls", "ls", "-al", 0 };

	char* tempcmd[20];
	char tempyline[150];
	int newcnt;
	int status;

	// set up the pipe
	if (pipe(fd) == -1) {
		printf("pipe error\n");
		exit(EXIT_FAILURE);
	}

	// tokenize each command group by space cmd0:ls, 1:-la, etc
	for (x = 0; x < cnt; x++) {
		newcnt = 0;
		strcpy(tempyline, token[x]);
		tempcmd[0] = strtok(tempyline, " ");

		while (tempcmd[newcnt] != NULL) {
			//printf("tempcmd %d %s\n", newcnt, tempcmd[newcnt]);
			newcnt++;
			tempcmd[newcnt] = strtok(NULL, " ");
		}

		// check for builtins, and get out of this loop if fiding one
		if (builtin_checker(tempcmd, newcnt) == 0) {
			return(1);
        	}

		// give last array null value as required by exec
		tempcmd[newcnt] = NULL;

		// going to test the commands before continuing
		if (! cmd_existor(tempcmd)) {
			printf("%s: command not found :(\n", tempcmd[newcnt - 1]);
			
			// if first command exist, second nonexist, prevent zombie child
			
			if (x > 0) {
				while (wait(&status) != pid) {
                			;
				}
			}
			return(1);	
		}
		//printf("%s\n", tempcmd[0]);
		
		// support for 1 pipe, ie cmd1|cmd2			
		if (x == 0) {
			pid = fork();
			if (pid == 0) {
				pipe_write(tempcmd, fd, cnt);
			}
		}
		if (x != 0 && cnt > 1) {
			pid = fork();
			if (pid == 0) {
				pipe_read(tempcmd, fd);
			}
		}	
	} 
	// these are key. after loop, close fd1 and wait	
	close(fd[1]);

        while (wait(&status) != pid) {
                ;
        }
	
	return(0);
}	
