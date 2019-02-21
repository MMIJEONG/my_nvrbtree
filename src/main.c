
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
	struct timeval start, end, in_start, in_end, ser_start, ser_end, del_start, del_end;
	long long int in_time=0, ser_time=0, del_time=0;
	//----------- argument parsing ------------//
	unsigned long long int sizeofmem = 0;
	double transmemsize = 0.0;
	int bytelevel = 0;
	char byteunit[5][4] = {"B", "KB", "MB", "GB", "TB"};
	char trc_fname[100];
	int in_count=0, del_count=0, ser_count=0;
	long long int trc_key;
	char *ptr=NULL;	
	long long int ivalue[3];
	char com[10]={0,};	
	
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
		gettimeofday(&in_start, NULL);
		retval = rb_insert(root, (long long int)trc_key);
		gettimeofday(&in_end, NULL);
		in_time += (((in_end.tv_sec - in_start.tv_sec )*1000000) + (in_end.tv_usec - in_start.tv_usec));
		in_count+=1;
	}
 	//search
 	printf("Search ... \n");
	fseek(fp, 0, SEEK_SET);
	while(!feof(fp)){	
		 fscanf(fp,"%lld",&trc_key);
		 gettimeofday(&ser_start, NULL);
 		 ser_key = rb_search_key(root, (long long int)trc_key);
 		 gettimeofday(&ser_end, NULL);
		 ser_time += (((ser_end.tv_sec - ser_start.tv_sec )*1000000) + (ser_end.tv_usec - ser_start.tv_usec));
 		 ser_count+=1;
 	}
	//delete
 	printf("Delete ... \n");
	fseek(fp, 0, SEEK_SET);
	while(!feof(fp)){
		fscanf(fp,"%lld",&trc_key);
		gettimeofday(&del_start, NULL);
		retval = rb_delete(root, (long long int)trc_key);
		gettimeofday(&del_end, NULL);
		del_time += (((del_end.tv_sec - del_start.tv_sec )*1000000) + (del_end.tv_usec - del_start.tv_usec));
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
	printf(" insert time: %lld us\n",in_time);
	printf(" search time: %lld us\n",ser_time);
	printf(" delete time: %lld us\n",del_time);
  	printf(" total elapsed time: %lld us\n",in_time + ser_time + del_time);

	//printf(" Elapsed time: %6.2f s\n", end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec) / 1000000.0);
	//printf(" Elapese time: %lf us\n",((end.tv_sec - start.tv_sec )*1000000.0) + (end.tv_usec - start.tv_usec));
	return 0;
}
