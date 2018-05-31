/*
history.c - part of seeshell - bash-like unix shell replacement
 
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
No warranty. Software provided as is.
Copyright Matthew Wilson, 2016-2018.
*/

#include<stdio.h>
#include<errno.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
#include"shheader.h" 

/* when calling this routine without a line do the following:
history(NULL, 1, 0, 0); // show history
history(NULL, 1, 1, 0); // or delete history       
history(NULL, 1, 2, mynum); // or show n history items */                                                  
int history(char line[1024], int builtin, int histoption, int recstoprint) {
	FILE* f;
	int c;
	int x = 0;
	int nrecs = 0;
	char theline[500];	
	char* filelines[2000];
	int maxhistory = 2000;

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
	
	rewind(f);	
	
	// now read all of the lines
	while (fgets(theline, sizeof theline, f) != NULL) {
		filelines[x] = malloc(strlen(theline) + 1);
		theline[strlen(theline) - 1] = '\0';
		filelines[x] = strdup(theline);
		x++;
	}
	fclose(f);

	// if we're not calling the history builtin, count records and add to file
	if (builtin != 1) {
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
	else {
		if (histoption == 1) { // delete history
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
					printf("Done :)\n");
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
		else { 	// otherwise print history
			x = 0;
			f = fopen(HISTFILE, "r");

			if (f == NULL) {
				printf("history file error: %s\n", strerror(errno));
				return(1);
			}
			// here show n lines
			if (histoption == 2) {
				//printf("%d records\n", nrecs);
				//printf("%d records to print\n", recstoprint);
			
				/* this works
				for (x = 0; x < nrecs; x++) {
					printf("! %d %s\n", x+1, filelines[x]);	
				}*/
				for (x = nrecs - recstoprint; x < nrecs; x++) {
					printf("\t%d: %s\n", x + 1, filelines[x]);	
				}

			}	
			// otherwise print all lines
			else {
				while (fgets(theline, sizeof theline, f) != NULL) {
					theline[strlen(theline) - 1] = '\0';
					printf("\t%d: %s\n", x + 1, theline);
					x++;
				}
			}
			fclose(f);
		}	
	}
return(0);
}
