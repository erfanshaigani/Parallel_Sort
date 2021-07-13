// Include your C header files here

	// we have 3 stages in this algo :
	/*
		1) divide the array into 4 segments and sort them in parallel by 4 threads
			(each thread sorts one segment serially with quicksort) but the threads work in parallel
		2) now we have 4 sorted segments -> s1,s2,s3,s4
			one thread merges (s1,s2) and another thread merges (s3,s4)
			again the merges are serial, but 2 threads work in parallel

		3) now that we have 2 sorted arrays, each of sise N/2, we should do parallel merge 
			in this stage because the size of the arrays is big. so we launch 4 
			threads to do parallel merge of these 2 arrays and form the final sorted array
	*/	

#include "pth_msort.h"
#include "timer.h"
unsigned int N_array;
int* array;
int gp[2][6];
int* c;

void binary_search(int* a, int n, int key, int l, int r, int* ans)
{
	/////////////////////if the key is smaller than the min or bigger than the max of the array////////////////////////////
	if(key < a[0])
	{
		*ans = 0;
		return;
	}
	else if(key > a[n - 1])
	{
		*ans = n - 1;
		return;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/////////////////////////////////RECURSION STOP CONDITIONS BLOCK///////////////////////////////////////////////////////
	if(r == l)
	{
		if(key < a[l])
		{
			*ans = l - 1;
			return;		
		}
		else if(key >= a[l])
		{
			*ans = l;
			return;
		}
		
	}
	
	if((r - l) == 1)
	{
		// we have three regions : (1 before left) ... left (2 between left and right) right ... (3 after right)
		if(key >= a[l] && key < a[r]) // region 2 
		{
			*ans = l;
		}
		else if(key >= a[r]) // region 3
		{
			*ans = r;
		}
		else // region 1
		{
			*ans = l - 1;
		}
		return;	
	}
	
	/////////////////////////////////////////recursion/////////////////////////////////////////////////////////////////////////////////////
	int mid = (l + r) / 2;
	if(key < a[mid]) // search the LEFT half
	{
		binary_search(a, n, key, l,mid - 1, ans);
	}
	else if(key > a[mid]) // search the RIGHT half
	{
		binary_search(a, n, key, mid + 1, r, ans);
	}
	else if(key == a[mid])
	{
		*ans = mid;
	}
}

void merge_unsized_arrays(int* a, int n1, int*b, int n2, int* c) // merge a of size n1,b of size n2 into -> c of size n1 + n2
{
	int n = n1 + n2;
	int p = 0, q = 0;
	int i;
	for(i = 0; i < n ; i++)
	{
		if(p == n1) // if first array reaches it's end, fill the rest of the result array with rest of the 2nd array
		{
			int now = i;
			int j;
			for(j = q; j < n2; j++)
			{
				c[now] = b[j];
				now++;
			}
			break; // and end!
		}
		if(q == n2) // if 2nd array reaches it's end, fill the rest of the result array with rest of the 1nd array
		{
			int now = i;
			int j;
			for(j = p; j < n1; j++)
			{
				c[now] = a[j];
				now++;
			}
			break;
		}
		////////////////////
		if(a[p] <= b[q])
		{
			c[i] = a[p];
			p++;
		}
		else if(a[p] > b[q])
		{
			c[i] = b[q];
			q++;
		}
	}
}


// note that a itself will be sorted too, but I wanted another result array in addition to that
// after this function both a and asorted are sorted and are completely equal
void quicksort(int* a, int n, int* asorted) // sorts array a of size n and asorted is the result
{
	if(n == 0)
	{
		return;		
	}
	if(n == 1)
	{
		asorted[0] = a[0];
		return;
	}
	if(n == 2)
	{
		if(a[0] > a[1])
		{
			asorted[1] = a[0];
			asorted[0] = a[1];
			int temp;
			temp = a[0];
			a[0] = a[1];
			a[1] = temp;
		}
		else
		{
			asorted[1] = a[1];
			asorted[0] = a[0];
		}
		return;
	}
	
	int small = n-1;
	int big = 1;
	int temp = 0;
	int piv = 0;
	while(big < small)
	{
	
		while(a[big] <= a[piv])
		{
			big++;
		}
		while(a[small] > a[piv])
		{
			small--;
		}
		if(big < small)
		{
			temp = a[big];
			a[big] = a[small];
			a[small] = temp;
		}
	}
	asorted[small] = a[piv];
	temp = a[small];
	a[small] = a[piv];
	a[piv] = temp;
	
	piv = small;
	//printf("piv : %d\n",piv);
	//printf("%d\n",(a + piv + 1)[0]);
	quicksort(a, piv, asorted);
	quicksort(a + piv + 1, n - 1 - piv, asorted + piv + 1);
}



void quicksortt(int* a, int n) // sorts array a of size n
{
	if(n == 1 || n == 0)
	{
		return;		
	}
	if(n == 2)
	{
		if(a[0] > a[1])
		{
			int temp;
			temp = a[0];
			a[0] = a[1];
			a[1] = temp;
		}
		return;
	}
	
	int small = n-1;
	int big = 1;
	int temp = 0;
	int piv = 0;
	while(big < small)
	{
	
		while(a[big] <= a[piv])
		{
			big++;
		}
		while(a[small] > a[piv])
		{
			small--;
		}
		if(big < small)
		{
			temp = a[big];
			a[big] = a[small];
			a[small] = temp;
		}
	}
	temp = a[small];
	a[small] = a[piv];
	a[piv] = temp;
	
	piv = small;
	//printf("piv : %d\n",piv);
	//printf("%d\n",(a + piv + 1)[0]);
	quicksortt(a, piv);
	quicksortt(a + piv + 1, n - 1 - piv);
}

void* thread_sort(void* rank) 
{
   	long r = (long) rank; // like thread Index
	quicksort(array + r * (N_array / 4), N_array / 4, c + r * (N_array / 4));
  	return NULL;
}


void* thread_simple_merge(void* rank) 
{
   	long r = (long) rank; // like thread Index
	merge_unsized_arrays(c + (2 * r) * (N_array / 4), N_array / 4, c + ((2 * r) + 1) * (N_array / 4), N_array / 4, array + 2 * r * (N_array / 4));
  	return NULL;
}

void* thread_merge(void* rank) 
{
   	long r = (long) rank; // like thread Index
	if(r == 0)
	{
		/////////first part///////////////////////
		int n1 = gp[0][r + 1] - gp[0][r] + 1;
		int n2 = gp[1][r + 1] - gp[1][r] + 1;
		merge_unsized_arrays(array, n1, array + (N_array / 2), n2, c);
		r = 4;
		//////////last part///////////////////////
	}
   	int n1 = gp[0][r + 1] - gp[0][r];
	int n2 = gp[1][r + 1] - gp[1][r];
	merge_unsized_arrays(array + gp[0][r] + 1, n1, array + (N_array / 2) + gp[1][r] + 1, n2, c + gp[0][r] + 1 + gp[1][r] + 1);
  	return NULL;
}

void mergeSortParallel (const int* values, unsigned int N, int* sorted)
{
    N_array = N;
	array = values;
	c = sorted;
	int ans;
	int T = 4;
	pthread_t* handles = (pthread_t*) malloc (T*sizeof(pthread_t));
	int j;
	/////////////////////////////sort 4 segments of size N / 4 by 4 threads///////////////////////////
	double ts1,ts2;
	GET_TIME(ts1);
	for (j=0; j<T; j++)  
	{
      	pthread_create(&handles[j], NULL, thread_sort, (void*)j);  
	}

	for (j=0; j<T; j++) 
   	{
      	pthread_join(handles[j], NULL); 
   	}
	GET_TIME(ts2);
	///////////merge these 4 sorted segments 2 by 2///////////
	double tm1,tm2;
	GET_TIME(tm1);
	for (j=0; j<2; j++)  
	{
      	pthread_create(&handles[j], NULL, thread_simple_merge, (void*)j);  
	}

	for (j=0; j<2; j++) 
   	{
      	pthread_join(handles[j], NULL); 
	}
	GET_TIME(tm2);
	////////parallely merge 2 sorted arrays of size N / 2 by 4 threads/////////////
	int samples[3] = {0, ((N_array / 4) - 1), (N_array / 2) - 1};
	int i;
	for(i = 0; i < 3; i++)
	{
		binary_search(array + (N_array / 2), (N_array / 2), array[samples[i]], 0, (N_array / 2) - 1, &ans);
		gp[0][2 * i] = samples[i];
		gp[1][2 * i] = ans;
		binary_search(array, (N_array / 2), (array + (N_array / 2))[samples[i]], 0, (N_array / 2) - 1, &ans);
		gp[0][2 * i + 1] = ans;
		gp[1][2 * i + 1] = samples[i];
	} 

	quicksortt(gp[0],6); // sort the first row
	quicksortt(gp[1],6); // sort the second row

	double tm3,tm4;
	GET_TIME(tm3);
	for (j=0; j<T; j++)  
	{
      	pthread_create(&handles[j], NULL, thread_merge, (void*)j);  
	}

	for (j=0; j<T; j++) 
   	{
      	pthread_join(handles[j], NULL); 
   	}
	GET_TIME(tm4);

	//memcpy(c, array, N_array * sizeof(int));

	// you can uncomment this line to see the duration of each stage!
	// we have 3 stages in this algo :
	/*
		1) divide the array into 4 segments and sort them in parallel by 4 threads
			(each thread sorts one segment serially with quicksort) but the threads work in parallel
		2) now we have 4 sorted segments -> s1,s2,s3,s4
			one thread merges (s1,s2) and another thread merges (s3,s4)
			again the merges are serial, but 2 threads work in parallel

		3) now that we have 2 sorted arrays, each of sise N/2, we should do parallel merge 
			in this stage because the size of the arrays is big. so we launch 4 
			threads to do parallel merge of these 2 arrays and form the final sorted array
	*/	
	// you can uncomment this line to see the duration of each stage!			 
	//printf("first sort : %g  middle merge : %g  last merge : %g  \n",(ts2-ts1),(tm2-tm1),(tm4-tm3));
}