#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include "rbtree_group.h"
#include "buddy_allocator.h"
#include "linked_list.h"
struct simul_swap_node swap_head;
unsigned long long int swap_count = 0;
unsigned long long int cnt_node = 0;
unsigned long long int cnt_key = 0;
int GROUP_SIZE = 0; 
int WSS = 0;
unsigned long long int ENABLE_MEM_SIZE = (1024 * 1024 * 1024);	// 8 GB => this means 8 * 1GB / 8;  "/ 8" -> 64bit machine.
unsigned long long int write_count = 0;
unsigned long long int remain_count = 0;
unsigned long long int color_write_count = 0;
unsigned long long int read_count = 0;
unsigned long long int point_count = 0;
simul_rb_node *parent_of(simul_rb_node *node) {
	return node->rb_parent;
}

void simul_free(simul_rb_node *node) {
	free(node);
}

simul_rb_node *right_of(simul_rb_node *node) {
	return node->rb_right;
}

simul_rb_node *left_of(simul_rb_node *node) {
	return node->rb_left;
}

void init_swap_node() {
	swap_head.node = NULL;
	swap_head.prev = &swap_head;
	swap_head.next = &swap_head;
}

void init_rb_key(simul_rb_key *key_node, long long int key) {
	key_node->rb_key = key;
	init_list_head(&(key_node->list));
}

void init_rb_node(simul_rb_root *T, simul_rb_node *node, simul_rb_key *key_node) {
	set_rb_red(node);
	node->std_key = key_node->rb_key / (GROUP_SIZE);
	init_list_head(&(node->rb_keys));
	list_add(&(key_node->list), &(node->rb_keys));
	point_count+=4;
	node->cnt_key = 1;
	node->rb_parent = T->nil_node;
	node->rb_right = T->nil_node;
	node->rb_left = T->nil_node;
	node->count = 1;
	node->is_free = 0;
}

void init_rb_root(simul_rb_root *root) {
	root->nil_node = (simul_rb_node *)malloc(sizeof(simul_rb_node));
	root->root_node = (simul_rb_node *)malloc(sizeof(simul_rb_node));

	set_rb_black(root->nil_node);
	root->nil_node->std_key = SIMUL_RB_NIL;
	init_list_head(&(root->nil_node->rb_keys));
	root->nil_node->cnt_key = 0;
	root->nil_node->rb_parent = root->nil_node;
	root->nil_node->rb_right = root->nil_node;
	root->nil_node->rb_left = root->nil_node;
	root->nil_node->count = 1;
	root->nil_node->is_free = 0;
	set_rb_black(root->root_node);
	root->root_node->std_key = SIMUL_RB_NIL;
	init_list_head(&(root->root_node->rb_keys));
	root->root_node->cnt_key = 0;
	root->root_node->rb_parent = root->nil_node;
	root->root_node->rb_left = root->nil_node;
	root->root_node->rb_right = root->nil_node;
	root->root_node->count = 1;
	root->root_node->is_free = 0;
}

unsigned int is_rb_black(simul_rb_node *node) {
	return node->rb_color == SIMUL_RB_BLACK ? 1 : 0;
}

unsigned int is_rb_red(simul_rb_node *node) {
	return node->rb_color == SIMUL_RB_RED ? 1 : 0;
}

void set_rb_black(simul_rb_node *node) {
	node->rb_color = SIMUL_RB_BLACK;
	color_write_count++;
}

void set_rb_red(simul_rb_node *node) {
	node->rb_color = SIMUL_RB_RED;
	color_write_count++;
}

simul_rb_root *simul_rb_alloc_root(void) {
	simul_rb_root *ret = (simul_rb_root *)malloc(sizeof(simul_rb_root));
	return ret;
}

