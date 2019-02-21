#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>

#define MAX 100

unsigned long * arr;

void swap(unsigned long i, unsigned long j)
{
	unsigned long tmp = arr[i];
	arr[i] = arr[j];
	arr[j] = tmp;
}

int main(int argc, char** argv)
{
	unsigned long i;
	unsigned long a, b, max, stride;

	if (argc < 3) {
		printf("Error: Usage ./trc_gen footprint stride\n");
		return 0;
	}

	max = atoi(argv[1]);
	stride = atoi(argv[2]);

	// 	
	arr = (unsigned long*) malloc (sizeof(unsigned long) * max);
	if(!arr){
		printf("Failed to alloc memory\n");
		return -1;
	}
	memset(arr, 0, max);
	//for(i = 0; i < max; i++)
	for(i = 0; i < max; i+=stride)
		arr[i] = i;

	srand((unsigned)time(NULL));

	// permutation 
	for(i = 0; i < max; i++){
		a = rand()%max;
		b = rand()%max;
		swap(a, b);
	}

	// print 
	for (i = 0; i < max; i++)
		if(arr[i]) printf("%lu \n", arr[i]);

	return 0;
}
