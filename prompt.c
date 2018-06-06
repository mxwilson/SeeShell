#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<readline/readline.h>
#include<readline/history.h>
#include"shheader.h"

// compiled like this
//gcc -Wall readprompt.c -o readprompt -lreadline

int prompt(void) {
        //if (argc > 1) {
        //        rl_bind_key('\t', rl_insert);
        //}
	char cwd[300];
        char* buf;
	char shell_prompt[100];
	char line[1024];

	do {
		getcwd(cwd, sizeof(cwd));
		snprintf(shell_prompt, sizeof(shell_prompt), "%s> ", cwd);

		buf = readline(shell_prompt); 

		if (!buf) { // wait for CTRL+D
			printf("\n");
			return(0);
		}
		if (strlen(buf) < 1) { // empty line
			continue;
		}
     		if (strlen(buf) > 0) { // otherwise add to internal history buffer 
                	add_history(buf);
                }
		
		strcpy(line, buf);
		// add to our history file
		if (history(line, 0, 0, 0) != 0) {
			return(1);
		}
		arg_checker(line);
		free(buf);
        } while (1);

return 0;
}