simul_rb_node *simul_rb_left_rotate(simul_rb_root *T, simul_rb_node *node) {
	simul_rb_node *ret = NULL;
	simul_rb_node *right_node = right_of(node);

	/* exchange left child of node_right to right child of node */
	node->rb_right = left_of(right_node);
	node->count++;   // for analysis
	write_count++;


	if (left_of(right_node) != T->nil_node) {
		left_of(right_node)->rb_parent = node;
		left_of(right_node)->count++;   // for analysis
		write_count++;

	}

	/* change parent connection */
	right_node->rb_parent = parent_of(node);
	right_node->count++;   // for analysis
 	write_count++;


	if (node == parent_of(node)->rb_left) {   /* case: node is left child of parent */
		parent_of(node)->rb_left = right_node;
		parent_of(node)->count++;   // for analysis
 		write_count++;

	}
	else if (node == parent_of(node)->rb_right) {   /* case: node is right child of parent */
		parent_of(node)->rb_right = right_node;
		parent_of(node)->count++;   // for analysis
 		write_count++;

	}

	right_node->rb_left = node;
	right_node->count++;   // for analysis
	write_count++;
	
	node->rb_parent = right_node;
	node->count++;   // for analysis
	write_count++;


	ret = node;

	return ret;
}

simul_rb_node *simul_rb_right_rotate(simul_rb_root *T, simul_rb_node *node) {
	simul_rb_node *ret = NULL;
	simul_rb_node *left_node = left_of(node);

	/* exchange right child of node_left to left child of node */
	node->rb_left = right_of(left_node);
	node->count++;   // for analysis
	write_count++;

	if (right_of(left_node) != T->nil_node) {
		right_of(left_node)->rb_parent = node;
		right_of(left_node)->count++;   // for analysis
	}

	/* change parent connection */
	left_node->rb_parent = parent_of(node);
	left_node->count++;   // for analysis
	write_count++;

	if (node == parent_of(node)->rb_left) {   /* case: node is left child of parent */
		parent_of(node)->rb_left = left_node;
		parent_of(node)->count++;   // for analysis
		write_count++;
	}
	else if (node == parent_of(node)->rb_right) {   /* case: node is right child of parent */
		parent_of(node)->rb_right = left_node;
		parent_of(node)->count++;   // for analysis
		write_count++;
	}

	left_node->rb_right = node;
	left_node->count++;   // for analysis
	write_count++;
	node->rb_parent = left_node;
	node->count++;   // for analysis
	write_count++;
	ret = node;

	return ret;
}

/* change parent of chg_node from parent of pre_node */
unsigned int simul_rb_transplant(simul_rb_root *T, simul_rb_node *pre_node, simul_rb_node *chg_node) {
	unsigned int ret = 0;

	if (parent_of(pre_node) == T->nil_node)   /* case: node is root node */
		T->root_node = chg_node;
	else if (pre_node == parent_of(pre_node)->rb_left)   /* case: node is left child of parent */
		parent_of(pre_node)->rb_left = chg_node;
	else if (pre_node == parent_of(pre_node)->rb_right)   /* case: node is right child of parent */
		parent_of(pre_node)->rb_right = chg_node;

	chg_node->rb_parent = parent_of(pre_node);   /* change chg_node's parent */

	return ret;
}

simul_rb_node *simul_rb_minimum(simul_rb_node *node) {
	simul_rb_node *min_node = node;
	while (left_of(min_node)->std_key != SIMUL_RB_NIL)
		min_node = left_of(min_node);

	return min_node;
}

simul_rb_node *simul_rb_successor(simul_rb_root* T, simul_rb_node *node) {
	simul_rb_node *y;
	simul_rb_node *nil = T->nil_node;
	simul_rb_node *root = T->root_node;

	if (nil != (y = node->rb_right)) {
		while (y->rb_left != nil) {
			y = y->rb_left;
		}
		return (y);
	}
	else {
		y = node->rb_parent;
		while (node == y->rb_right) {
			node = y;
			y = y->rb_parent;
		}
		if (y == root) return (nil);
		return (y);
	}
}


