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


// forking

child_process(char* token[99]) {
	//printf("Now in the child process\n");
	char bindir[1048] = "./bin/";
	char prog_to_run[1048]; 
	strcat(bindir, token[0]);
	strcpy(prog_to_run, bindir);
	
	//printf("prog to run: %s\n", prog_to_run);
	// run the concatenated program ie ./bin/progname. 
	// visit 'builtins' func to check if program is not found
	
	if (execvp(prog_to_run, token) == -1) {
		//(builtin_checker(token) == 1) ) 
			printf("%s: command not found\n", token[0]);
			_exit(EXIT_FAILURE);
		//}
	}
	else {
		_exit(EXIT_SUCCESS);
	}
}

forker(char* token[99]) {
	pid_t child_pid = 0;

	// returns processs id of forked child
	child_pid = fork();

	// forking begins now, and using the ID we can determine which 

	// execte the child process
	if (child_pid == 0) { 
		child_process(token);
	}	
	
	else if (child_pid > 0) {	
		int status;
		//printf("back in main process\n");
		// here, we could sleep() to allow the child to execute
		// or wait() to wait until child finishes
				
		//waitpid(child_pid, &status, 0);
		wait(NULL);
		//if (status == 0) {
		//	printf("child end success\n");
		//}
		//if (status != 0) {
		//	printf("%s: command not found\n", token[0]);
		//}
	}
	else {
		printf("forking error\n");
		exit(EXIT_FAILURE);
	}
	
}

int builtin_checker(char* token[99]) {
	struct passwd *pw = getpwuid(getuid());

	// check for "CD" and go to home dir
	if (strcmp(token[0], "cd") == 0) {
		if (token[1] == NULL) {
			if (chdir(pw->pw_dir) != 0) {
				printf("%s: %s\n", token[0], strerror(errno));
				return(0);
			}
		}
		else {
			// or attempt to cd(token0) to somedir(token1)
			if (chdir(token[1]) != 0) {
				printf("%s: %s\n", token[1], strerror(errno));
				return(0);
			}
		}
		return(0);
	}
	// or check for "pwd"
	else if (strcmp(token[0], "pwd") == 0) {
		char cwd[1024];
		if (getcwd(cwd, sizeof(cwd)) != NULL) {
			printf("%s\n", cwd);
		}
		return(0);
	}
	else if (strcmp(token[0], "cl") == 0) {
		system("clear");
		return(0);
	}
	else if (strcmp(token[0], "help") == 0) {
		printf("go to help\n");
		return(0);
	}
	else {
		return(1);
	}
}

int arg_checker(char line[1024]) {
	int j, i = 0;
	char user_input[1024];
	char user_input_copy[1024];
	char* delim = " "; // MY DELIMITER 
	char* token[99];
	
	//line[strlen(line) - 1] = '\0';

	strcpy(user_input, line);
	strcpy(user_input_copy, user_input);
	char* rez = strstr(user_input_copy, "|");

	//tokenize the incoming command, with space delim
	
	token[0] = strtok(user_input, delim);

	while (token[i] != NULL) {
		printf("A:%d %s\n", i, token[i]);
		i++;
		//printf("%d\n", i);
		token[i] = strtok(NULL, delim);
	}

	// pipe is found, proceed to secondary parse
	if (rez != NULL) {
		printf("pipe found\n");
		pipe_parser(token, i);	
	
	}

	// now go to builtin checker or forker
	// only if a pipe has not been found
	if (rez == NULL) {
		if (builtin_checker(token) == 1) {
			printf("bc 1\n");
			forker(token);
			//token[0] = '\0';
		}
	}
}

