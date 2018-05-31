/*
prompt.c - part of seeshell - bash-like unix shell replacement
 
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
No warranty. Software provided as is.
Copyright Matthew Wilson, 2016-2018.
*/

#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include"shheader.h"

int prompt(void) {
        char cwd[1024];
        char line[1024];
        
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
                        if (history(line, 0, 0, 0) != 0) {
                              return(1);
                        }
                        arg_checker(line);
                } 
        } while(1);
}
