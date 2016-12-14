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

const char* HISTFILE = "./shell/.history";


child_process(char* token[99]) {
	//printf("Now in the child process\n");

	char bindir[1048] = "./shell/bin/";
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

int builtin_checker(char* token[99], int argc) {
	struct passwd *pw;
	char thedir[99];

	if (strcmp(token[0], "cd") == 0) {
		// cd and cd ~
		if ((argc == 1) || (argc == 2) && (strcmp(token[1], "~") == 0)) {
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
				return(0);
			}
					
			if (chdir(thedir) != 0) {
				printf("%s: %s\n", token[0], strerror(errno));
				return(0);
			}
			return(0);  // hmmm
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
				return(0);
			}	
		}
		// cd /somedir
		else if (argc == 2) {
			if (chdir(token[1]) != 0) {
				printf("%s: %s\n", token[0], strerror(errno));
				return(0);
			}
		}
		// or attempt to cd(token0) to somedir(token1)
		else if ((argc == 2) && (strncmp(token[1], "~", 1) != 0)) {
			if (chdir(token[1]) != 0) {
				printf("%s: %s\n", token[1], strerror(errno));
				return(0);
			}
		}
	}
	// or check for "pwd"
	else if (strcmp(token[0], "pwd") == 0) {
		char cwd[1024];
		if (getcwd(cwd, sizeof(cwd)) != NULL) {
			printf("%s\n", cwd);
		}
		return(0);
	}
	else if ((strcmp(token[0], "cl") == 0) || (strcmp(token[0], "clear") == 0)) {
		system("clear");
		return(0);
	}
	else if (strcmp(token[0], "history") == 0) { 
		if (argc == 1) {
			history(NULL, 1, 0); // show history
		}
		if (argc > 1) {	
			if (strcmp(token[1], "-c") == 0) {
				history(NULL, 1, 1); // or delete history
			}
			else {
				printf("history syntax err\n");
			}
		}
		return(0);
	}
	else if (strcmp(token[0], "help") == 0) {
		printf("go to help\n");
		return(0);
	}
	// no builtin found, head to forker and search for coreutil
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
	
	//printf("!!%c\n", line[0]);
	
	strcpy(user_input, line);
	strcpy(user_input_copy, user_input);
	char* rez = strstr(user_input_copy, "|");

	// error if line starts with a pipe
	if (strncmp(line, "|", 1) == 0) {	
		printf("syntax error: unexpected token '|'\n");
		return(1);
	}

	//tokenize the incoming command, with space delim
	
	token[0] = strtok(user_input, delim);

	//printf("first pass, tokenize spaces\n");

	while (token[i] != NULL) {
		printf("tokens: %d: %s\n", i, token[i]);
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
		if (builtin_checker(token, i) == 1) {
			forker(token);
		}
	}
}

int pipe_parser(char* token[99], int i) {
	printf("in pipe parser. num of tokens: %d\n", i);
	
	char* rez;
	int x; 
	char* newtokens[99];
	int pos;
	int len = 0;
	char* liltoken[99];
	char* liltoken2[99];

	char* final_tokens[99];
	int ft = 0;
	//int a = 0;

	// also do: last is a pipe: expect another command to run at prompt
	
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
			//printf("P %d: %s - has pipe in pos ", x, token[x]);
			pos = (int)(rez - token[x]);
			//printf("%d ", pos);
			//printf("num of pipes: %d ", cnt);
			//printf("len: %d\n", strlen(token[x]));
		
			int c = 0;
			int r = 0;

			// ie: "|la"
			// next new token to be pipe, following new token is command
			if (pos == 0 && cnt == 1) {
				//printf("next tokens are: %s %s\n", "|", token[x]+1);
				final_tokens[ft] = "|";
				ft++;
				final_tokens[ft] = token[x]+1;
				ft = ft + 1;
			}
	
			//tokenize the incoming command, with pipe delim
			char myline[1024];
			strcpy(myline, token[x]);	

			// control for |com1|com2

			if (pos == 0 && cnt > 1) {
				c = 0;
				printf("Pos 0 and cnt > 1\n");
				liltoken[c] = "|";
				liltoken[c+1] = strtok(myline, "|");
		
				//printf("LT0: %s\n", liltoken[c]);
				c = c+1;
				while (token[c] != NULL) {
					c++;
					liltoken[c++] =  "|";		
					liltoken[c++] = strtok(NULL, "|");
				}
				
				for (r = 0; r < c; r++) {
					//printf("liltoken:%d %s\n", r, liltoken[r]);
					final_tokens[ft] = liltoken[r];
					ft++;
				}
			}

			// THIS ONE MAY BE CAUSING PROBLEMS
		 	// control for com1|com2 and not: com|com2|	
			if (pos > 0)  {
				//char* subfinaltok[99];

				int d = 0;
				int a = 0;
				
				printf("K2\n");
				liltoken2[0] = strtok(myline, "|");
		
				printf("ft starting at: %d. cnt: %d\n", ft, cnt);	

				while (liltoken2[a] != NULL) {
					//printf("Tx a:%d ft:%d %s\n", a, ft, liltoken2[a]);
					//final_tokens[ft] = liltoken2[a];
					
					final_tokens[ft] = malloc(strlen(liltoken2[a]) + 1);
					final_tokens[ft] = strdup(liltoken2[a]);	
					//printf("az %d %s\n", ft, final_tokens[ft]);
					//d++;  
					ft++;
					a++;	
					if (d != cnt) {	
						//printf("d is: %d\n", d);
						liltoken2[a] = "|";
					        //printf("Tp a:%d ft:%d %s\n", a, ft, liltoken2[a]);
						final_tokens[ft] = malloc(strlen(liltoken2[a]) + 1);
						final_tokens[ft] = strdup(liltoken2[a]);
						
						//printf("azz %d %s\n", ft, final_tokens[ft]);
						ft++;
						d++;
						a++;
					}

					liltoken2[a] = strtok(NULL, "|");
					
					//printf("A %d\n", a);
					//d=d+1;
				}
				//printf("new array total %d\n", a);		
				//ft = ft + a;
				//ft++;
				//ft = ft+1;	
			}
			printf("ft ending at: %d\n", ft);
		

		}
				
		// or is it a pre-parsed pipe?
		else if (strcmp(token[x], "|") == 0) {
			printf("%d: %s - is pipe\n", x, token[x]);
			final_tokens[ft] = "|";
			ft++;
		}
		// has no pipe
		else {
			printf("NP %d: %s\n", x, token[x]);
			final_tokens[ft] = token[x];
			ft++;
		}	
	}	

	printf("grand unified final token count: %d\n", ft);

	for (x = 0; x < ft; x++) {
		printf("%d: %s\n", x, final_tokens[x]);
	}


}