//void simul_rb_swapping(simul_rb_root *T) {
/*
	simul_rb_node *new_node = NULL;
	struct simul_swap_node *cur_swap_node = swap_head.next, *del_swap_node = NULL;

	while (cur_swap_node != &swap_head) {
		if (cur_swap_node->node->is_free == 0) {
			swap_count++;
			new_node = (simul_rb_node *)malloc(sizeof(simul_rb_node));
			new_node->rb_color = cur_swap_node->node->rb_color;
			new_node->std_key = cur_swap_node->node->std_key;
			new_node->rb_keys.next = cur_swap_node->node->rb_keys.next;
			new_node->rb_keys.next->prev = &(new_node->rb_keys);
			new_node->rb_keys.prev = cur_swap_node->node->rb_keys.prev;
			new_node->rb_keys.prev->next = &(new_node->rb_keys);
			new_node->rb_parent = cur_swap_node->node->rb_parent;
			new_node->rb_left = cur_swap_node->node->rb_left;
			new_node->rb_right = cur_swap_node->node->rb_right;
//			new_node->area = (struct simul_area *)malloc(sizeof(struct simul_area));
*/
/*
			new_node->area = (struct simul_area *)smalloc(sizeof(struct simul_area));
			(*(new_node->area->addr))++;
*/
/*
			new_node->is_free = 0;
			new_node->count = 1;
			
			// parent pointer 
			if (cur_swap_node->node == T->root_node)
				T->root_node = new_node;
			else if (cur_swap_node->node->rb_parent == T->root_node) 	// old_node is root
				T->root_node->rb_left = new_node;
			else if (cur_swap_node->node->rb_parent->rb_left == cur_swap_node->node) 	// old_node is parent's left child
				cur_swap_node->node->rb_parent->rb_left = new_node;
			else if (cur_swap_node->node->rb_parent->rb_right == cur_swap_node->node)	// old_node is parent's right child
				cur_swap_node->node->rb_parent->rb_right = new_node;
			cur_swap_node->node->rb_parent = NULL;	// old_node's parent pointer is clear.

													//children pointer 
			if (cur_swap_node->node->rb_left != T->nil_node)	// old_node's left is exist
				cur_swap_node->node->rb_left->rb_parent = new_node;
			cur_swap_node->node->rb_left = NULL;	// old_node's left pointer is clear.
			if (cur_swap_node->node->rb_right != T->nil_node)	// old_node's right is exist
				cur_swap_node->node->rb_right->rb_parent = new_node;
			cur_swap_node->node->rb_right = NULL;	// old_node's left pointer is clear.
		}
		del_swap_node = cur_swap_node;
		cur_swap_node = cur_swap_node->next;
		del_swap_node->next->prev = del_swap_node->prev;
		del_swap_node->prev->next = del_swap_node->next;
		del_swap_node->node->rb_keys.next = NULL;
		del_swap_node->node->rb_keys.prev = NULL;
		
		//sfree(del_swap_node->node->area);
//		free(del_swap_node->node->area);
		free(del_swap_node->node);
		del_swap_node->next = NULL;
		del_swap_node->prev = NULL;
		del_swap_node->node = NULL;
		free(del_swap_node);
	}
*/
//}

void add_swap_list(simul_rb_node *node) {
	struct simul_swap_node *new_swap = NULL, *cur_node = swap_head.next;
	if (node->is_free == 1 || node->count >= SWAP_COUNT) {
		while (cur_node != &swap_head) {
			if (node->std_key == cur_node->node->std_key)
				return;
			cur_node = cur_node->next;
		}
		new_swap = (struct simul_swap_node *)malloc(sizeof(struct simul_swap_node));
		new_swap->node = node;
		new_swap->prev = &swap_head;
		new_swap->next = swap_head.next;
		new_swap->next->prev = new_swap;
		new_swap->prev->next = new_swap;
	}
}

