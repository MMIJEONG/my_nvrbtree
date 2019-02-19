
#include "nv_rbtree.h"


int main(int argc, char *argv[])
{
	rb_root *root = NULL;
	rb_node *node = NULL;
	rb_key *ser_key;
	FILE *fp;
	char in_type[20], fn[40];
	int retval = -1;
	time_t t;
	struct timeval start, end;

	//----------- argument parsing ------------//
	unsigned long long int sizeofmem = 0;
	double transmemsize = 0.0;
	int bytelevel = 0;
	char byteunit[5][4] = {"B", "KB", "MB", "GB", "TB"};
	char trc_fname[100];
	int in_count=0, del_count=0, ser_count=0;
	long long int trc_key;

	if (argc < 3) {
		printf("Usage: ./nvrbtree group_size trc_fname\n");
		printf("e.g.: ./nvrbtree 100 ./a.trc\n");
		return 1;
	}

	GROUP_SIZE = atoi(argv[1]); 
	if (GROUP_SIZE < 1) {
		printf("Error: group size should be greater than 0.\n");
		return 1;
	}
	
	//strcpy(trc_fname,"/home/lmj/change_nvrbtree/microbench-v/wss/" );
	//strcat(trc_fname,argv[2]);
	strcpy(trc_fname, argv[2]);
	fp = fopen(trc_fname, "r");
	if (!fp) {
		printf("Failed to open a file %s\n", trc_fname);
		exit(-1);
	}

	printf("Run ... \n");

	//------------ time start ----------------//
	srand((unsigned) time(&t));
	gettimeofday(&start, NULL);

	root = rb_alloc_root();
	rb_init_root(root);
	

	//insert	
	printf("Insert ... \n");
	while(!feof(fp)){
		fscanf(fp,"%lld",&trc_key);
		retval = rb_insert(root, (long long int)trc_key);
		in_count+=1;
	}

	//search
	printf("Search ... \n");
	fseek(fp, 0, SEEK_SET);

	while(!feof(fp)){
		fscanf(fp,"%lld",&trc_key);
		ser_key = rb_search_key(root, (long long int)trc_key);
		ser_count+=1;
	}

	//delete
	printf("Delete ... \n");
	fseek(fp, 0, SEEK_SET);
	while(!feof(fp)){
		fscanf(fp,"%lld",&trc_key);
		retval = rb_delete(root, (long long int)trc_key);
		del_count+=1;
	}
	fclose(fp);
	gettimeofday(&end, NULL);

	sizeofmem = cnt_node * (sizeof(unsigned int) + sizeof(long long int) + (sizeof(struct rb_node *)*3) + sizeof(unsigned int) + (sizeof(long long int)*(GROUP_SIZE)));
	while (sizeofmem / 1024 > 0) {
		transmemsize = (double)sizeofmem / 1024;
		sizeofmem /= 1024;
		bytelevel++;
	}

	printf(" number of node : %llu\n", cnt_node);
	printf(" size of tree memory : %.4lf %s\n", transmemsize, byteunit[bytelevel]);
	printf(" insert : %d, serach : %d, delete : %d\n",in_count, ser_count, del_count);
	printf(" total read count : %llu\n", read_count);
	printf(" total write count : %llu\n", write_count + color_write_count);
	printf(" Elapsed time: %6.2f s\n", end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec) / 1000000.0);
	printf(" Elapese time: %lf us\n",((end.tv_sec - start.tv_sec )*1000000.0) + (end.tv_usec - start.tv_usec));
	return 0;
}
