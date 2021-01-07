#define _POSIX_C_SOURCE 199309L //required for clock
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <limits.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>
#include <inttypes.h>
#include <math.h>
struct thread_data{
	int l;
	int r;
	int *a;
};
int *shareMem(size_t size)
{
    key_t mem_key = IPC_PRIVATE;
    int shm_id = shmget(mem_key, size, IPC_CREAT | 0666);
    return (int*)shmat(shm_id, NULL, 0);
}
void merging(int *a,int l1,int r1,int l2,int r2)
{
	int *d = (int*)malloc(sizeof(int)*(100000000));
	int b1=l1,b2=l2,j=0,k=0;
	if(l1>=l2)
	{
		return;
	}
	while(b1<=r1&&b2<=r2)
	{
		if(a[b1]<=a[b2])
		{
			d[j]=a[b1];
			j++;
			b1++;
		}
		else
		{
			d[j]=a[b2];
			j++;
			b2++;
		}
	}
	while(b1<=r1)
	{
		d[j]=a[b1];
		j++;
		b1++;
	}
	while(b2<=r2)
	{
		d[j]=a[b2];
		j++;
		b2++;
	}
	for(int i=l1;i<=r2;i++)
	{
		a[i]=d[k];
		k++;
	}
}
void c_mergesort(int *a,int l,int r)
{
	if(l<r)
	{
		if(r-l<5)
		{
			int min,temp;
			for(int i=l;i<r;i++)
			{
				min = i;
				for(int j=i+1;j<=r;j++)
				{
					if(a[j]<a[min])
					{
						min =j;
					}
				}
				temp=a[min];
				a[min]=a[i];
				a[i]=temp;
			}
		}
		else
		{
			int mid;
			mid=(l+r)/2;
			int pid1 = fork();
			if(pid1==0)
			{	
				c_mergesort(a,l,mid);
				exit(1);
			}
			else
			{
				int pid2 = fork();
				if(pid2==0)
				{
					c_mergesort(a,mid+1,r);
					exit(1);
				}
				else
				{
					int status;
					waitpid(pid1,&status,0);
					waitpid(pid2,&status,0);
				}
				merging(a,l,mid,mid+1,r);
			}
		}
	}
}
void n_mergesort(int *a,int l,int r)
{
	if(l<r)
	{
		if(r-l<5)
		{
			int min,temp;
			for(int i=l;i<r;i++)
			{
				min = i;
				for(int j=i+1;j<=r;j++)
				{
					if(a[j]<a[min])
					{
						min =j;
					}
				}
				temp=a[min];
				a[min]=a[i];
				a[i]=temp;
			}
		}
		else
		{
			int mid;
			mid=(l+r)/2;	
			n_mergesort(a,l,mid);
			n_mergesort(a,mid+1,r);
			merging(a,l,mid,mid+1,r);
		}
	}
}
void *t_mergesort(void *e)
{
	struct thread_data *x = (struct thread_data*) e;
	int l = x->l;
	int r = x->r;
	int *f = x->a;
	if(l<r)
	{
		if(r-l<5)
		{
			int min,temp;
			for(int i=l;i<r;i++)
			{
				min = i;
				for(int j=i+1;j<=r;j++)
				{
					if(f[j]<f[min])
					{
						min =j;
					}
				}
				temp=f[min];
				f[min]=f[i];
				f[i]=temp;
			}
			return NULL;
		}
		else
		{
			int mid;
			mid=(l+r)/2;
			struct thread_data g;
		    g.l = l;
		    g.r = mid;
		    g.a = f;	
			pthread_t tid1;
			pthread_create(&tid1,NULL,t_mergesort,&g);
			struct thread_data h;
		    h.l = mid+1;
		    h.r = r;
		    h.a = f;	
			pthread_t tid2;
			pthread_create(&tid2,NULL,t_mergesort,&h);
			pthread_join(tid1,NULL);
			pthread_join(tid2,NULL);
			merging(f,l,mid,mid+1,r);
		}
	}
	return NULL;
}

int main()
{
	int n;
	scanf("%d",&n);
	int *a = shareMem(sizeof(int)*(n+1));
	int *b = (int*)malloc(sizeof(int)*(n+1));
	int *c = (int*)malloc(sizeof(int)*(n+1));
	struct timespec ts;
	for(int i=0;i<n;i++)
	{
		scanf("%d",&a[i]);
		b[i]=a[i];
		c[i]=a[i];
	}
	printf("\033[0;31m");
	printf("Running concurrent mergesort for n = %d\n",n);
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    long double st = ts.tv_nsec/(1e9)+ts.tv_sec;
    c_mergesort(a,0,n-1);
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    long double en = ts.tv_nsec/(1e9)+ts.tv_sec;
    long double t1 = en - st ;
    for(int i=0;i<n;i++)
    { 	
          printf("%d ",a[i]);
    }
    printf("\n");
    shmdt(a);
    printf("time taken = %Lf\n",t1);
    printf("\n");
    printf("\033[0;32m");
    printf("Running normal mergesort for n = %d\n",n);
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    st = ts.tv_nsec/(1e9)+ts.tv_sec;
    n_mergesort(b,0,n-1);
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    en = ts.tv_nsec/(1e9)+ts.tv_sec;
    long double t2 = en - st;
    for(int i=0;i<n;i++)
    { 	
          printf("%d ",b[i]);
    }
    printf("\n");
    printf("time taken = %Lf\n",t2);
    printf("\n");
    printf("\033[0;34m");
    printf("Running threaded mergesort for n = %d\n",n);
    pthread_t tid;
    struct thread_data s;
    s.l=0;
    s.r=n-1;
    s.a = c;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    st = ts.tv_nsec/(1e9)+ts.tv_sec;
    pthread_create(&tid,NULL,t_mergesort,&s);
    pthread_join(tid,NULL);
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    en = ts.tv_nsec/(1e9)+ts.tv_sec;
    long double t3 = en - st;
    for(int i=0;i<n;i++)
    { 	
          printf("%d ",c[i]);
    }
    printf("\n");
    printf("time taken = %Lf\n",t3);
    printf("\n");
    printf("\033[0;33m");
    printf("RESULT:-\n");
    printf("Normal mergesort is %Lf times faster than concurrent mergesort\n",t1/t2);
    printf("Normal mergesort is %Lf times faster than threaded mergesort\n",t3/t2);
	return 0;
}