int simul_rb_insert(simul_rb_root *T, long long int inkey) {
	int ret = 0, is_in_group = 0;
	simul_rb_node *comp_node = T->root_node->rb_left;
	simul_rb_node *in_parent_pos = T->root_node;
	simul_rb_node *new_node = (simul_rb_node *)malloc(sizeof(simul_rb_node));
	long long int searching_inkey = inkey / (GROUP_SIZE);
	simul_rb_key *new_key = NULL;

	while (comp_node != T->nil_node) {
		in_parent_pos = comp_node;
		if (searching_inkey < comp_node->std_key){
			comp_node = left_of(comp_node);
			read_count++;
		}
		else if (searching_inkey > comp_node->std_key){
			comp_node = right_of(comp_node);
			read_count++;
		}
		else if (searching_inkey == comp_node->std_key) {
			is_in_group = 1;
			break;
		}
	}

//	if (is_in_group == 1 && comp_node->rb_keys[inkey % (GROUP_SIZE)].rb_key != 0)
//		return SIMUL_RB_EXIST;
	
	new_key = (simul_rb_key *)malloc(sizeof(simul_rb_key));
	init_rb_key(new_key, inkey);
	cnt_key++;

	if (is_in_group == 1) {
		list_add(&(new_key->list), &(comp_node->rb_keys));	// connect list : second -> first -> second->next
		point_count+=4;
	//	comp_node->rb_keys[new_key->rb_key % GROUP_SIZE].area = new_key->area;
		comp_node->cnt_key++;
		return SIMUL_RB_IN_GROUP;
	}
	
	// init node
	init_rb_node(T, new_node, new_key);
	cnt_node++;

	new_node->rb_parent = in_parent_pos;

	if (in_parent_pos == T->root_node) {   /* the first insert (rbtree is empty) */
		T->root_node->rb_left = new_node;
		T->root_node->count++;   // for analysis
		write_count++;
	}
	else if (new_node->std_key < in_parent_pos->std_key) {
		in_parent_pos->rb_left = new_node;
		in_parent_pos->count++;   // for analysis
		write_count++;
	}
	else {
		in_parent_pos->rb_right = new_node;
		in_parent_pos->count++;   // for analysis
		write_count++;
	}

	simul_rb_insert_fixup(T, new_node);

	return ret;
}

void simul_rb_insert_fixup(simul_rb_root *T, simul_rb_node *alt_node) {
	simul_rb_node *across_parent = NULL;

	while (is_rb_red(parent_of(alt_node))) {
		if (parent_of(alt_node) == parent_of(alt_node)->rb_parent->rb_left) {

			across_parent = parent_of(alt_node)->rb_parent->rb_right;

			if (is_rb_red(across_parent)) {   /* case 1: color of across is red */
				set_rb_black(parent_of(alt_node));
				set_rb_black(across_parent);
				set_rb_red(parent_of(alt_node->rb_parent));
				alt_node = parent_of(alt_node->rb_parent);
			}
			else {
				if (alt_node == parent_of(alt_node)->rb_right) {   /* case 2: color of across is black && alt_node is right child */
					alt_node = parent_of(alt_node);
					alt_node = simul_rb_left_rotate(T, alt_node);
				}
				/* case 3: color of across is black && alt_node is left child */
				set_rb_black(parent_of(alt_node));
				set_rb_red(parent_of(alt_node->rb_parent));
				simul_rb_right_rotate(T, parent_of(alt_node->rb_parent));
			}
		}
		else if (parent_of(alt_node) == parent_of(alt_node)->rb_parent->rb_right) {

			across_parent = parent_of(alt_node)->rb_parent->rb_left;

			if (is_rb_red(across_parent)) {   /* case 1: color of across is red */
				set_rb_black(parent_of(alt_node));
				set_rb_black(across_parent);
				set_rb_red(parent_of(alt_node->rb_parent));
				alt_node = parent_of(alt_node->rb_parent);
			}
			else {
				if (alt_node == parent_of(alt_node)->rb_left) {   /* case 2: color of across is black && alt_node is right child */
					alt_node = parent_of(alt_node);
					alt_node = simul_rb_right_rotate(T, alt_node);
				}
				/* case 3: color of across is black && alt_node is left child */
				set_rb_black(parent_of(alt_node));
				set_rb_red(parent_of(alt_node->rb_parent));
				simul_rb_left_rotate(T, parent_of(alt_node->rb_parent));
			}
		}
	}

	set_rb_black(T->root_node->rb_left); /* rule 2: color of root node is black */
}

