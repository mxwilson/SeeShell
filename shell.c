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


int check_for_builtins(char line[1024]) {
	int x;
	char* list[4] = {"?", "help", "yo"};

	// if we hit one of the above, say ? then run an ls on builtin commands?
	while (x < sizeof(list)/sizeof(list[0])) {
		if (strcmp(line, list[x]) == 0) {
			printf("found\n");
			return(0);
		}		
		x++;
	}
	return(1);
}


// forking

child_process(char line[1024]) {
	//printf("Now in the child process\n");

	char bindir[1048] = "./bin/";
	char prog_to_run[1048]; 
	strcat(bindir, line);
	strcpy(prog_to_run, bindir);
	
	printf("prog to run: %s\n", prog_to_run);
	// run the concatenated program ie ./bin/progname. 
	// visit 'builtins' func to check if program is not found

	if (execl(prog_to_run, line, "-la", NULL) == -1) {

	//if (execvp(prog_to_run, "-l") == -1) {
		if (check_for_builtins(line) == 1) 
			_exit(EXIT_FAILURE);
	}
	 
	_exit(EXIT_SUCCESS);
	
}

forker(char line[1024]) {
	pid_t child_pid = 0;

	// returns processs id of forked child
	child_pid = fork();

	// forking begins now, and using the ID we can determine which 

	// execte the child process
	if (child_pid == 0) { 
		child_process(line);
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

int prompt() {
	char cwd[1024];
	char line[1024];

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
	
	forker(line);
	
}


int main(int argc, char* argv[]) {
	system("clear");
	printf("welcome to seeshell\n");
	while (1) {
		prompt();
	}
}

