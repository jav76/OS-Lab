#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>

sem_t agentSem, tobacco, paper, match;
sem_t tobaccoSem, paperSem, matchSem, mutex; 

bool isMatch, isPaper, isTobacco;


void* agentTP() // Agent that provides tobacco and paper
{
	for (int i=0; i<6; ++i) 
	{
		sleep(0.001*(rand()%200)); // sleep up to 200ms
		sem_wait(&agentSem);// P(agentSem)
		sem_post(&paper); // V(paper)
		sem_post(&tobacco);// V(tobacco)
	}
}


void* agentPM() // Agent that provides paper and match
{
	for (int i=0; i<6; ++i) 
	{
		sleep(0.001*(rand()%200)); 
		sem_wait(&agentSem);
		sem_post(&paper);
		sem_post(&match);
	}
}

void* agentMT() // Agent that provides match and tobacco
{
	for (int i=0; i<6; ++i) 
	{
		sleep(0.001*(rand()%200)); 
		sem_wait(&agentSem);
		sem_post(&match);
		sem_post(&tobacco);
	}
}

void* pusherT()
{
	//Each pusher loops 12 times
	for (int i=0; i<12; ++i)
	{
		sem_wait(&tobacco);//P(tobacco)
		sem_wait(&mutex);//P(mutex)
		if (isPaper) 
		{
			isPaper = false;
			sem_post(&matchSem);//V(matchSem)
		}
		else if (isMatch) 
		{
			isMatch = false;
			sem_post(&paperSem);//V(paperSem)
		}
		else isTobacco = true;
		sem_post(&mutex);//V(mutex)
	}
}

void* pusherP()
{
	for (int i=0; i<12; ++i)
	{
		sem_wait(&paper);
		sem_wait(&mutex);
		if (isTobacco) 
		{
			isTobacco = false;		
			sem_post(&matchSem);
		}
		else if (isMatch) 
		{
			isMatch = false;			
			sem_post(&tobaccoSem);
		}
		else isPaper = true;
		sem_post(&mutex);
	}	
}

void* pusherM()
{
	for (int i=0; i<12; ++i)
	{
		sem_wait(&match);
		sem_wait(&mutex);
		if (isPaper) 
		{
			isPaper = false;	
			sem_post(&tobaccoSem);
		}
		else if (isTobacco) 
		{
			isTobacco = false;
			sem_post(&paperSem);
		}
		else isMatch = true;
		sem_post(&mutex);
	}	
}

void* smokerT()
{
	//Each smoker finishes 3 cigarettes before exiting
	for (int i=0; i<3; ++i)
	{
		sleep(0.001*(rand()%50)); //sleep up to 50ms for making the cigarette
		sem_wait(&tobaccoSem); // P(tobaccoSem)
		printf("Tobacco smoker is making a cigarette\n");
		sleep(0.001*(rand()%50)); //sleep again for smoking
		sem_post(&agentSem);// V(agentSem)
		printf("Tobacco smoker is smoking a cigarette\n");
	}
}

void* smokerP()
{
	for (int i=0; i<3; ++i)
	{
		sleep(0.001*(rand()%50));
		sem_wait(&paperSem);
		printf("Paper smoker is making a cigarette\n");
		sleep(.001*(rand()%50)); 
		sem_post(&agentSem);
		printf("Paper smoker is smoking a cigarette\n");
	}
}

void* smokerM()
{
	for (int i=0; i<3; ++i)
	{
		sleep(0.001*(rand()%50));
		sem_wait(&matchSem);
		printf("Match smoker is making a cigarette\n");		
		sleep(0.001*(rand()%50));
		sem_post(&agentSem);
		printf("Match smoker is smoking a cigarette\n");
	}
}



int main() 
{ 
	srand(time(0));
	
	isTobacco = false;
	isPaper = false;
	isMatch = false;
	
	// Initialize the semaphores
	sem_init(&tobaccoSem, 0, 0); 
	sem_init(&paperSem, 0, 0); 
	sem_init(&matchSem, 0, 0); 
	sem_init(&mutex, 0, 1); 

	sem_init(&agentSem, 0, 1);
	sem_init(&tobacco, 0, 0);
	sem_init(&paper, 0, 0);
	sem_init(&match, 0, 0);
	
	pthread_t smokers[6];
	pthread_t pushers[3];
	pthread_t agents[3];
	for (int i=0; i<2; ++i)
	{
		pthread_create(&smokers[i],NULL,smokerM,NULL);

	}
	for (int i=2; i<4; ++i)
	{
		pthread_create(&smokers[i],NULL,smokerT,NULL);

	}
	for (int i=4; i<6; ++i)
	{
		pthread_create(&smokers[i],NULL,smokerP,NULL);

	}
	
	pthread_create(&agents[0],NULL,agentTP,NULL);
	pthread_create(&agents[1],NULL,agentMT,NULL);
	pthread_create(&agents[2],NULL,agentPM,NULL);

	pthread_create(&pushers[0],NULL,pusherM,NULL);
	pthread_create(&pushers[1],NULL,pusherT,NULL);
	pthread_create(&pushers[2],NULL,pusherP,NULL);
	
	
	for(int i = 0; i < 6; ++i)
	{
		pthread_join(smokers[i],NULL);
	}
	printf("Each smoker has finished 3 cigarattes and are leaving.\n");
	for (int i = 0; i < 2; ++i)
	{
		pthread_join(agents[i],NULL);
		pthread_join(pushers[i],NULL);
	}
	
	return 0;
} 