// manage history file

int history(char line[1024], int builtin, int delete) {
	FILE* f;
	int c;
	int x = 0;
	int nrecs = 0;
	char theline[500];	
	
	int maxhistory = 2000;

	// if we're not calling the history builtin, count records and add to file
	if (builtin != 1) {
		f = fopen(HISTFILE, "r+");

		if (f == NULL) {
			printf("history file error: %s\n", strerror(errno));
			return(1);
		}

		// first count lines to give us array size for filelines[nrecs]
		while ((c = fgetc(f)) != EOF) {
			if (c == '\n') {
				++nrecs;
			}
		}
		//printf("%d records on file\n", nrecs);
		rewind(f);	
	
		char* filelines[nrecs+1];
	
		// now read all of the lines

		while (fgets(theline, sizeof theline, f) != NULL) {
			filelines[x] = malloc(strlen(theline) + 1);
			theline[strlen(theline) - 1] = '\0';
			filelines[x] = strdup(theline);
			x++;
		}
		fclose(f);
	
		// if history file larger than MAXHISTORY, shrink the file

		if (nrecs > maxhistory) {
			int a = nrecs - maxhistory;
			int newcnt = 0;	
			char* newlines[nrecs - a];
	
			// get the lines to include in .history replacement

			for (a = (nrecs - maxhistory); a < nrecs; a++) {
				newlines[newcnt] = strdup(filelines[a]);
				newcnt++;
			}
	
			// truncate and replace with new lines
			f = fopen(HISTFILE, "w");
		
			if (f == NULL) {
				printf("history file error: %s\n", strerror(errno));
				return(1);
			}

			for (x = 0; x < newcnt; x++) {
				fprintf(f, "%s\n", newlines[x]);
			}		
			fclose(f);
		}
	
		// finally append latest line to the file
		f = fopen(HISTFILE, "a");
	
		if (f == NULL) {
			printf("history file error: %s\n", strerror(errno));
			return(1);
		}
		else {
			fprintf(f, "%s\n", line);
		}
		fclose(f);
		return(0);
	}
	// otherwise, display or delete history if calling builtin
	if (builtin == 1) { 
		// delete history by calling history -c or history(NULL, 1, 1)
		if (delete == 1) {
			char q[10];	
			int fd = open(HISTFILE, O_WRONLY); //truncate

			if (fd == -1) {
				printf("history file error: %s\n", strerror(errno));
				return(1);
			}
		
			while(1) {
               			printf("delete history? y/n\n");
               			fgets(q, sizeof (q), stdin);

				if (q[0] == 'y') {
                        		ftruncate(fd, 0);
					fsync(fd);
					printf("done\n");
					break;
				}
				else if (q[0] == 'n') {
					break;
				}	
				else {
                        		continue;
				}
			}
			close(fd);
		}
		// otherwise print history
		else { 
			x = 0;
			f = fopen(HISTFILE, "r");

			if (f == NULL) {
				printf("history file error: %s\n", strerror(errno));
				return(1);
			}
			while (fgets(theline, sizeof theline, f) != NULL) {
				theline[strlen(theline) - 1] = '\0';
				printf("\t%d: %s\n", x, theline);
				x++;
			}
			fclose(f);
		}
		return(0);
	}
}

// some routine tests before starting
int program_init() {
	
	// start with the history file
	FILE* fh;

	fh = fopen(HISTFILE, "a+");
	
	if (fh == NULL) {
		printf("history file error: %s\n", strerror(errno));
		return(1);
	}		
	
	fclose(fh);
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
				
		// otherwise proceed to history writer and arg_checker		
		if (strlen(line) > 0) {
			if (history(line, 0, 0) != 0) {
				return(1);
			}
			arg_checker(line);
		} 
	} while(1);
}

int main(int argc, char* argv[]) {
	
	if (program_init() == 1) {
		exit(EXIT_FAILURE);
	}
	
	//system("clear");
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
