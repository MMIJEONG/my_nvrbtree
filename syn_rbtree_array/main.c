
#include "nv_rbtree.h"


int main(int argc, char *argv[])
{
	rb_root *root = NULL;
	rb_node *node = NULL;
	long long int ser_key;
	FILE *in_fp = NULL, *ser_fp = NULL, *del_fp = NULL;
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
		printf("e.g.: ./nvrbtree 100 ../trace/rvalue5\n");
		return 1;
	}

	GROUP_SIZE = atoi(argv[1]); 
	if (GROUP_SIZE < 1) {
		printf("Error: group size should be greater than 0.\n");
		return 1;
	}

	//------------ time start ----------------//
	srand((unsigned) time(&t));
	gettimeofday(&start, NULL);

	root = rb_alloc_root();

	rb_init_root(root);

	
	printf("--run syntheic mode --\n");
	
	
	strcpy(trc_fname,"/home/lmj/change_nvrbtree/microbench-v/wss/" );
	strcat(trc_fname,argv[2]);
	
	//insert	
	in_fp = fopen(trc_fname, "r");
	while(!feof(in_fp)){
		fscanf(in_fp,"%lld",&trc_key);
		retval = rb_insert(root, (long long int)trc_key);
		in_count+=1;
	}
	fclose(in_fp);
	//search
	ser_fp = fopen(trc_fname,"r");
	while(!feof(ser_fp)){
		fscanf(ser_fp,"%lld",&trc_key);
		ser_key = rb_search_key(root, (long long int)trc_key);
		ser_count+=1;
	}
	fclose(ser_fp);
	//delete
	del_fp = fopen(trc_fname,"r");
	while(!feof(del_fp)){
		fscanf(del_fp,"%lld",&trc_key);
		retval = rb_delete(root, (long long int)trc_key);
		del_count+=1;
	}
	fclose(del_fp);

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

	return 0;
}
