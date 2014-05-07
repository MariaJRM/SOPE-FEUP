#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <wait.h>

pid_t *pidsAux;
pid_t pidFileMonitor;
int nFiles;
int zeroFiles;

void processKiller(){
unsigned i,j;
	for(i = 0; i <nFiles; i++)
	{
		for(j= 0; j<3; j++) {
			kill(-pidsAux[i], SIGUSR1);
		}
	}
	kill(pidFileMonitor, SIGUSR1);
	printf("killing...\n");
	exit(0);
}

void handler(int sig) {
	if(sig==SIGALRM) {
		processKiller();
	}
	if(sig == SIGCHLD) {
		int status;
		wait(&status);
	}
	if (sig == SIGUSR1)		
		exit(0);
	if (sig == SIGUSR2)
		zeroFiles = 1;
}
char* getCWDPath() {
	char* cwd = (char*) malloc(1024*sizeof(char));

	getcwd(cwd, 1024);

	return cwd;
}

char* getPathMonitorAux()  {
	char* cwd = getCWDPath();

	char* monitorAuxPath;
	if((monitorAuxPath = malloc(strlen(cwd)+1)) != NULL) {
		monitorAuxPath[0] = '\0';	// ensures the memory is an empty string
		strcat(monitorAuxPath, cwd);
		strcat(monitorAuxPath, "/monitorAux");
	} 

	return monitorAuxPath;
}

void file_monitor(char* files[])
{ 
	int monFiles = nFiles; 
	while(monFiles > 0) { 
		int i; 
		for(i = 0; i < nFiles; i ++) { 
			if(pidsAux[i] == 0) continue; 
			if(fopen(files[i], "r") == NULL) { 
				printf("%s was removed \n",files[i]); 
				printf("killing file's auxiliar monitor...\n"); 
				kill(-pidsAux[i],SIGUSR1); 
				pidsAux[i] = 0; 
				monFiles--; 
				}
			}
			if(monFiles == 0) kill(getppid(),SIGUSR2); 
			sleep(5);
		} 
}


int main(int argc, char *argv[]) {
	
	int timer = atoi(argv[1]);
	nFiles = argc-3;
	char* files[nFiles];
	char* word = argv[2];
	pid_t auxiliar[nFiles];
	int i;
	for (i = 0; i < nFiles; i++) {
		auxiliar[i] = 0;
	}
	
	if (argc < 4) {
		printf("Wrong number of arguments.");
		exit(-1);
	}
	
	for (i=0; i < nFiles; i++)
	{
		files[i] = argv[i+3];
	}
	
	for (i=0; i<nFiles;i++)
	{
		if(fopen(files[i], "r") == NULL)
		{
			printf("Non existant file.\n");
			exit(0);
		}
	}
	
	pidsAux= (int*) malloc(nFiles*sizeof(int));
	
	for (i = 0; i < nFiles; i++)
	{
		auxiliar[i] = fork();
		pidsAux[i]=auxiliar[i];
		if (auxiliar[i] < 0) {
		printf("Error while forking.");
		}
		else if (auxiliar[i] > 0){
			//parent
		}
		else {
			//son
			execlp(getPathMonitorAux(), "monitorAux",word,argv[i+3], NULL);
		}
	}
	
	pidFileMonitor = fork();

	if(pidFileMonitor < 0) {
		fprintf(stderr, "Error while forking.\n");
		exit(1);
	}
	//child process
	else if(pidFileMonitor > 0) {
		//parent process
	}

	else {
		//child process
		char* files[nFiles];

		for(i = 0;i<nFiles;i++)
			files[i] = argv[i+3];

		file_monitor(files);
	}

	struct sigaction action;
	action.sa_handler = handler;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;
	sigaction(SIGUSR1,&action,NULL);
	sigaction(SIGUSR2,&action,NULL);
	sigaction(SIGCHLD,&action,NULL);
	sigaction(SIGALRM,&action,NULL);
	alarm(timer);
	
	while(timer > 0 && zeroFiles == 0)
		timer = sleep(timer);

	zeroFiles = 1;

	if(zeroFiles == 1)
		processKiller();

	return 0;	
}
