#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

volatile int running = 1;
pid_t *pidsAux;
int nFiles;


void sigHandler(int signo) {
	
	//kill process
	running = 1;
	 int i,j;
  for (i=0;i<nFiles;i++)
  {
    for (j = 0; j < 3; j++)
    {
		//printf("%d\n", pidsAux[i]);
      kill(-pidsAux[i],SIGINT);
    }
  }
 printf("killing...\n");
	exit(0);
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


int main(int argc, char *argv[]) {
	
	int timer = atoi(argv[1]);
	nFiles = argc-3;
	//char* files[nFiles];
	char* word = argv[2];
	pid_t auxiliar[nFiles];
	//pidsAux = auxiliar;
	int i;
	for (i = 0; i < nFiles; i++) {
		auxiliar[i] = 0;
	}
	
	if (argc < 4) {
		printf("Number of arguments wrong");
		exit(-1);
	}
	
	pidsAux= (int*) malloc(nFiles*sizeof(int));
	for (i = 0; i < nFiles; i++)
	{
		auxiliar[i] = fork();
		pidsAux[i]=auxiliar[i];
		if (auxiliar[i] < 0) {
		printf("Error while forking");
		}
		else if (auxiliar[i] > 0){
			//parent
		}
		else {
			//son
			execlp(getPathMonitorAux(), "monitorAux",word,argv[i+3], NULL);
		}
	}
	
	struct sigaction sact;
	sigemptyset(&sact.sa_mask);
	sact.sa_flags = 0;
	sact.sa_handler = sigHandler;
	sigaction(SIGALRM, &sact, NULL);
	alarm(timer);
	
	while(running);

	return 0;	
}
