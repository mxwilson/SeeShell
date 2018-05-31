/*
builtins.c - part of seeshell - bash-like unix shell replacement
 
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
No warranty. Software provided as is.
Copyright Matthew Wilson, 2016-2018.
*/

#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<pwd.h>
#include<fcntl.h>
#include<ctype.h>
#include"shheader.h"

int builtin_checker(char* token[20], int argc) {
	struct passwd *pw;
	char thedir[99];

	if (strcmp(token[0], "cd") == 0) {
		// cd and cd ~
		//if ((argc == 1) || (argc == 2) && (strcmp(token[1], "~") == 0)) {
		if (argc == 1 || (argc == 2 && (strcmp(token[1], "~") == 0))) {
			pw = getpwuid(getuid());
			if (chdir(pw->pw_dir) != 0) {
				printf("%s: %s\n", token[0], strerror(errno));
				return(0);
			}
		}
		// cd ~/dir/
		else if ((argc == 2) && (strncmp(token[1], "~/", 2) == 0)) {
			pw = getpwuid(getuid());
			strcpy(thedir, pw->pw_dir);
			strcat(thedir, token[1]+1);

			if (pw == NULL) {
				printf("%s: No such file or directory\n", token[1]+1);
			}
			if (chdir(thedir) != 0) {
				printf("%s: %s\n", token[0], strerror(errno));
				return(0);
			}
		}
		// cd ~user
		else if ((argc == 2) && (strncmp(token[1], "~", 1) == 0)) {
			char* cdtokens[10];
			int x = 0;
			
			cdtokens[x] = strtok(token[1]+1, "/");
		
			while(cdtokens[x] != NULL) {
				x++;
				cdtokens[x] = strtok(NULL, "/");
			}
			
			struct passwd *pw2;
			pw2 = getpwnam(cdtokens[0]);
				
			if (pw2 == NULL) {
				printf("%s: No such file or directory\n", token[1]+1);
				return(0);
			}
			// cd ~user	
			if (x == 1) {
				strcpy(thedir, pw2->pw_dir);
				//printf("thedir: %s\n", thedir);
			}	
			// cd ~user/dir
			if (x != 1) {
				int y;
				strcpy(thedir, pw2->pw_dir);
				//printf("0thedir: %s\n", thedir);
		
				for (y = 1; y < x; y++) {		
					strcat(thedir, "/");	
					strcat(thedir, cdtokens[y]);
					//printf("ee %d %s\n", y, cdtokens[y]);
				}
			}
			// error out 
			if (chdir(thedir) != 0) {
				printf(":) %s: %s\n", thedir, strerror(errno));
				//return(0);
			}	
		}
		// cd /somedir
		else if (argc == 2) {
			if (chdir(token[1]) != 0) {
				printf("%s: %s\n", token[0], strerror(errno));
				//return(0);
			}
		}
		// or attempt to cd(token0) to somedir(token1)
		else if ((argc == 2) && (strncmp(token[1], "~", 1) != 0)) {
			if (chdir(token[1]) != 0) {
				printf("%s: %s\n", token[1], strerror(errno));
				//return(0);
			}
		}
	}
	// or check for "pwd"
	else if (strcmp(token[0], "pwd") == 0) {
		char cwd[1024];
		if (getcwd(cwd, sizeof(cwd)) != NULL) {
			printf("%s\n", cwd);
		}
	}
	// clear screen
	else if ((strcmp(token[0], "cl") == 0) || (strcmp(token[0], "clear") == 0)) {
		//system("clear");
		printf("\033c");
	}
	// history
	else if (strcmp(token[0], "history") == 0) { 
		if (argc == 1) {
			history(NULL, 1, 0, 0); // show history
		}
		if (argc > 1) {	
			int mynum = atoi(token[1]);
			if (strcmp(token[1], "-c") == 0) {
				history(NULL, 1, 1, 0); // or delete history
			}
			// or show n history items
			else if (mynum) {
				history(NULL, 1, 2, mynum);
			}
			else {
				printf("history syntax err\n");
			}
		}
		//return(0);
	}
	else if (strcmp(token[0], "help") == 0) {
		printf("go to help\n");
		//return(0);
	}
	else if (strcmp(token[0], "exit") == 0) {
		exit(EXIT_SUCCESS);
	}
	// no builtin found, search for coreutil
	else {
		return(1);
	}
	return(0);
}
