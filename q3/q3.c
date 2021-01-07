#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include <semaphore.h>
//Struct for Artists(Musicians+Singer) 
typedef struct Artists{
	int id;//id of the artist
	int s_id;//id for the stage
	char name[50];//name of artist
	char instrument;//instrument character
	int fl;//fl=1 for acoustic , fl=2 for electric
	int v; // v=1 for allowed , v=2 goes back
	int time;//time in secs of arrival
	pthread_t a_tid1;//defining thread for acoustic
	pthread_t a_tid2;//defining other thread for electric
	pthread_t a_tid3;//defining thread for timeout
	pthread_mutex_t a_mutex;//defining mutex
	pthread_mutex_t a_mutex1;//defining mutex
	pthread_cond_t a_cond;//defining condition variable
	pthread_cond_t a_cond2;//defining condition variable
} Artists;
//Struct for Stages
typedef struct Stages{
	int id;//id of stage
	int a_id;//id of artist
	int s_id;//id of singer
} Stages;

int a;//number of acoustic stages.
int e;//number of electric stages.
int a_s;//number of singers in acoustic stage.
int e_s;//number of singers in electric stage.
int k;//number of musicians and singers.
int t1;//minimum duration of performance.
int t2;//maximum duration of performance.
int t;//waiting time for artists.
int c;//number of coordinators.
Artists *Artist;//Artist(Musicians+Singers) Struct
Stages *E_Stage;//electric Stage struct
Stages *A_Stage;//acoustic Stage struct
sem_t acoustic;//semaphore for acoustic stages
sem_t electric;//semaphore for electric stages
sem_t coordinators;//semaphore for coordinators.

