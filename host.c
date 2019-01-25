    #include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <fcntl.h>
#include <syslog.h>
#include <time.h>
#include <math.h>
#include <errno.h>

#include "connector.h"

#define BUF_SIZE 100
#define CLIENT_NUM 4
#define ANSWER_NUM 6

#define END_PROGRAM "quit\n"

static char* answers[ANSWER_NUM] = {
    "Yes",
    "No",
    "Maybe",
    "Definitely",
    "obligatory",
    "Controversial"
    };

int GetNumAnswer(int seed)
{
	return (rand() & seed) % ANSWER_NUM;
}

void GetClientsAnswer(int i, int seed, sem_t* sem, char* nameSem, sem_t* semParent)
{
	int ind_answer, status;
	while(1)
	{
		sem_wait(sem); //wait for host
		status = conn_read(i);
		if (status == 0)
			break;
		ind_answer =  GetNumAnswer(seed);
		conn_write(i, ind_answer);
		sem_post(semParent);
	}
	exit(0);
}

void GetHostsQuestion(sem_t* semParent, sem_t* sem[], char* nameSem[])
{
	char string[BUF_SIZE], *answer;
	int i;
    
	while(1)
	{
		printf("Enter your question: \n");
        fgets(string, BUF_SIZE, stdin);  
		if (!(strcmp(string,END_PROGRAM)) ) // End work
  		{   
  			for (i = 0; i < CLIENT_NUM; i++)
  			{
  				conn_write(i, 0); // stop
  				sem_post(sem[i]); // send signal to end work
  			}
  			printf("Goodbye!\n");
  			break;
  		}
        
		for (i = 0; i < CLIENT_NUM; i++)
		{ 
            conn_write(i, 1); // status for proceed work
            sem_post(sem[i]); // send signal to child
            sem_wait(semParent); // wait for child
 		}
        
  		for (i = 0; i < CLIENT_NUM; i++)
  		{ 
  			answer = answers[conn_read(i)];
  			printf("%s\n", answer );
  		}
	}
}


int main(int argc, char *argv[])
{
	srand(time(0));
	pid_t pid;
    sem_t* sem[CLIENT_NUM], *par_sem;
  	int i;
  	char* sem_names[CLIENT_NUM] = {"/semaphore_1","/semaphore_2","/semaphore_3","/semaphore_4"};

  	int rand_seed[CLIENT_NUM];
    
  	for (i = 0; i < CLIENT_NUM; i++)
  	{
  		rand_seed[i] = rand();
  	}
	
  	conn_create();

  	par_sem = sem_open("/par_sem", O_CREAT, S_IRWXU, 0); 
  	if(par_sem == SEM_FAILED)
	{
		printf("E: With sem_open\n");
	}

	for (i = 0; i < CLIENT_NUM; i++) // create semaphores
	{
		sem[i] = sem_open(sem_names[i], O_CREAT, S_IRWXU, 0);
	  	if(sem[i] == SEM_FAILED)
	  	{
	  		printf("E: With sem_open %i\n", i);
	  	}
	}
	// create client processes
	for(i = 0; i < CLIENT_NUM; i++) {
		pid = fork();
	  	if(pid){
	  		continue;
	  	}
	  	else{
	  		GetClientsAnswer(i, rand_seed[i], sem[i], sem_names[i], par_sem);
	  		exit(0);
	  	}
	}
	GetHostsQuestion(par_sem, sem, sem_names);

	conn_delete();
	sem_close(par_sem);
	printf("close\n");
	sem_unlink("/par_sem");
	for (i = 0; i < CLIENT_NUM; i++)
	{
	 	sem_close(sem[i]);
	 	sem_unlink(sem_names[i]);
	}

	return 0;
}

