#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

//난수  17자리수, 5자리수 
int main(int argc, char *argv[]) {
 	unsigned long r=0;
	int cnt = 0;	

	if (argc < 2) {
		printf(" [GEN ERROR] execute error\n");
		printf(" [GEN ERROR] usage. %s WSS key range\n", argv[0]);
		printf(" [GEN ERROR] example. %s 1000000 5\n", argv[0]);
		return 0;
	}

	srand(time(NULL));

	while ( cnt != atoll(argv[1])){
		r =((unsigned long)(rand() * rand()))/1000;
		printf("%lu\n", r);
		cnt++;


	}
	
	return 0;
}
