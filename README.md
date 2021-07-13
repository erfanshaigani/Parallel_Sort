# Parallel_Sort
## Sort an array in parallel with threads!

By deploying pthread library (libpthread.so) we are able to launch multiple threads
and do tasks in parallel. In this code DLP (data level parallelism) has been implemented
and the objective is to sort a giant array of size **N = 2 ^ M  where M can be up to 30!**
So it may take forever if you try to sort it serially.

Now the idea is to break the procedure into 3 stages :

		1) divide the array into 4 segments and sort them in parallel by 4 threads
			(each thread sorts one segment serially with quicksort) but the threads work in parallel
		
            2) now we have 4 sorted segments -> s1,s2,s3,s4
			one thread merges (s1,s2) and another thread merges (s3,s4)
			again the merges are serial, but 2 threads work in parallel
            
            3) now that we have 2 sorted arrays, each of sise N/2, we should do parallel merge 
			in this stage because the size of the arrays is big. so we launch 4 
			threads to do parallel merge of these 2 arrays and form the final sorted array
		
------------------------------------------------------------------------------------------------------------
### Compile and run

**compile : gcc -O2 pth_msort_test.c pth_msort.c -o sort -lpthread -lm**<br/>
**run : ./sort M**