int pipe_parser(char* token[99], int i) {
	printf("in pipe parser. num of tokens: %d\n", i);
	
	char* rez;
	int x; // count of original token set
	char* newtokens[99];
	int pos;
	int len = 0;
	char* liltoken[99];
	char* liltoken2[99];
	
	// also: if first is a pipe = error
	// last is a pipe: expect another command to run at prompt

	
	for (x = 0; x < i; x++) {
		// determine if the string token[x] has pipes	
		rez = strstr(token[x], "|");
		len = strlen(token[x]);

		// has a pipe 	
		if ((rez) && strlen(token[x]) != 1) {
			// how many pipes?
			int cnt = 0;
			const char *tmp = token[x];
			while (tmp = strstr(tmp, "|")) {
				cnt++;
				tmp++;
			}
			printf("P %d: %s - has pipe in pos ", x, token[x]);
			pos = (int)(rez - token[x]);
			printf("%d ", pos);
			printf("num of pipes: %d ", cnt);
			printf("len: %d\n", strlen(token[x]));
			
		
			int c = 0;
			int r = 0;
			/*
			// ie: "|la"
			// next new token to be pipe, following new token is command
			if (pos == 0 && cnt == 1) {
				printf("-!next tokens are: %s %s\n", "|", token[x]+1);
			}

			// ie: "la|"  - pipe in last position: remove it, new token
			else if (cnt == 1 && (((strlen(token[x])) - pos) == 1) ) {
				token[x][len - 1] = '\0';
				printf("next tokens are: %s %s\n", token[x], "|");
			}

			// ie: abc|defg  - pipe in the middle
			else if ((cnt == 1 && strlen(token[x]) > 1)) {
			*/	
				
							
		
			//tokenize the incoming command, with pipe delim
			char myline[1024];
			strcpy(myline, token[x]);	
		
			printf("MYLINE: %s\n", myline);
		
			
			// will have to control |com
			// pos0, pipes1
	
			// control for |com1|com2

			if (pos == 0 && cnt > 1) {
				c = 0;
				printf("Pos 0 and cnt > 1\n");
				liltoken[c] = "|";
				liltoken[c+1] = strtok(myline, "|");
		
				printf("LT0: %s\n", liltoken[c]);
				c = c+1;
				while (token[c] != NULL) {
					c++;
					liltoken[c++] =  "|";		
					liltoken[c++] = strtok(NULL, "|");
				}
				
				for (r = 0; r < c; r++) {
					printf("!:%d %s\n", r, liltoken[r]);
				}
			}

			// all of this seems to be working
		 	// control for com1|com2 and not: com|com2|	
			if (pos > 0)  {
				
				char* subfinaltok[99];

				int d = 0;
				printf("K2\n");
				liltoken2[0] = strtok(myline, "|");
		
				//printf("Ad is: %d\n", d);
		
				
				while (liltoken2[d] != NULL) {
					printf("token %d %s\n", d, liltoken2[d]);
				
					d++;
						
					if (d == cnt) {	
						//printf("Cd is: %d\n", d);
						liltoken2[d] = "|";
						printf("token %d %s\n", d, liltoken2[d]);
						d++;
					}

					liltoken2[d] = strtok(NULL, "|");
					//printf("C3 %d\n", d);
					//d=d+1;
				}
		
						
		
				printf("new array total %d\n", d);		
		
			

							


			}
			
		}
		
					
				/*
				char temptok1[99];
				char* temptok2 = token[x]+pos+1; // defg
				printf("temptok2: %s %d\n", temptok2, strlen(temptok2));
				
				token[x][len-(pos+1)] = '\0'; // abc
				strcpy(temptok1, token[x]);
				//temptok1 = token[x];
				*/
				//printf("temptok1: %s %d\n", temptok1, strlen(temptok1));
				
				//printf("!next tokens are: %s %s %s\n", temptok1, "|", temptok2); 
			//}
		
			// ie: |cmd|
			//else if (cnt == 2) {
			//	token[x][len-1] = '\0';
			//	token[x] = token[x]+1;
			//	printf("next tokens are: %s %s %s\n", "|", token[x], "|");
			//}

		//}
			
		// or is it a pre-parsed pipe?
		else if (strcmp(token[x], "|") == 0) {
			printf("%d: %s - is pipe\n", x, token[x]);
		}
		// has no pipe
		else {
			printf("%d: %s\n", x, token[x]);
		}	
	}	

}



int history(char line[1024]) {
	FILE* f;

	f = fopen("./.history", "a");

	if (f == NULL) {
		return(1);
	}
	else {
		fprintf(f, "%s\n", line);
	}
	fclose(f);
	return(0);
}

int prompt() {
	char cwd[1024];
	char line[1024];
	int ch;
	
	do {
		if (getcwd(cwd, sizeof(cwd)) != NULL) {
			printf("%s> ", cwd);
		}
		else {
			printf("> ");
		}
		// wait for CTRL+D
		if (fgets(line, sizeof(line), stdin) == NULL) {
			printf("\n");
			return(0);
		}

		line[strlen(line) - 1] = '\0';
		printf("L:%s\n", line);
			
		// otherwise proceed to arg_checker		
		if (strlen(line) > 0) {
			history(line);
			arg_checker(line);
			//line[0] = '\0';
		} 
	} while(1);
}

int main(int argc, char* argv[]) {
	//system("clear");
	printf("welcome to seeshell\n"); 
	while (1) {
		if (prompt() == 0) {
			exit(EXIT_SUCCESS);
		}
		else {
			exit(EXIT_FAILURE);
		}
	}
}