int simul_rb_delete(simul_rb_root *T, long long int delkey) {
	int ret = 0, is_in_group = 0;
	simul_rb_node *del_node = simul_rb_search(T, delkey);
	simul_rb_node *y;
	simul_rb_node *x;
	simul_rb_key *pre_cur_key = NULL, *cur_key = NULL;
	struct simul_list_head *cur_list = NULL;

	if (del_node == T->nil_node)
		return SIMUL_RB_NOT_EXIST;

	pre_cur_key = &(del_node->rb_keys);
	cur_key = container_of(del_node->rb_keys.next, simul_rb_key, list);
	cur_list = del_node->rb_keys.next;

	while (cur_list != &del_node->rb_keys) {
		if (cur_key->rb_key == delkey) {
			is_in_group = 1;
			break;
		}
		pre_cur_key = cur_key;
		cur_key = container_of(cur_key->list.next, simul_rb_key, list);
		cur_list = cur_list->next;
		read_count++;
	}

	if (is_in_group == 0)
		return SIMUL_RB_NOT_EXIST;

	list_del(&(cur_key->list));
	free(cur_key);
	del_node->cnt_key--;
	if (del_node->cnt_key > 0) 
		return SIMUL_RB_IN_GROUP;
	
	y = ((del_node->rb_left == T->nil_node) || (del_node->rb_right == T->nil_node)) ? del_node : simul_rb_successor(T, del_node);
	x = (y->rb_left == T->nil_node) ? y->rb_right : y->rb_left;

	x->count++;   // for analysis
	write_count++;
	if (T->root_node == (x->rb_parent = y->rb_parent)) {/* assignment of y->p to x->p is intentional */
		T->root_node->rb_left = x;
		T->root_node->count++;   // for analysis
		write_count++;
	}
	else {
		if (y == y->rb_parent->rb_left) {
			y->rb_parent->rb_left = x;
			y->rb_parent->count++;   // for analysis
			write_count++;
		}
		else {
			y->rb_parent->rb_right = x;
			y->rb_parent->count++;   // for analysis
			write_count++;
		}
	}

	if (y != del_node) {
		if (is_rb_black(y))
			simul_rb_delete_fixup(T, x);

		y->rb_left = del_node->rb_left;
		y->count++;   // for analysis
		write_count++;
		y->rb_right = del_node->rb_right;
		y->count++;   // for analysis
		write_count++;
		y->rb_parent = del_node->rb_parent;
		y->count++;   // for analysis
		write_count++;
		y->rb_color = del_node->rb_color;
		del_node->rb_left->rb_parent = del_node->rb_right->rb_parent = y;
		del_node->rb_left->count++;      // for analysis
		write_count++;
		del_node->rb_right->count++;   // for analysis
		write_count++;

		if (del_node == del_node->rb_parent->rb_left) {
			del_node->rb_parent->rb_left = y;
			del_node->rb_parent->count++;   // for analysis
			write_count++;
		}
		else {
			del_node->rb_parent->rb_right = y;
			del_node->rb_parent->count++;   // for analysis
			write_count++;
		}

		free(del_node);

	}
	else {
		if (is_rb_black(y))
			simul_rb_delete_fixup(T, x);

		free(y);
	}
	T->nil_node->rb_parent = T->nil_node;
	T->nil_node->count++;   // for analysis
	write_count++;
	T->nil_node->rb_left = T->nil_node;
	T->nil_node->count++;   // for analysis
	write_count++;
	T->nil_node->rb_right = T->nil_node;
	T->nil_node->count++;   // for analysis
	write_count++;
	return ret;
}

void simul_rb_delete_fixup(simul_rb_root *T, simul_rb_node *chg_node) {
	simul_rb_node *bro_node = NULL;
	while (chg_node != T->root_node && is_rb_black(chg_node)) {

		if (chg_node == parent_of(chg_node)->rb_left) {
			bro_node = parent_of(chg_node)->rb_right;

			if (is_rb_red(bro_node)) { /* case 1: color of bro node is red */
				set_rb_black(bro_node);
				set_rb_red(parent_of(chg_node));
				simul_rb_left_rotate(T, parent_of(chg_node));
				bro_node = parent_of(chg_node)->rb_right;
			}

			if (is_rb_black(left_of(bro_node)) && is_rb_black(right_of(bro_node))) {   /* case 2: color of bro node is black && both of bro's children are black */
				set_rb_red(bro_node);
				chg_node = parent_of(chg_node);

			}
			else {
				if (is_rb_black(right_of(bro_node))) {   /* case 3: bro is black && left child is red && right child is black */
					set_rb_black(left_of(bro_node));
					set_rb_red(bro_node);
					simul_rb_right_rotate(T, bro_node);
					bro_node = parent_of(chg_node)->rb_right;
				}
				bro_node->rb_color = parent_of(chg_node)->rb_color;
				set_rb_black(parent_of(chg_node));
				set_rb_black(right_of(bro_node));
				simul_rb_left_rotate(T, parent_of(chg_node));
				chg_node = T->root_node;
			}
		}
		else {
			bro_node = parent_of(chg_node)->rb_left;

			if (is_rb_red(bro_node)) { /* case 1: color of bro node is red */
				set_rb_black(bro_node);
				set_rb_red(parent_of(chg_node));
				simul_rb_right_rotate(T, parent_of(chg_node));
				bro_node = parent_of(chg_node)->rb_left;
			}

			if (is_rb_black(left_of(bro_node)) && is_rb_black(right_of(bro_node))) {   /* case 2: color of bro node is black && both of bro's children are black */
				set_rb_red(bro_node);
				chg_node = parent_of(chg_node);

			}
			else {
				if (is_rb_black(left_of(bro_node))) {   /* case 3: bro is black && right child is red && left child is black */
					set_rb_black(right_of(bro_node));
					set_rb_red(bro_node);
					simul_rb_left_rotate(T, bro_node);
					bro_node = parent_of(chg_node)->rb_left;
				}
				bro_node->rb_color = parent_of(chg_node)->rb_color;
				set_rb_black(parent_of(chg_node));
				set_rb_black(left_of(bro_node));
				simul_rb_right_rotate(T, parent_of(chg_node));
				chg_node = T->root_node;
			}
		}
	} //while
	set_rb_black(chg_node);
}

