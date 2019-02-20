#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#define MAX 100

int* arr;

void swap(int i, int j)
{
	int tmp = arr[i];
	arr[i] = arr[j];
	arr[j] = tmp;
}

int main(int argc, char** argv)
{
	int i;
	int a, b, max;
	if (argc < 2) {
		printf("Error: Usage ./trc_gen footprint\n");
		return 0;
	}

	max = atoi(argv[1]);


	// 	
	arr = (int*) malloc (sizeof(int) * max);
	if(!arr){
		printf("Failed to alloc memory\n");
		return -1;
	}
	for(i = 0; i < max; i++)
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
		printf("%d \n", arr[i]);

	return 0;
}
