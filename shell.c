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

int check_for_builtins(char* token[99]) {
	int x;
	char* list[4] = {"?", "help", "yo"};

	//printf("bb\n");
	//while (x < sizeof(list)/sizeof(list[0])) {
	for (x = 0; x < sizeof(list) / sizeof(list[0]); x++) {
		if (strcmp(token[0], list[x]) == 0) {
			printf("found\n");
			return(0);
		
		//x++;
		}
	}
	return(1);
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
		//if (check_for_builtins(token) != 0) {
			printf("%s: command not found\n", token[0]);
			_exit(EXIT_FAILURE);
		}
	//}
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

// change directory built-in
cd_builtin(char* token[99]) {
	struct passwd *pw = getpwuid(getuid());

	// check for just "CD" and go to user's home dir
	if (token[1] == NULL) {
		if (chdir(pw->pw_dir) != 0) {
			printf("%s: %s\n", token[1], strerror(errno));
		}
	}
	// or attempt to cd(token0) to somedir(token1)
	else if (chdir(token[1]) != 0) {
		printf("%s: %s\n", token[1], strerror(errno));
	}
	else {
		prompt();
	}
}

int arg_checker(char line[1024]) {
	
	//tokenize the incoming command
	int j, i = 0;
	char user_input[1024];
	char* delim = " "; // MY DELIMITER 
	char* token[99];

	//printf("arg checker\n");
	char user_input_copy[1024];
	strcpy(user_input_copy, line);
	
	char* rez = strstr(user_input_copy, "|");
	
	if (rez != NULL) {
		printf("pipe found\n");
	}


	// space delim checker
	
	strcpy(user_input, line);
	
	token[0] = strtok(user_input, delim);

	while (token[i] != NULL) {
		printf("%d %s\n", i, token[i]);
		i++;
		//printf("%d\n", i);
		token[i] = strtok(NULL, delim);
	}

	// check for "cd" otherwise go to forker
	if (strcmp(token[0], "cd") == 0) {
		cd_builtin(token);
	}
	// or check for "pwd"
	else if (strcmp(token[0], "pwd") == 0) {
		char cwd[1024];
		if (getcwd(cwd, sizeof(cwd)) != NULL) {
			printf("%s\n", cwd);
		}
	}
	else {
		forker(token);
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
	
		// otherwise proceed to arg_checker		
		if (strlen(line) > 0) {
			history(line);
			arg_checker(line);
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

