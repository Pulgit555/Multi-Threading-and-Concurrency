
# Simulated System For Musical Concert
ensuring a nice management of 5 types of musicians to perform on 2 types of stages along with maximum waiting time, it uses semaphores, mutexes and threads.

## How to Run ?
compile the code using `gcc q3.c -lpthread`
and run the code using `./a.out`

### Question Overview
- There are a acoustic and e electric stages.
- k musicians who can perform on a specific stage or can perform on both the stages.They will perform for a random  time t'. 
- Whereas singers can perform solo or can join other musician also and increases the time of performance of the musician by 2 sec. 
- After arriving they will not wait for more than t secs and they will leave Srujana. 
- After that there are c coordinators who will take 2 secs to give t-shirts to artist who had performed on stage.

### Structs Used

1.	**Artists** 
```
int id; //id of the artist
int s_id; //id for the stage
char name[50]; //name of artist
char instrument; //instrument character
int fl; //fl=1 for acoustic, fl=2 for electric
int v; // v=1 for allowed, v=2 goes back
int time; //time in secs of arrival
pthread_t a_tid1; //defining thread for acoustic
pthread_t a_tid2; //defining other thread for electric
pthread_t a_tid3; //defining thread for timeout
pthread_mutex_t a_mutex; //defining mutex
pthread_mutex_t a_mutex1; //defining mutex
pthread_cond_t a_cond; //defining condition variable
pthread_cond_t a_cond2; //defining condition variable
``` 
2.	**Stages**
```
int id; //id of stage
int a_id; //id of artist
int s_id; //id of singer
```

### Variables Used 
```
int a; //number of acoustic stages.
int e; //number of electric stages.
int k; //number of musicians and singers.
int t1; //minimum duration of performance.
int t2; //maximum duration of performance.
int t; //waiting time for artists.
int c; //number of coordinators.
Artists *Artist; //Artist(Musicians+Singers) Struct
Stages *E_Stage; //electric Stage struct
Stages *A_Stage; //acoustic Stage struct
```

### Semaphores Used
```
sem_t acoustic; //semaphore for acoustic stages
sem_t electric; //semaphore for electric stages
sem_t coordinators; //semaphore for coordinators.
```
### Functions Used
1. **Check**
- This is used to check whether a performer wait for more than t secs or not by using clock_gettime(CLOCK_REALTIME, &ts) to get current relative time and then using pthread_cond_wait(), - this thread will wait for other artist thread to either signal this wait condition when he gets empty stage or after specified t secs it will automatically get over. 
- and then if time taken in getting the stage is less than t then it will set the flag(A->v) to 1 else to 2.
```
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
```

2. **acousti** 
- Musicians who palys violin ('v') only perform on acoustic stage . So they wait for 1 acoustic stage to get empty(using sem_wait(&acoustic)) using semaphore acoustic. 
- After getting an empty stage ,check for time taken in getting stage, if time taken is greater than t sec then he will leave Srujana.
- If time is less than t sec then he/she will perform for t' secs(which is found using rand() from t1-t2). 
- After his performance, he will check whether any singer joined the stage or not. 
- If singer joined then performance will increase by 2 secs otherwise performance end then only and then he/she leave the stage and wait for 1 coordinator to get t-shirt.
- Coordinator will take 2 secs to give t-shirt and after that he will leave.This is implemented using acousti function. 
```
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
```

3. **electri** 
- Musicians who play bass('b') only perform on electric stage and similar to point(1), this is implemented using function electri.
```
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
```

4. **electric_i and acoustic_i**
- Musicians who play piano('p') and guitar('g'), they can perform on both acoustic and electric stages. 
- First we need to ckeck on which stage musician perform electric or acoustic, for that I am using flag (A->fl) and semaphores(acoustic and electric ).
- Whichever empty stage he/she gets first will change the flag and that stage gets fixed and on other stage using sem_post he/she can leave that if the stage is electric , logic is same as case of 2 and if the stage is acoustic , logic is same as case of 1.
- This is implemented using electric_i and acoustic_i function.
```
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
```

5. **musical_mayhem**
- Singers are special cases, They will choose id randomly from (1-a+e) as kk.
- If kk>a then it will go to electrical 
- Else it will go to acoustic.
- When it is in electrical or acoustic it will check whether at that stage is there any other artist also then it will join the performance otherwise it will do solo performance.
- After performing it will wait for the coordinator to give him/her a t-shirt and all other logic is same as point (1) in function 
	musical_mayhem
```
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
```
