#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define READ 0
#define WRITE 1

#define MAXLINE 256

char* timeStr()
{
	time_t cTime;
	
	if ((cTime = time(NULL)) == (time_t) - 1)
	printf("Error return time.\n");
	
	int year = localtime(&cTime)->tm_year+1900;
	int month = localtime(&cTime)->tm_mon+1;
	int day = localtime(&cTime)->tm_mday;
	int hour = localtime(&cTime)->tm_hour;
	int minute = localtime(&cTime)->tm_min;
	int second = localtime(&cTime)->tm_sec;
		
	char str[20];
	sprintf(str, "%02d-%02d-%02dT%02d:%02d:%02d", year, month, day, hour, minute, second);
		
	char* strTime;
	strTime = malloc(strlen(str + 1));
	strcpy(strTime, str);
	
	return strTime;
}


int main(int argc, char *argv[])
{
	
	if (argc != 3) {
		printf("Number of arguments wrong.\n");
		printf("It should be <monitorAux> <word> <file>\n");
	}
	
	int p1[2], p2[2];
	pid_t pidTail, pidGrep;
	char* word = argv[1];
	char* filename = argv[2];
	
	
	//pipes
	if(pipe(p1) < 0 || pipe(p2) < 0) {
		printf("Error creating pipes\n");
		exit(1);
	}
	setpgrp();
	//starts forking (tail- son)
	if ((pidTail = fork()) < 0) {
		printf("Error while forking (Tail)\n");
		exit(2);
	} else if (pidTail > 0) {
		//forking (grep- son)
		if ((pidGrep = fork()) < 0) {
			printf("Error while forking (Grep)\n");
			exit(3);
		}
		else if (pidGrep > 0) {
			//parent
			close(p2[WRITE]);
			
			int n;
			char line[MAXLINE];
			while (1) {
				n = read(p2[READ], line, MAXLINE);
				line[n-1] = 0;
				printf("%s - %s - ",timeStr(),filename);
				int j;
				for(j=0;j<MAXLINE;j++)
					if(line[j]=='\n')
						line[j]='\0';
				printf("\"%s\"\n", line);
			}
		} else {
			//son - grep
			close(p1[WRITE]);
			dup2(p1[READ], STDIN_FILENO);
		
			dup2(p2[WRITE], STDOUT_FILENO);
			close(p2[READ]);
			
			execlp("grep", "grep", "--line-buffered", word, NULL);
		}
	} else {
		//son - tail
		dup2(p1[WRITE], STDOUT_FILENO);
		close(p1[READ]);
		
		execlp("tail", "tail", "-f", "-n", "0", filename, NULL);
	}

	return 0;
}
