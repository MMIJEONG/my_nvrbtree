#ifndef _NV_RBTREE_H
#define _NV_RBTREE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
//#include "linked_list.h"

#define _CRT_SECURE_NO_WARNINGS
#define MEM_POOL_SIZE   (1024*2*1024)

#define SIMUL_RB_RED      0
#define SIMUL_RB_BLACK   1
#define SIMUL_INIT_KEY   -1
#define SIMUL_RB_NIL      -2
#define SIMUL_RB_NOT_EXIST   -3
#define SIMUL_RB_EXIST   -4
#define SIMUL_RB_IN_GROUP	-5
#define SWAP_COUNT	50
#define DEBUG_WRITE_COUNT 1

int sfull_mem[MEM_POOL_SIZE];


// rbtree node structure 
typedef struct rb_key{
	 long long int key;
	 struct simul_area *area;
} rb_key;

typedef struct rb_node {
	unsigned int color;
	long long int primary_key;
	struct rb_key *keys; 
	struct rb_node *parent;
	struct rb_node *right;
	struct rb_node *left;
	unsigned int cnt_key;
	unsigned long count;
	unsigned int is_free;
	struct area *area;
} rb_node;

typedef struct rb_root {
	rb_node *root_node;
	rb_node *nil_node;
} rb_root;

// reference func.
rb_node *parent_of(rb_node *node);
rb_node *right_of(rb_node *node);
rb_node *left_of(rb_node *node);

// init func. 
void rb_init_key(rb_key *key_node, long long int key);
void rb_init_node(rb_root *T, rb_node *node, rb_key *key_node);
void rb_init_root(rb_root *root);

// set/check colors
unsigned int is_rb_black(rb_node *node);
unsigned int is_rb_red(rb_node *node);
void set_rb_black(rb_node *node);
void set_rb_red(rb_node *node);

// rb reblanacing functions
rb_root *rb_alloc_root(void);
rb_node *rb_rotate_left(rb_root *T, rb_node *node);
rb_node *rb_rotate_right(rb_root *T, rb_node *node);

unsigned int rb_transplant_node(rb_root *T, rb_node *pre_node, rb_node *chg_node);
rb_node *rb_get_min(rb_node *node);
rb_node *rb_get_successor(rb_root* T, rb_node *node);

// rb access functions 
int rb_insert(rb_root *T, long long int inkey);
void rb_insert_fixup(rb_root *T, rb_node *alt_node);
int rb_delete(rb_root *T, long long int delkey);
void rb_delete_fixup(rb_root *T, rb_node *chg_node);

rb_node *rb_search(rb_root *T, long long int find_key);
rb_key *rb_search_key(rb_root *T, long long int find_key);


// parameters 
extern unsigned long long int cnt_node;
int GROUP_SIZE; 
static unsigned long long int ENABLE_MEM_SIZE = (1024 * 1024 * 1024);	// 8 GB => this means 8 * 1GB / 8;  "/ 8" -> 64bit machine.
extern unsigned long long int write_count;
extern unsigned long long int color_write_count;
extern unsigned long long int read_count;

#endif



