simul_rb_node *simul_rb_search(simul_rb_root *T, long long int find_key) {
	simul_rb_node *search_node = T->root_node->rb_left;
	long long int find_std_key = find_key / (GROUP_SIZE);

	while (search_node != T->nil_node) {
		if (find_std_key == search_node->std_key)
			break;

		if (find_std_key < search_node->std_key){
			search_node = left_of(search_node);
			read_count++;
		}
		else{
			search_node = right_of(search_node);
			read_count++;
		}
	}

	return search_node;
}


simul_rb_key *simul_rb_search_key(simul_rb_root *T, long long int find_key) {
	simul_rb_node *search_node = simul_rb_search(T, find_key);
	simul_rb_key *pre_cur_key = NULL, *cur_key = NULL;
	struct simul_list_head *cur_list = NULL;

	if (search_node == T->nil_node)
		return NULL;

	pre_cur_key = &(search_node->rb_keys);
	cur_key = container_of(search_node->rb_keys.next, simul_rb_key, list);
	cur_list = search_node->rb_keys.next;

	while (cur_list != &search_node->rb_keys) {
		if (cur_key->rb_key == find_key) {
			break;
		}
		pre_cur_key = cur_key;
		cur_key = container_of(cur_key->list.next, simul_rb_key, list);
		cur_list = cur_list->next;
		read_count++;
	}

	return cur_key;
} 

void output_sum(char *fn) {
	char output_fn[60];
	FILE *fp = NULL;
	int i = 0;
	unsigned long long int sum = 0;

	strcpy(output_fn, fn);
	strcat(output_fn, ".orig_group_rb.sum.txt");

	fp = fopen(output_fn, "a+t");

	for (i = 0; i < MEM_POOL_SIZE; i++)
		sum += sfull_mem[i];
	fprintf(fp, "%llu\n", sum);

	fclose(fp);
}

void output_mem(char *fn, int line) {
	char output_fn[60];
	FILE *fp = NULL;
	int i = 0;

	sprintf(output_fn, "%s.orig_group_rb.mem.%d.txt", fn, line);

	fp = fopen(output_fn, "w+t");

	for (i = 0; i < MEM_POOL_SIZE; i++) {
		if (sfull_mem[i] == 0)
			continue;
		fprintf(fp, "%d\n", sfull_mem[i]);
	}
	fclose(fp);
}

void output_var(char *fn) {
	char output_fn[60];
	FILE *fp = NULL;
	int i = 0;
	unsigned long long int sum = 0,cnt = 0;
	double mean, var;

	strcpy(output_fn, fn);
	strcat(output_fn, ".orig_group_rb.var.txt");

	fp = fopen(output_fn, "a+t");

	for (i = 0; i < MEM_POOL_SIZE; i++) {
		if (sfull_mem[i] == 0)
			continue;
		sum += sfull_mem[i];
		cnt++;
	}

	mean = sum / cnt;

	sum = 0;
	cnt = 0;
	for (i = 0; i < MEM_POOL_SIZE; i++) {
		if (sfull_mem[i] == 0)
			continue;
		sum += ((sfull_mem[i] - mean)*(sfull_mem[i] - mean));
		cnt++;
	}
	var = sum / cnt;

	fprintf(fp, "%.0lf %.0lf %.0lf\n", mean, var, sqrt(var));

	fclose(fp);
}

