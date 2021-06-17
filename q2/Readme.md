
# Simulated System For Vaccination Drive
Where Pharmaceutical Company produces vaccines and deliver the vaccine to Vaccination Zone, where It simultaneously create zones and vaccinate students using mutexe locks and threads.

## How to Run ?
- compile the code using `gcc q2.c -lpthread` 
- and run the code using `./a.out`

### Question Overview
- **n Pharmaceutical Companies**
Each company takes w seconds to prepare r batches having p vaccines. Each vaccine of this company has xi probability. They will distribute the vaccine to the vaccination zone and wait for all its batches produced to get used.
- **m Vaccination Zones**
Each Zone waits to recieve 1 batch of vaccine from any company and then it will create k slots and wait for students to arrive, when some students arrive it will vaccinate less than or equal to k. While it is vaccinating it will not create new slots.
- **o Students**
Each student waits for a zone to create a slot where he can move. After he gets the vaccine he gets an antibody test, if he test positive for antibody he can now leave the process but if he test negative for antibody he has to repeat this process again. He can get vaccine maximum of 3 times.
### Structs Used

1.	**Company** - denoting Pharmaceutical Companies
```
int id; //denoting number of company 1-n
int r; //number of batches
int p; //number of vaccine in each batch
int x; //probability of vaccine
char s[6]; //stores the status for each batch(P-produced; G-given to zone; D-distributed)
pthread_t c_tid; //defining thread
pthread_mutex_t c_mutex; //defining mutex
pthread_cond_t c_cond; //defining condition variable
``` 
2.	**Zone** - denoting Vaccination Zone
```
int id; //denoting number of zone 1-m
int c_id; //denoting number of company who produced the batch for this zone.
int s_id[10]; //denoting the ids of student given vaccine.
int k; //number of slots
int b; //slots left
int g; //number of vaccine left in that zone
int h; //currently number of students left with test having vaccine.
int v; //status of zone(0-waiting for company; 1-already have ; 2-waiting for stud)
char s[10]; //stores the status for each slot(C-created ; A-assigned)
pthread_t z_tid; //defining thread
pthread_mutex_t z_mutex; //defining mutex
pthread_cond_t z_cond; //defining condition variable
pthread_cond_t z_cond1; //defining condition variable
```
3.	**Student** - denoting students
```
int id; //denoting number of student 1-o
int z_id; //denoting number of zone where he gets vaccinated.
int count; //number of times he is vaccinated
int v; //status for student(0-waiting ; 1-given ; 2-done) 
pthread_t s_tid; //defining thread
pthread_mutex_t s_mutex; //defining mutex
pthread_cond_t s_cond; //defining condition variable
```

### Variables Used 
```
int n; //number of companies
int m; //number of zones
int o; //number of students
Company *Comp ; //Company struct
Zone *Zon; //Zone struct
Student *Stud ; //Student struct
int a; //number of students currently available .
int su=0; //number of students successfully vaccinated .
```

### Functions Used
1. **Prod_vaccine**
- This function is used to implement role of Pharmaceutical Companies.
- If the number of current batches are not zero it will wait till its all batches are used. 
- If the batches are zero then it will prepare r batches(1-5) having vaccine (10-20) in each batch by taking w second(2-5) then it will signal all the zones who dont have any batch and currently not in vaccination phase to come to take batch.
- Each thread will do this function till a>0.
```
void* Prod_vaccine(void* input)
{
	Company *C = (Company*) input;
	pthread_mutex_lock(&(C->c_mutex));
	while(a>0)
	{
		if(C->r!=0)
		{
			for(int j=1;j<=m;j++)
			{
				if(Zon[j].v==0)
				{
					pthread_cond_signal(&(Zon[j].z_cond));
				}
			}
			printf("\033[0;32m");
			printf("Pharmaceutical Company %d is waiting for all its batches get used\n",C->id);
			pthread_cond_wait(&(C->c_cond),&(C->c_mutex));
		}
		else
		{
			int aa=1,bb=5,cc=10,dd=20,ee=2,ff=5,gg;
			C->r= rand()%(bb-aa+1)+aa;
			C->p = rand()%(dd-cc+1)+cc;
			printf("\033[0;32m");
			printf("Pharmaceutical Company %d is preparing %d batches of vaccines %d in each batch with success probability percentage %d %c\n",C->id,C->r,C->p,C->x,37);
			for(int j=1;j<=C->r;j++)
			{
				C->s[j]='P';
			}
			int w = rand()%(ff-ee+1)+ee;
			sleep(w);
			printf("\033[0;31m");
			printf("Pharmaceutical Company %d has prepared %d batches of vaccine %d in each batch with success probability percentage %d %c\n",C->id,C->r,C->p,C->x,37);
			for(int j=1;j<=m;j++)
			{
				if(Zon[j].v==0)
				{
					pthread_cond_signal(&(Zon[j].z_cond));
				}
			}
		}
	}
	pthread_mutex_unlock(&(C->c_mutex));
	return NULL;
}
```

