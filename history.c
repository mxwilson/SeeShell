/*
history.c - part of seeshell - bash-like unix shell replacement
 
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
No warranty. Software provided as is.
Copyright Matthew Wilson, 20116.
*/

#include<stdio.h>
#include<errno.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
#include"shheader.h" 

/* when calling this routine without a line do the following:
history(NULL, 1, 0); // show history
history(NULL, 1, 1); // or delete history */

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
	//if (builtin == 1) { 
	else {
		// delete history by calling history -c or history(NULL, 1, 1)
		if (delete == 1) {
			char q[5];	
			int fd = open(HISTFILE, O_WRONLY); //truncate

			if (fd == -1) {
				printf("history file error: %s\n", strerror(errno));
				return(1);
			}
		
			while(1) {
               			printf("delete history? y/n\n");
               			fgets(q, sizeof (q), stdin);

				if ((q[0] == 'y') || (strcmp(q, "yes") == 0)) {
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
	}
	return(0);
}