void output_swap(char *fn) {
	char output_fn[60];
	FILE *fp = NULL;
	int i = 0;
	unsigned long long int sum = 0;

	strcpy(output_fn, fn);
	strcat(output_fn, ".orig_group_rb.swap.txt");

	fp = fopen(output_fn, "a+t");

	fprintf(fp, "%llu\n", swap_count);

	fclose(fp);
}

int main(int argc, char *argv[])
{
	simul_rb_root *root = NULL;
	simul_rb_node *node = NULL;
	simul_rb_key *ser_key = NULL;
	FILE *fp = NULL;
	long long int in_key = -1, i = 0, *rvalue;
	char in_type[20], fn[40];
	int retval = -1, endpoint = 0, line = 1000;
	time_t t;
	struct timeval start, end;

	// for research
	unsigned long long int sizeofmem = 0;
	double transmemsize = 0.0;
	int bytelevel = 0;
	char byteunit[5][4] = {"B", "KB", "MB", "GB", "TB"};
	char trc_fname[100];
	int in_count=0, del_count=0, ser_count=0;
	long long int trc_key;

	if (argc < 3) {
		printf(" Usage: ./exefile group_size trc_fname\n");
		printf("e.g: ./exefile 100 ./trace/fname\n");
		return 1;
	}

	GROUP_SIZE = atoi(argv[1]); 
	if (GROUP_SIZE < 1) {
		printf("Error:group size should be greater than 0.\n");
		return 1;
	}
	strcpy(trc_fname, argv[2]);
	fp = fopen(trc_fname, "r");
	if(!fp){
		printf("Failed to open a file %s\n",trc_fname);
		exit(-1);
	}
	printf("Run ...\n");

//------------------time start--------------------//
	srand((unsigned) time(&t));
	gettimeofday(&start, NULL);

	root = simul_rb_alloc_root();
	init_rb_root(root);
  	//insert
    printf("Insert ... \n");
    while(!feof(fp)){
         fscanf(fp,"%lld",&trc_key);
         retval = simul_rb_insert(root, (long long int)trc_key);
         in_count+=1;
    }
 
    //search
    printf("Search ... \n");
    fseek(fp, 0, SEEK_SET);
 
    while(!feof(fp)){
         fscanf(fp,"%lld",&trc_key);
         ser_key = simul_rb_search_key(root, (long long int)trc_key);
         ser_count+=1;
     }
 
     //delete
     printf("Delete ... \n");
     fseek(fp, 0, SEEK_SET);
     while(!feof(fp)){
         fscanf(fp,"%lld",&trc_key);
         retval = simul_rb_delete(root, (long long int)trc_key);
         del_count+=1;
     }
     fclose(fp);
	
	gettimeofday(&end, NULL);

	sizeofmem = (cnt_node * (sizeof(unsigned int) + sizeof(long long int) + (sizeof(struct simul_rb_node *)*3) + sizeof(unsigned int))) + (cnt_key * (sizeof(struct simul_list_head)));
	while (sizeofmem / 1024 > 0) {
		transmemsize = (double)sizeofmem / 1024;
		sizeofmem /= 1024;
		bytelevel++;
	}

	printf(" number of node : %llu\n", cnt_node);
	printf(" size of tree memory : %.4lf %s\n", transmemsize, byteunit[bytelevel]);
	printf(" insert : %d, search : %d, delete : %d\n", in_count, ser_count, del_count);
	printf(" total read count : %llu\n", read_count);
	printf(" total write count : %llu\n", write_count + color_write_count + point_count);
	printf(" Elapsed time: %6.2f s\n", end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec) / 1000000.0);
	printf(" Elapsed time: %lf us\n",((end.tv_sec - start.tv_sec)*1000000.0) + (end.tv_usec - start.tv_usec));
	return 0;
}