2. **Get_vaccine** 
- This function is used to implement role of students. 
- If student does not get any slot it will wait till any vaccination zone give it a signal that slot is ready it will wait.
- When it gets the slot, flag v=1 then their will be antibodies test for each company.
- Probability is given, consider new probability as probability*100 and then generate random variable between 1-100. 
- If the variable <= probability then he will test positive for antibodies and here function end for this .
- Else he will test negative for antibodies and then again come to zone.
- After that it will signal the zone that vaccination is done. 
- It will do this function till either he tested positive for antibodies or all 3 chances are over.
```
void* Get_vaccine(void* input)
{
	Student *S = (Student*) input;
	pthread_mutex_lock(&(S->s_mutex));
	int aaa=0;
	sleep(rand()%5+1);
	while(S->count>0)
	{
		if(aaa!=0)
		{
			printf("\033[0;33m");
			printf("Student %d has arrived for his %d round of Vaccination\n",S->id,4-S->count);
		}
		aaa++;
		if(S->v==0)
		{
			S->pp=0;
			printf("\033[1;33m");
			printf("Student %d is waiting to be allocated a Vaccination Zone\n",S->id);
			pthread_cond_wait(&(S->s_cond),&(S->s_mutex));
			S->pp=1;
		}
		else
		{
			printf("\033[0;32m");
			printf("Student %d assigned a slot on the Vaccination Zone %d and waiting to be vaccinated\n",S->id,S->z_id);
			int y = Comp[Zon[S->z_id].c_id].x;
			int w = rand()%100+1;
			int zi;
			printf("Student %d on Vaccination Zone %d has been vaccinated which has success probability %d %c\n",S->id,S->z_id,Comp[Zon[S->z_id].c_id].x,37);
			if(w<=y)
			{
				su++;
				S->count=0;
				a--;
				zi=2;
			}
			else
			{
				if(S->count==1)
				{
					a--;
				}
				zi=0;
				S->count--;
			}
			sleep(1);
			if(w<=y)
			{
				printf("\033[0;33m");
				printf("Student %d has tested positive for antibodies\n",S->id);
				S->v=0;
			}
			else
			{
				printf("\033[0;31m");
				printf("Student %d has tested negative for antibodies\n",S->id);
				S->v=0;
			}
			if(Zon[S->z_id].g==0)
			{
				Zon[S->z_id].v=0;
			}
			else
			{
				Zon[S->z_id].v=1;
			}
			pthread_cond_signal(&(Zon[S->z_id].z_cond1));
		}
	}
	pthread_mutex_unlock(&(S->s_mutex));
	return NULL;
}
```

