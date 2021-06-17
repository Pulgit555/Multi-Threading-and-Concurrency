
# Concurrent Merge Sort
Implemented concurrent, threaded and sequential merge sort with a modification of selection sort for less than 5 elements.

## How to Run ?
+ compile the code using `gcc q1.c -lpthread`
+ and run the code using `./a.out`

### Major Functions Used

1.	**n_mergesort** -> this is used to implement normal merge sort .In this function it breaks the array in two sub-parts recursively and calling merging function to merge the two-parts after sorting it.If size of subpart becomes less than 5 ,selection sort is used.
```
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
``` 

2.	**c_mergesort** -> this is used to implement concurrent merge sort. In this function, maked two child processes recursively using fork() , one for left sub-part and other for right sub-part . In this case also,If size of subpart becomes less than 5 ,selection sort is used. Since they are child processes and we want to make changes in a single array for that we used shareMem function.
```
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
```

3.	**t_mergesort** -> this is used to implement threaded merge sort. In this function, created two threads recursively one for left sub-part and other for right sub_part.In this case also,If size of subpart becomes less than 5 ,selection sort is used. 
```
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
```

4.	**merging** -> this is used to implement merging the two sub-parts in all the above cases.
```
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
```

### Results
#### For n=10
```
time taken for normal mergesort     = 0.000021
time taken for concurrent mergesort = 0.001129
time taken for threaded mergesort   = 0.000637
Normal mergesort is 53.880288 times faster than concurrent mergesort
Normal mergesort is 30.369468 times faster than threaded mergesort	
```
#### For n=100
```
time taken for normal mergesort     = 0.000383
time taken for concurrent mergesort = 0.009967
time taken for threaded mergesort   = 0.008095
Normal mergesort is 26.047318 times faster than concurrent mergesort
Normal mergesort is 21.154636 times faster than threaded mergesort
```
#### For n=1000
```
time taken for normal mergesort     = 0.001077
time taken for concurrent mergesort = 0.034633
time taken for threaded mergesort   = 0.010473
Normal mergesort is 32.169897 times faster than concurrent mergesort
Normal mergesort is 9.728397 times faster than threaded mergesort
```

### Comparison
Concurrent merge sort takes more time than normal merge sort due to the creation of processes which increases context switches and uses shared memory which takes more time in accessing data. 
Threaded merge sort takes more time than normal merge sort due to creation of so many threads which do very minimal work. 

### Conclusion
Normal merge sort is faster than concurrent and threaded merge sort.