void *musical_mayhem (void *input)
{
	Artists *A =(Artists*)input;
	int array[a+e];
	sleep(A->time);
	printf("\033[01;33m");
	printf("%s who is a Singer arrived at Srujana\n",A->name);
	int kk = rand()%(a+e)+1;
	pthread_mutex_lock(&(A->a_mutex1));
	if(kk>a)
	{
		kk=kk-a;
		sem_wait(&electric);
		pthread_cond_signal(&(A->a_cond));
		sleep(1);
		if(A->v==1)
		{
			if(E_Stage[kk].a_id==0)
			{
				E_Stage[kk].s_id=A->id;
				A->s_id = kk;
				A->fl = 2;
				int p_t;
				if(t2==0||t1==t2)
				{
					p_t = 0;
				}
				else
				{
				 	p_t = rand()%(t2-t1+1)+t1;
				}
				printf("\033[0;32m");
				printf("%s who is a singer starts the solo performance at electric stage for %d secs\n",A->name,p_t);
				sleep(p_t);
				printf("\033[0;35m");
				printf("%s who is a singer ended the solo performance at electric stage\n",A->name);
				E_Stage[kk].s_id=0;
				sem_post(&electric);
				sem_wait(&coordinators);
				sleep(2);
				printf("\033[0;33m");
				printf("%s gets the t-shirt\n",A->name);
				sem_post(&coordinators);
				printf("%s leaves after being happy\n",A->name);
				pthread_mutex_unlock(&(A->a_mutex1));
				return NULL;
			}
			else
			{
				sem_post(&electric);
				E_Stage[kk].s_id=A->id;
				A->s_id = kk;
				A->fl = 2;
				printf("\033[0;35m");
				printf("%s joins the musician %s\n",A->name,Artist[E_Stage[kk].a_id].name);
				pthread_cond_wait(&(A->a_cond2),&(A->a_mutex1));
				sem_wait(&coordinators);
				sleep(2);
				printf("\033[0;33m");
				printf("%s gets the t-shirt\n",A->name);
				sem_post(&coordinators);
				printf("%s leaves after being happy\n",A->name);
				pthread_mutex_unlock(&(A->a_mutex1));
				return NULL;
			}
		}
		else
		{
			sem_post(&electric);
			printf("\033[0;31m");
			printf("%s who is a singer leaves Srujana due to impatience\n",A->name);
			pthread_mutex_unlock(&(A->a_mutex1));
			return NULL;
		}	
	}
	else
	{
		sem_wait(&acoustic);
		pthread_cond_signal(&(A->a_cond));
		sleep(1);
		if(A->v==1)
		{
			if(A_Stage[kk].a_id==0)
			{
				A_Stage[kk].s_id=A->id;
				A->s_id = kk;
				A->fl =1;
				int p_t;
				if(t2==0||t1==t2)
				{
					p_t = 0;
				}
				else
				{
				 	p_t = rand()%(t2-t1+1)+t1;
				}
				printf("\033[0;32m");
				printf("%s who is a singer starts the solo performance at acoustic stage for %d secs\n",A->name,p_t);
				sleep(p_t);
				printf("\033[0;35m");
				printf("%s who is a singer ended the solo performance at acoustic stage\n",A->name);
				A_Stage[k].s_id=0;
				sem_post(&acoustic);
				sem_wait(&coordinators);
				sleep(2);
				printf("\033[0;33m");
				printf("%s gets the t-shirt\n",A->name);
				sem_post(&coordinators);
				printf("%s leaves after being happy\n",A->name);
				pthread_mutex_unlock(&(A->a_mutex1));
			}
			else
			{
				sem_post(&acoustic);
				A_Stage[kk].s_id=A->id;
				A->s_id = kk;
				A->fl = 2;
				printf("\033[0;35m");
				printf("%s joins the musician %s\n",A->name,Artist[E_Stage[kk].a_id].name);
				pthread_cond_wait(&(A->a_cond2),&(A->a_mutex1));
				sem_wait(&coordinators);
				sleep(2);
				printf("\033[0;33m");
				printf("%s gets the t-shirt\n",A->name);
				sem_post(&coordinators);
				printf("%s leaves after being happy\n",A->name);
				pthread_mutex_unlock(&(A->a_mutex1));
				return NULL;
			}
		}
		else
		{
			sem_post(&acoustic);
			printf("\033[0;31m");
			printf("%s who is a singer leaves Srujana due to impatience\n",A->name);
			pthread_mutex_unlock(&(A->a_mutex1));
			return NULL;
		}
	}
}
void *electric_i (void *input)
{
	Artists *A =(Artists*)input;
	sleep(A->time);
	if(A->fl==0)
	{
		sem_wait(&electric);
		if(A->fl!=0)
		{
			sem_post(&electric);
			return NULL;
		}
		else
		{
			A->fl=2;
			pthread_mutex_lock(&(A->a_mutex1));
			pthread_cond_signal(&(A->a_cond));
			sleep(1);
			if(A->v==1)
			{
				for(int j=1;j<=a;j++)
				{
					if(E_Stage[j].a_id==0)
					{
						E_Stage[j].a_id=A->id;
						A->s_id = j;
						break;
					}
				}
				int p_t;
				if(t2==0||t1==t2)
				{
					p_t = 0;
				}
				else
				{
				 	p_t = rand()%(t2-t1+1)+t1;
				}
				printf("\033[0;32m");
				printf("%s who plays %s starts the performance at electric stage for %d secs\n",A->name,&A->instrument,p_t);
				sleep(p_t);
				if(E_Stage[A->s_id].s_id==0)
				{
					sem_post(&electric);
					E_Stage[A->s_id].a_id=0;
					printf("\033[0;35m");
					printf("%s performance on electric stage ended\n",A->name);
					sem_wait(&coordinators);
					sleep(2);
					printf("\033[0;33m");
					printf("%s gets t-shirt\n",A->name);
					sem_post(&coordinators);
					printf("%s leaves after being happy\n",A->name);
					pthread_mutex_unlock(&(A->a_mutex1));
					return NULL;
				}
				else
				{
					sleep(2);
					sem_post(&electric);
					printf("\033[0;35m");
					printf("%s performance on electric stage ended\n",A->name);
					pthread_cond_signal(&(Artist[A_Stage[A->s_id].s_id].a_cond2));
					E_Stage[A->s_id].a_id=0;
					E_Stage[A->s_id].s_id=0;
					sem_wait(&coordinators);
					sleep(2);
					printf("\033[0;33m");
					printf("%s gets t-shirt\n",A->name);
					sem_post(&coordinators);
					printf("%s leaves after being happy\n",A->name);
					pthread_mutex_unlock(&(A->a_mutex1));
					return NULL;
				}
			}
			else
			{
				sem_post(&electric);
				printf("\033[0;31m");
				printf("%s who plays %s leaves Srujana due to impatience\n",A->name,&A->instrument);
				pthread_mutex_unlock(&(A->a_mutex1));
				return NULL;
			}
		}
	}
	else 
	{
		return NULL;
	}	
}
void *acoustic_i (void *input)
{
	Artists *A =(Artists*)input;
	sleep(A->time);
	printf("\033[01;33m");
	printf("%s who plays %s arrived at Srujana\n",A->name,&A->instrument);
	if(A->fl==0)
	{
		sem_wait(&acoustic);
		if(A->fl!=0)
		{
			sem_post(&acoustic);
			return NULL;
		}
		else
		{
			A->fl=1;
			pthread_mutex_lock(&(A->a_mutex1));
			pthread_cond_signal(&(A->a_cond));
			sleep(1);
			if(A->v==1)
			{
				for(int j=1;j<=a;j++)
				{
					if(A_Stage[j].a_id==0)
					{
						A_Stage[j].a_id=A->id;
						A->s_id = j;
						break;
					}
				}
				int p_t;
				if(t2==0||t1==t2)
				{
					p_t = 0;
				}
				else
				{
				 	p_t = rand()%(t2-t1+1)+t1;
				}
				printf("\033[0;32m");
				printf("%s who plays %s starts the performance at acoustic stage for %d secs\n",A->name,&A->instrument,p_t);
				sleep(p_t);
				if(A_Stage[A->s_id].s_id==0)
				{
					sem_post(&acoustic);
					printf("\033[0;35m");
					printf("%s performance on acoustic stage ended\n",A->name);
					A_Stage[A->s_id].a_id=0;
					sem_wait(&coordinators);
					sleep(2);
					printf("\033[0;33m");
					printf("%s gets t-shirt\n",A->name);
					sem_post(&coordinators);
					printf("%s leaves after being happy\n",A->name);
					pthread_mutex_unlock(&(A->a_mutex1));
					return NULL;
				}
				else
				{
					sleep(2);
					sem_post(&acoustic);
					printf("\033[0;35m");
					printf("%s performance on acoustic stage ended\n",A->name);
					pthread_cond_signal(&(Artist[A_Stage[A->s_id].s_id].a_cond2));
					A_Stage[A->s_id].a_id=0;
					A_Stage[A->s_id].s_id=0;
					sem_wait(&coordinators);
					sleep(2);
					printf("\033[0;33m");
					printf("%s gets t-shirt\n",A->name);
					sem_post(&coordinators);
					printf("%s leaves after being happy\n",A->name);
					pthread_mutex_unlock(&(A->a_mutex1));
					return NULL;
				}
			}
			else
			{
				sem_post(&acoustic);
				printf("\033[0;31m");
				printf("%s who plays %s leaves Srujana due to impatience\n",A->name,&A->instrument);
				pthread_mutex_unlock(&(A->a_mutex1));
				return NULL;
			}
		}
	}
	else
	{
		return NULL;
	}
}
void *check (void *input)
{
	struct timespec ts;
	Artists *A =(Artists*)input;
	sleep(A->time);
	pthread_mutex_lock(&(A->a_mutex));
	clock_gettime(CLOCK_REALTIME, &ts);
	int b_t = ts.tv_sec;
    ts.tv_sec = ts.tv_sec + t;
    int rc=pthread_cond_timedwait(&(A->a_cond),&(A->a_mutex),&ts);
    clock_gettime(CLOCK_REALTIME, &ts);
	int a_t = ts.tv_sec;
	int r=a_t-b_t;
	if(r>=t)
	{
		A->v=2;
	}
	else
	{
		A->v=1;
	}
	pthread_mutex_unlock(&(A->a_mutex));
	return NULL;
}
void *electri (void *input)
{
	Artists *A =(Artists*)input;
	A->fl=2;
	pthread_mutex_lock(&(A->a_mutex1));
	sleep(A->time);
	printf("\033[01;33m");
	printf("%s who plays %s arrived at Srujana\n",A->name,&A->instrument);
	sem_wait(&electric);
	pthread_cond_signal(&(A->a_cond));
	sleep(1);
	if(A->v==1)
	{
		for(int j=1;j<=a;j++)
		{
			if(E_Stage[j].a_id==0)
			{
				E_Stage[j].a_id=A->id;
				A->s_id = j;
				break;
			}
		}
		int p_t;
		if(t2==0||t1==t2)
		{
			p_t = 0;
		}
		else
		{
		 	p_t = rand()%(t2-t1+1)+t1;
		}
		printf("\033[0;32m");
		printf("%s who plays %s starts the performance at electric stage for %d secs\n",A->name,&A->instrument,p_t);
		sleep(p_t);
		if(E_Stage[A->s_id].s_id==0)
		{
			sem_post(&electric);
			printf("\033[0;35m");
			printf("%s performance on electric stage ended\n",A->name);
			E_Stage[A->s_id].a_id=0;
			sem_wait(&coordinators);
			sleep(2);
			printf("\033[0;33m");
			printf("%s gets t-shirt\n",A->name);
			sem_post(&coordinators);
			printf("%s leaves after being happy\n",A->name);
			pthread_mutex_unlock(&(A->a_mutex1));
			return NULL;
		}
		else
		{
			sleep(2);
			sem_post(&electric);
			printf("\033[0;35m");
			printf("%s performance on electric stage ended\n",A->name);
			pthread_cond_signal(&(Artist[A_Stage[A->s_id].s_id].a_cond2));
			E_Stage[A->s_id].a_id=0;
			E_Stage[A->s_id].s_id=0;
			sem_wait(&coordinators);
			sleep(2);
			printf("\033[0;33m");
			printf("%s gets t-shirt\n",A->name);
			sem_post(&coordinators);
			printf("%s leaves after being happy\n",A->name);
			pthread_mutex_unlock(&(A->a_mutex1));
			return NULL;
		}
	}
	else
	{
		sem_post(&electric);
		printf("\033[0;31m");
		printf("%s who plays %s leaves Srujana due to impatience\n",A->name,&A->instrument);
		pthread_mutex_unlock(&(A->a_mutex1));
		return NULL;
	}
}
void *acousti (void *input)
{
	Artists *A =(Artists*)input;
	A->fl=1;
	pthread_mutex_lock(&(A->a_mutex1));
	sleep(A->time);
	printf("\033[01;33m");
	printf("%s who plays %s arrived at Srujana\n",A->name,&A->instrument);
	if(a==0)
	sem_wait(&acoustic);
	pthread_cond_signal(&(A->a_cond));
	sleep(1);
	if(A->v==1)
	{
		for(int j=1;j<=a;j++)
		{
			if(A_Stage[j].a_id==0)
			{
				A_Stage[j].a_id=A->id;
				A->s_id = j;
				break;
			}
		}
		int p_t;
		if(t2==0||t1==t2)
		{
			p_t = 0;
		}
		else
		{
		 	p_t = rand()%(t2-t1+1)+t1;
		}
		printf("\033[0;32m");
		printf("%s who plays %s starts the performance at acoustic stage for %d secs\n",A->name,&A->instrument,p_t);
		sleep(p_t);
		if(A_Stage[A->s_id].s_id==0)
		{
			sem_post(&acoustic);
			A_Stage[A->s_id].a_id=0;
			printf("\033[0;35m");
			printf("%s performance on acoustic stage ended\n",A->name);
			sem_wait(&coordinators);
			sleep(2);
			printf("\033[0;33m");
			printf("%s gets t-shirt\n",A->name);
			sem_post(&coordinators);
			printf("%s leaves after being happy\n",A->name);
			pthread_mutex_unlock(&(A->a_mutex1));
			return NULL;
		}
		else
		{
			sleep(2);
			sem_post(&acoustic);
			printf("\033[0;35m");
			printf("%s performance on acoustic stage ended\n",A->name);
			pthread_cond_signal(&(Artist[A_Stage[A->s_id].s_id].a_cond2));
			A_Stage[A->s_id].a_id=0;
			A_Stage[A->s_id].s_id=0;
			sem_wait(&coordinators);
			sleep(2);
			printf("\033[0;33m");
			printf("%s gets t-shirt\n",A->name);
			sem_post(&coordinators);
			printf("%s leaves after being happy\n",A->name);
			pthread_mutex_unlock(&(A->a_mutex1));
			return NULL;
		}
	}
	else
	{
		sem_post(&acoustic);
		printf("\033[0;31m");
		printf("%s who plays %s leaves Srujana due to impatience\n",A->name,&A->instrument);
		pthread_mutex_unlock(&(A->a_mutex1));
		return NULL;
	}
}
int main()
{
	scanf("%d %d %d %d %d %d %d",&k,&a,&e,&c,&t1,&t2,&t);
	sem_init(&acoustic, 0, a);
	sem_init(&electric, 0, e);
	sem_init(&coordinators, 0, c);
	Artist = (Artists*)malloc(sizeof(Artists)*(k+2));
	E_Stage = (Stages*)malloc(sizeof(Stages)*(e+2));
	A_Stage = (Stages*)malloc(sizeof(Stages)*(a+2));
	for(int i=1;i<=a;i++)
	{
		A_Stage[i].id=i;
		A_Stage[i].a_id=0;
		A_Stage[i].s_id=0;
	}
	for(int i=1;i<=e;i++)
	{
		E_Stage[i].id=i;
		E_Stage[i].a_id=0;
		E_Stage[i].s_id=0;
	}
	for(int i=1;i<=k;i++)
	{
		scanf("%s",Artist[i].name);
		scanf("%s",&(Artist[i].instrument));
		scanf("%d",&(Artist[i].time));
	}
	if(c==0)
	{
		printf("Not possible c>=1\n");
	}
	else
	{
		for(int i=1;i<=k;i++)
		{
			Artist[i].id=i;
			Artist[i].fl=0;
			pthread_mutex_init(&(Artist[i].a_mutex),NULL);
			pthread_cond_init(&(Artist[i].a_cond),NULL);
			pthread_mutex_init(&(Artist[i].a_mutex1),NULL);
			pthread_cond_init(&(Artist[i].a_cond2),NULL);
			if(Artist[i].instrument=='v')
			{
				Artist[i].v=0;
				pthread_create(&(Artist[i].a_tid2),NULL,acousti,(void*)(&Artist[i]));
				pthread_create(&(Artist[i].a_tid1),NULL,check,(void*)(&Artist[i]));
			}
			else if(Artist[i].instrument=='b')
			{
				Artist[i].v=0;
				pthread_create(&(Artist[i].a_tid1),NULL,check,(void*)(&Artist[i]));
				pthread_create(&(Artist[i].a_tid2),NULL,electri,(void*)(&Artist[i]));
			}
			else if(Artist[i].instrument=='s')
			{
				Artist[i].v=0;
				pthread_create(&(Artist[i].a_tid1),NULL,check,(void*)(&Artist[i]));
				pthread_create(&(Artist[i].a_tid2),NULL,musical_mayhem,(void*)(&Artist[i]));
			}
			else
			{
				Artist[i].v=0;
				pthread_create(&(Artist[i].a_tid1),NULL,check,(void*)(&Artist[i]));
				pthread_create(&(Artist[i].a_tid2),NULL,electric_i,(void*)(&Artist[i]));
				pthread_create(&(Artist[i].a_tid3),NULL,acoustic_i,(void*)(&Artist[i]));
			}
		}
		for(int i=1;i<=k;i++)
		{
			if(Artist[i].instrument=='v'||Artist[i].instrument=='b'||Artist[i].instrument=='s')
			{
				pthread_join(Artist[i].a_tid1,NULL);
				pthread_join(Artist[i].a_tid2,NULL);
			}
			else
			{
				pthread_join(Artist[i].a_tid1,NULL);
				pthread_join(Artist[i].a_tid2,NULL);
				pthread_join(Artist[i].a_tid3,NULL);
			}
			pthread_mutex_destroy(&(Artist[i].a_mutex));
			pthread_cond_destroy(&(Artist[i].a_cond));
			pthread_mutex_destroy(&(Artist[i].a_mutex1));
			pthread_cond_destroy(&(Artist[i].a_cond2));
		}
		printf("\033[1;36m");
		for(int i=1;i<=k;i++)
		{
			if(Artist[i].s_id!=0)
			{
				if(Artist[i].fl==1)
				{
					printf("%s has performed on acoustic stage %d\n",Artist[i].name,Artist[i].s_id);
				}
				else
				{
					printf("%s has performed on electric stage %d\n",Artist[i].name,Artist[i].s_id);
				}
			}
		}
	}
	printf("\033[0;34m");
	printf("Finished\n");
	return 0;
}