3. **Dist_vaccine** 
- This function is used to implement Vaccination Zones.
- If flag =0 then it will search for a company, if any company does not have any batch left it will give signal to company (using pthread_cond_signal) and the zone will wait for a company to produce a batch and signal the zone for batch is prepared .
- If flag = 2 that means vaccination phase is going it will wait for students to end their vaccination.
- If flag = 1 that is it have a batch or any slot made previously has left, if slots are left then it will use these slots otherwise it will create new slot (1-min(8,students left,vaccines left in zone)) then it will find students who are left with their vaccination zone will choose the students and then signal them that you are given a slot. This function also continues till students leave.
```
void* Dist_vaccine(void* input)
{
	Zone *Z = (Zone*) input;
	pthread_mutex_lock(&(Z->z_mutex));
	int aaa=0;
	sleep(1);
	while(a>0)
	{
		if(Z->v==0)
		{
			if(aaa!=0)
			{
				printf("\033[0;32m");
				printf("Vaccination Zone %d has run out of vaccines\n",Z->id);
			}
			aaa++;
			Z->qq=0;
			printf("\033[0;33m");
			printf("Vaccination Zone %d is waiting to be filled\n",Z->id);
			pthread_cond_wait(&(Z->z_cond),&(Z->z_mutex));
			Z->qq=1;
			for(int j=1;j<=n;j++)
			{
				if(Comp[j].r!=0)
				{
					for(int l=1;l<=Comp[j].r;l++)
					{
						if(Comp[j].s[l]=='P')
						{
							Z->v=1;
							Z->g=Comp[j].p;
							Z->c_id=Comp[j].id;
							Comp[j].s[l]='G';
							printf("\033[1;34m");
							printf("Pharmaceutical Company %d is delivering a vaccine batch to Vaccination Zone %d which has success probability %d %c\n",j,Z->id,Comp[j].x,37);
							break;
						}
					}
				}
				if(Z->v==1)
				{
					break;
				}
			}
			for(int j=1;j<=n;j++)
			{
				if(Comp[j].r==0)
				{	
					printf("\033[1;34m");
					printf("All the vaccines prepared by Pharmaceutical Company %d are emptied. Resumed production now\n",j);	
					pthread_cond_signal(&(Comp[j].c_cond));
				}
			}
		}
		else if(Z->v==2)
		{
			Z->pp=0;
			printf("\033[0;33m");
			printf("Vaccination Zone %d is waiting for students to finish its vaccinating phase\n",Z->id);
			pthread_cond_wait(&(Z->z_cond1),&(Z->z_mutex));
			Z->pp=1;
		}
		else
		{
			if(Z->g==0)
			{
				Z->v = 0;
			}
			else
			{
				int h,ll=Z->g;
				if(Z->b==0)
				{
					if(Z->g>=8&&a>=8)
					{
						h=8;
					}
					else if(Z->g<=8&&Z->g<=a)
					{
						h=Z->g;
					}
					else
					{
						h=a;
					}
					Z->k = rand()%h+1;
					Z->b = Z->k;
					printf("\033[0;32m");
					printf("Zone with %d created %d slots\n",Z->id,Z->k);
					for(int j=1;j<=Z->k;j++)
					{
						Z->s[j]='C';
						Z->s_id[j]=0;
					}
				}
				int zz=0;
				for(int j=1;j<=o;j++)
				{
					if(Stud[j].v==0)
					{
						for(int l=1;l<=Z->k;l++)
						{
							if(Z->s[l]=='C')
							{
								Stud[j].z_id=Z->id;
								Stud[j].v=1;
								Z->s[l]='A';
								Z->s_id[l]=j;
								Z->g--;
								Z->b--;
								zz++;
								if(Z->g==0)
								{
									Comp[Z->c_id].r--;
									Z->v=0;
									break;
								}
								if(Z->b==0)
								{
									Z->v=2;
									break;
								}
								break;
							}
						}
					}
					if(Z->b==0)
					{
						Z->v=2;
						break;
					}
					if(Z->g==0)
					{
						break;
					}
				}
				for(int q=1;q<=Z->k;q++)
				{
					if(Z->s_id[q]!=0)
					{
						pthread_cond_signal(&(Stud[Z->s_id[q]].s_cond));
						Z->s_id[q]=0;
					}
				}
				Z->h=zz;
				if(Z->g==0)
				{
					Comp[Z->c_id].r--;
					Z->v = 0;
				}
				else if(Z->g==ll)
				{
					Z->v = 1;
				}
				else
				{
					Z->v = 2;
				}
			}
		}
	}
	pthread_mutex_unlock(&(Z->z_mutex));
	return NULL;
}
```
