#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#define READ 0
#define WRITE 1

int main(int argc, char *argv[])
{
	int p1[2], p2[2];
	int pidTail, pidGrep;
	char filename[] = argv[1];
	char word[] = agrv[2];
	
	//pipes
	if(pipe(p1) < 0 || pipe(p2) < 0) {
		printf("Error creating pipes");
		exit(1);
	}
	
	//forking
	if((pidTail == fork()) < 0)
		printf("Error while forking (Tail)");
		exit(2);
	}
	
	if((pidGrep == fork()) < 0) {
		printf("Error while forking (Grep)");
		exit(3);
		} 
		
	if(pidTail == 0) {
		//tail (son)
		dup2(p1[WRITE], STDOUT_FILENO);
		close(p1[READ]);
		execlp("tail", "tail", "-f", "-n", "0", filename, NULL);
	}
	
	if(pidGrep == 0) {
		//grep
		close(p1[WRITE]);
		dup2(p1[READ], STDIN_FILENO);
		
		close(p2[READ]);
		dup2(p2[WRITE], STDOUT_FILENO);
		execlp("grep", "grep", "--line-buffered", word, NULL);
	}
	
	if(pidGrep > 0) {
		close(p1[WRITE]);
	}
	
		
	return 0;
}
