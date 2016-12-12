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


int check_for_builtins(char* token[99]) {
	int x;
	char* list[4] = {"?", "help", "yo"};

	while (x < sizeof(list)/sizeof(list[0])) {
		if (strcmp(token[0], list[x]) == 0) {
			printf("found\n");
			return(0);
		}
		x++;
	}
}


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
		if (check_for_builtins(token) != 0) 
			_exit(EXIT_FAILURE);
	}
	_exit(EXIT_SUCCESS);
	
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
				
		waitpid(child_pid, &status, 0);
		
		if (status == 0) {
			printf("child end success\n");
		}
		else {
			printf("child end failure\n");
		}
	}
	else {
		printf("forking error\n");
		exit(EXIT_FAILURE);
	}
	
}


// change directory built-in
cd_builtin(char* token[99]) {
	if (chdir(token[1]) != 0) {
		printf("no dir: %s\n", token[1]);
	}
	else {
		prompt();
	}
}

int arg_checker(char line[1024]) {
	//tokenize the incoming command
	int j, i = 0;
	char user_input[1024];
	char* delim = " ";
	char* token[99];

	//printf("arg checker\n");
	
	strcpy(user_input, line);
	
	token[0] = strtok(user_input, delim);

	while (token[i] != NULL) {
		i++;
		token[i] = strtok(NULL, delim);
	}
	
 	// check for "cd" otherwise go to forker
	if (strcmp(token[0], "cd") == 0) {
		cd_builtin(token);
	}
	else {
		forker(token);
	}
}

int prompt() {
	char cwd[1024];
	char line[1024];

		// if we hit one of the above, say ? then run an ls on builtin commands?
do {
		if (getcwd(cwd, sizeof(cwd)) != NULL) {
			printf("%s> ", cwd);
		}
		else {
			printf("> ");
		}
	
		fgets(line, sizeof(line), stdin);
		line[strlen(line) - 1] = '\0';
	
		if (strcmp(line, "exit") == 0) {
			printf("exiting\n");
			exit(EXIT_SUCCESS);
		}
	} while ((strlen(line) < 1));
	// proceed to arg_checker before forking
	arg_checker(line);
}


int main(int argc, char* argv[]) {
	system("clear");
	printf("welcome to seeshell\n"); 
	while (1) {
		prompt();
	}
}

