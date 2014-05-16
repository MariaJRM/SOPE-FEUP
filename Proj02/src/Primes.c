/*
	SOPE Project-02

	Authors:
		- Anaís Dias: ei12015@fe.up.pt
		- Maria João Marques: ei12104@fe.up.pt

	2014 FEUP
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "CircularQueue.h"

#define QUEUE_SIZE 10

int writeIndex, primesListSize;
long n;
unsigned long* primesList; 
sem_t termination;
pthread_mutex_t primesListControlMutex; 

//compare function that is going to be used on sort primes list function
int cmpfunc(const void * a, const void * b) {
	return (*(unsigned long*)a - *(unsigned long*)b);
}

//function to display prime numbers
void showPrimesList() {
	printf("Primes:\n");
	int i;
	for (i = 0; i < primesListSize; i++)
		printf("%ld ", primesList[i]);
	printf("\n");
}

//function to add a prime number to the prime numbers list
void addPrimeToList(unsigned long n) {
	pthread_mutex_lock(&primesListControlMutex);

	primesList[writeIndex] = n;
	writeIndex++;
	primesListSize++;

	pthread_mutex_unlock(&primesListControlMutex);
}

void* filterThread(void* arg) {
	CircularQueue* input = arg;

	unsigned long num = queue_get(input);

	//the rest of the numbers are prime numbers and are added to the prime list (Sieve of Eratosthenes)
	if (num > sqrt(n)) {
		while(num != 0) {
			addPrimeToList(num);
			num = queue_get(input);
		}

		sem_post(&termination);
	} else {
		//saves on a temporary variable the number on the head of the input queue
		unsigned long temp = num;

		//creates an output queue
		CircularQueue* output;
		queue_init(&output, QUEUE_SIZE);

		//creates the other filter thread of the sequence
		pthread_t filtThread;
		pthread_create(&filtThread, 0, filterThread, output);

		//checks if the rest of the number aren't multiples of the temporary variable
		do {
			num = queue_get(input);

			if(num % temp != 0)
				queue_put(output, num);
		} while (num != 0);

		//adds to the output queue the value 0 (zero)
		queue_put(output, 0);
		addPrimeToList(temp);
	}

	//destroys the input circular queue
	queue_destroy(input);

	return NULL;
}

void* initialThread(void* arg){
	//adds the number 2 to the primes list
	addPrimeToList(2);

	//if the argument is only 2 the termination semaphore is activated
	if (n == 2)
		sem_post(&termination);
	else {
		//creates an output queue
		CircularQueue* output;
		queue_init(&output, QUEUE_SIZE);

		//creates the filter thread
		pthread_t filtThread;
		pthread_create(&filtThread, 0, filterThread, output);

		//only the odd numbers are added to the output queue
		int i;
		for(i = 3; i <= n; i+=2)
			queue_put(output, i);

		//adds to the output queue the value 0 (zero)
		queue_put(output, 0);
	}

	return NULL;
}

int main(int argc, char** argv) {
	//prints an error message if the number of arguments is wrong
	if (argc != 2) {
		printf("Number of arguments wrong.\n");
		printf("It should be: primes <n>\n");
	}


	//converting argument (string) to long int
	char* pEnd;
	n = strtol(argv[1], &pEnd, 10);
	if (pEnd == argv[1] || n < 2) {
		printf("Argument invalid.\n");
		return -1;
	}

	//initializing variables
	writeIndex = 0;
	primesListSize = 0;
	primesList = (unsigned long*) malloc (sizeof(unsigned long) * 1.2 * n / log(n));
	if (sem_init(&termination, 0, 0) != 0)
		return -1;
	if (pthread_mutex_init(&primesListControlMutex, NULL) != 0)
		return -1; 
	
	//creates the initial thread
	pthread_t iniThread;
	pthread_create(&iniThread, 0, initialThread, 0);

	//waiting for termination semaphore
	sem_wait(&termination);

	//sort primes list
	qsort(primesList, primesListSize, sizeof(unsigned long), cmpfunc);

	showPrimesList();

	return 0;
}