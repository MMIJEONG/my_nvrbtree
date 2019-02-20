
#include "nv_rbtree.h"

//-----------------------------------------------//
extern rb_node *parent_of(rb_node *node) {
	return node->parent;
}
extern void rb_tree_free(rb_node *node) {
	free(node);
}

extern rb_node *right_of(rb_node *node) {
	return node->right;
}

extern rb_node *left_of(rb_node *node) {
	return node->left;
}
//-----------------------------------------------//
unsigned long long int cnt_node = 0;
unsigned long long int write_count = 0;
unsigned long long int color_write_count = 0;
unsigned long long int read_count = 0;

// init functions 
void rb_init_key(rb_key *key_node, long long int key){
	key_node->key = key;
}
void rb_init_node(rb_root *T, rb_node *node, rb_key *key_node) {
	set_rb_red(node);
	node->primary_key = key_node->key / (GROUP_SIZE);
	node->keys = (struct rb_key *) malloc (sizeof(struct rb_key) * (GROUP_SIZE));
	memset(node->keys, 0, sizeof(struct rb_key) * (GROUP_SIZE));
	node->keys[key_node->key % (GROUP_SIZE)].key = key_node->key;
	node->cnt_key = 1;
	node->parent = T->nil_node;
	node->right = T->nil_node;
	node->left = T->nil_node;
	node->count = 1;
	write_count++;
	node->is_free = 0;
}

void rb_init_root(rb_root *root) {
	root->nil_node = (rb_node *)malloc(sizeof(rb_node));
	root->root_node = (rb_node *)malloc(sizeof(rb_node));

	set_rb_black(root->nil_node);
	root->nil_node->primary_key = SIMUL_RB_NIL;
	root->nil_node->keys = (struct rb_key *) malloc (sizeof(rb_key) * (GROUP_SIZE));
	memset(root->nil_node->keys, 0, sizeof(struct rb_key) * (GROUP_SIZE));
	root->nil_node->cnt_key = 0;
	root->nil_node->parent = root->nil_node;
	root->nil_node->right = root->nil_node;
	root->nil_node->left = root->nil_node;
	root->nil_node->count = 1;
	root->nil_node->is_free = 0;

	set_rb_black(root->root_node);
	root->root_node->primary_key = SIMUL_RB_NIL;
	root->root_node->keys = (struct rb_key *) malloc (sizeof(struct rb_key) * (GROUP_SIZE));
	memset(root->nil_node->keys, 0, sizeof(struct rb_key) * (GROUP_SIZE));
	root->root_node->cnt_key = 0;
	root->root_node->parent = root->nil_node;
	root->root_node->left = root->nil_node;
	root->root_node->right = root->nil_node;
	root->root_node->count = 1;
	root->root_node->is_free = 0;
}
//------------color functions----------------//
unsigned int is_rb_black(rb_node *node) {
	return node->color == SIMUL_RB_BLACK ? 1 : 0;
}

unsigned int is_rb_red(rb_node *node) {
	return node->color == SIMUL_RB_RED ? 1 : 0;
}

void set_rb_black(rb_node *node) {
	node->color = SIMUL_RB_BLACK;
	color_write_count++;
}


void set_rb_red(rb_node *node) {
	node->color = SIMUL_RB_RED;
	color_write_count++;
}

rb_root *rb_alloc_root(void) {
	rb_root *ret = (rb_root *)malloc(sizeof(rb_root));
	return ret;
}
//---------------reblancing functions--------------//
rb_node *rb_rotate_left(rb_root *T, rb_node *node) {
	rb_node *ret = NULL;
	rb_node *right_node = right_of(node);

	/* exchange left child of node_right to right child of node */
	node->right = left_of(right_node);
	node->count++;   
	write_count++;

	if (left_of(right_node) != T->nil_node) {
		left_of(right_node)->parent = node;
		left_of(right_node)->count++;
		write_count++;  
	}

	/* change parent connection */
	right_node->parent = parent_of(node);
	right_node->count++;
	write_count++;  

	if (node == parent_of(node)->left) {   /* case: node is left child of parent */
		parent_of(node)->left = right_node;
		parent_of(node)->count++;  
		write_count++;
	}
	else if (node == parent_of(node)->right) {   /* case: node is right child of parent */
		parent_of(node)->right = right_node;
		parent_of(node)->count++;   
		write_count++;
	}

	right_node->left = node;
	right_node->count++;   
	write_count++;
	node->parent = right_node;
	node->count++;   
	write_count++;

	ret = node;

	return ret;
}

rb_node *rb_rotate_right(rb_root *T, rb_node *node) {
	rb_node *ret = NULL;
	rb_node *left_node = left_of(node);

	/* exchange right child of node_left to left child of node */
	node->left = right_of(left_node);
	node->count++;   
	write_count++;

	if (right_of(left_node) != T->nil_node) {
		right_of(left_node)->parent = node;
		right_of(left_node)->count++;   
		write_count++;
	}

	/* change parent connection */
	left_node->parent = parent_of(node);
	left_node->count++;  
	write_count++;

	if (node == parent_of(node)->left) {   /* case: node is left child of parent */
		parent_of(node)->left = left_node;
		parent_of(node)->count++;   
		write_count++;
	}
	else if (node == parent_of(node)->right) {   /* case: node is right child of parent */
		parent_of(node)->right = left_node;
		parent_of(node)->count++;   
		write_count++;
	}

	left_node->right = node;
	left_node->count++;   
	write_count++;
	node->parent = left_node;
	node->count++;   
	write_count++;

	ret = node;

	return ret;
}

/* change parent of chg_node from parent of pre_node */
unsigned int rb_transplant(rb_root *T, rb_node *pre_node, rb_node *chg_node) {
	unsigned int ret = 0;

	if (parent_of(pre_node) == T->nil_node)   /* case: node is root node */
		T->root_node = chg_node;
	else if (pre_node == parent_of(pre_node)->left)   /* case: node is left child of parent */
		parent_of(pre_node)->left = chg_node;
	else if (pre_node == parent_of(pre_node)->right)   /* case: node is right child of parent */
		parent_of(pre_node)->right = chg_node;

	chg_node->parent = parent_of(pre_node);   /* change chg_node's parent */

	return ret;
}

rb_node *rb_minimum(rb_node *node) {
	rb_node *min_node = node;
	while (left_of(min_node)->primary_key != SIMUL_RB_NIL)
		min_node = left_of(min_node);

	return min_node;
}

rb_node *rb_successor(rb_root* T, rb_node *node) {
	rb_node *y;
	rb_node *nil = T->nil_node;
	rb_node *root = T->root_node;

	if (nil != (y = node->right)) {
		while (y->left != nil) {
			y = y->left;
		}
		return (y);
	}
	else {
		y = node->parent;
		while (node == y->right) {
			node = y;
			y = y->parent;
		}
		if (y == root) return (nil);
		return (y);
	}
}

//-------------access functions-----------------//
int rb_insert(rb_root *T, long long int inkey) {
	int ret = 0, is_in_group = 0;
	rb_node *comp_node = T->root_node->left;
	rb_node *in_parent_pos = T->root_node;
	rb_node *new_node = (rb_node *)malloc(sizeof(rb_node));
	long long int searching_inkey = inkey / (GROUP_SIZE);
	rb_key *new_key = NULL;
	while (comp_node != T->nil_node) {
		in_parent_pos = comp_node;
		if (searching_inkey < comp_node->primary_key){
			comp_node = left_of(comp_node);
			read_count++;
		}		
		else if (searching_inkey > comp_node->primary_key){
			comp_node = right_of(comp_node);
			read_count++;
		}
		else if (searching_inkey == comp_node->primary_key) {
			is_in_group = 1;
			break;
		}
	}
	if (is_in_group == 1) {
		comp_node->keys[inkey % (GROUP_SIZE)].key = inkey;
		comp_node->cnt_key++;
		return SIMUL_RB_IN_GROUP;
	}

	// init node
	new_key = (rb_key *)malloc(sizeof(rb_key));
	rb_init_key(new_key, inkey);

	rb_init_node(T, new_node, new_key);
	cnt_node++;
	new_node->parent = in_parent_pos;
	if (in_parent_pos == T->root_node) {   /* the first insert (rbtree is empty) */
		T->root_node->left = new_node;
		T->root_node->count++;   
		write_count++;
	}
	else if (new_node->primary_key < in_parent_pos->primary_key) {
		in_parent_pos->left = new_node;
		in_parent_pos->count++;  
		write_count++;
	}
	else {
		in_parent_pos->right = new_node;
		in_parent_pos->count++;   
		write_count++;
	}
	rb_insert_fixup(T, new_node);
	return ret;
}

void rb_insert_fixup(rb_root *T, rb_node *alt_node) {
	rb_node *across_parent = NULL;

	while (is_rb_red(parent_of(alt_node))) {
		if (parent_of(alt_node) == parent_of(alt_node)->parent->left) {

			across_parent = parent_of(alt_node)->parent->right;

			if (is_rb_red(across_parent)) {   /* case 1: color of across is red */
				set_rb_black(parent_of(alt_node));
				set_rb_black(across_parent);
				set_rb_red(parent_of(alt_node->parent));
				alt_node = parent_of(alt_node->parent);
			}
			else {
				if (alt_node == parent_of(alt_node)->right) {   /* case 2: color of across is black && alt_node is right child */
					alt_node = parent_of(alt_node);
					alt_node = rb_rotate_left(T, alt_node);
				}
				/* case 3: color of across is black && alt_node is left child */
				set_rb_black(parent_of(alt_node));
				set_rb_red(parent_of(alt_node->parent));
				rb_rotate_right(T, parent_of(alt_node->parent));
			}
		}
		else if (parent_of(alt_node) == parent_of(alt_node)->parent->right) {

			across_parent = parent_of(alt_node)->parent->left;

			if (is_rb_red(across_parent)) {   /* case 1: color of across is red */
				set_rb_black(parent_of(alt_node));
				set_rb_black(across_parent);
				set_rb_red(parent_of(alt_node->parent));
				alt_node = parent_of(alt_node->parent);
			}
			else {
				if (alt_node == parent_of(alt_node)->left) {   /* case 2: color of across is black && alt_node is right child */
					alt_node = parent_of(alt_node);
					alt_node = rb_rotate_right(T, alt_node);
				}
				/* case 3: color of across is black && alt_node is left child */
				set_rb_black(parent_of(alt_node));
				set_rb_red(parent_of(alt_node->parent));
				rb_rotate_left(T, parent_of(alt_node->parent));
			}
		}
	}

	set_rb_black(T->root_node->left); /* rule 2: color of root node is black */
}

int rb_delete(rb_root *T, long long int delkey) {
	int ret = 0, is_in_group = 0;
	rb_node *del_node = rb_search(T, delkey);
	rb_node *y;
	rb_node *x;
	if (del_node == T->nil_node)
		return SIMUL_RB_NOT_EXIST;

	memset(&(del_node->keys[delkey % (GROUP_SIZE)]), 0, sizeof(rb_key));
	del_node->cnt_key--;

	if (del_node->cnt_key > 0)
		return SIMUL_RB_IN_GROUP;
	
	y = ((del_node->left == T->nil_node) || (del_node->right == T->nil_node)) ? del_node : rb_successor(T, del_node);
	x = (y->left == T->nil_node) ? y->right : y->left;

	x->count++;   
	write_count++;
	if (T->root_node == (x->parent = y->parent)) {/* assignment of y->p to x->p is intentional */
		T->root_node->left = x;
		T->root_node->count++;   
		write_count++;
	}
	else {
		if (y == y->parent->left) {
			y->parent->left = x;
			y->parent->count++;   
			write_count++;
		}
		else {
			y->parent->right = x;
			y->parent->count++;   
			write_count++;
		}
	}

	if (y != del_node) {
		if (is_rb_black(y))
			rb_delete_fixup(T, x);

		y->left = del_node->left;
		y->count++;  
		write_count++;
		y->right = del_node->right;
		y->count++;   
		write_count++;
		y->parent = del_node->parent;
		y->count++;   
		write_count++;
		y->color = del_node->color;
		del_node->left->parent = del_node->right->parent = y;
		del_node->left->count++;      
		write_count++;
		del_node->right->count++;   
		write_count++;
		if (del_node == del_node->parent->left) {
			del_node->parent->left = y;
			del_node->parent->count++;   
			write_count++;
		}
		else {
			del_node->parent->right = y;
			del_node->parent->count++;   
			write_count++;
		}

		free(del_node->keys);
		free(del_node);

	}
	else {
		if (is_rb_black(y))
			rb_delete_fixup(T, x);

		free(y->keys);
		free(y);
	}
	T->nil_node->parent = T->nil_node;
	T->nil_node->count++;   
	write_count++;
	T->nil_node->left = T->nil_node;
	T->nil_node->count++;   
	write_count++;
	T->nil_node->right = T->nil_node;
	T->nil_node->count++;   
	write_count++;
	return ret;
}

void rb_delete_fixup(rb_root *T, rb_node *chg_node) {
	rb_node *bro_node = NULL;
	while (chg_node != T->root_node && is_rb_black(chg_node)) {

		if (chg_node == parent_of(chg_node)->left) {
			bro_node = parent_of(chg_node)->right;

			if (is_rb_red(bro_node)) { /* case 1: color of bro node is red */
				set_rb_black(bro_node);
				set_rb_red(parent_of(chg_node));
				rb_rotate_left(T, parent_of(chg_node));
				bro_node = parent_of(chg_node)->right;
			}

			if (is_rb_black(left_of(bro_node)) && is_rb_black(right_of(bro_node))) {   /* case 2: color of bro node is black && both of bro's children are black */
				set_rb_red(bro_node);
				chg_node = parent_of(chg_node);

			}
			else {
				if (is_rb_black(right_of(bro_node))) {   /* case 3: bro is black && left child is red && right child is black */
					set_rb_black(left_of(bro_node));
					set_rb_red(bro_node);
					rb_rotate_right(T, bro_node);
					bro_node = parent_of(chg_node)->right;
				}
				bro_node->color = parent_of(chg_node)->color;
				set_rb_black(parent_of(chg_node));
				set_rb_black(right_of(bro_node));
				rb_rotate_left(T, parent_of(chg_node));
				chg_node = T->root_node;
			}
		}
		else {
			bro_node = parent_of(chg_node)->left;

			if (is_rb_red(bro_node)) { /* case 1: color of bro node is red */
				set_rb_black(bro_node);
				set_rb_red(parent_of(chg_node));
				rb_rotate_right(T, parent_of(chg_node));
				bro_node = parent_of(chg_node)->left;
			}

			if (is_rb_black(left_of(bro_node)) && is_rb_black(right_of(bro_node))) {   /* case 2: color of bro node is black && both of bro's children are black */
				set_rb_red(bro_node);
				chg_node = parent_of(chg_node);

			}
			else {
				if (is_rb_black(left_of(bro_node))) {   /* case 3: bro is black && right child is red && left child is black */
					set_rb_black(right_of(bro_node));
					set_rb_red(bro_node);
					rb_rotate_left(T, bro_node);
					bro_node = parent_of(chg_node)->left;
				}
				bro_node->color = parent_of(chg_node)->color;
				set_rb_black(parent_of(chg_node));
				set_rb_black(left_of(bro_node));
				rb_rotate_right(T, parent_of(chg_node));
				chg_node = T->root_node;
			}
		}
	} //while
	set_rb_black(chg_node);
}

rb_node *rb_search(rb_root *T, long long int find_key) {
	rb_node *search_node = T->root_node->left;
	long long int find_primary_key = find_key / (GROUP_SIZE);

	while (search_node != T->nil_node) {
		if (find_primary_key == search_node->primary_key)
			break;

		if (find_primary_key < search_node->primary_key){
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
rb_key *rb_search_key(rb_root *T, long long int find_key) {
	rb_node *find_node = rb_search(T, find_key);
 	rb_key *cur_key = NULL;
	if (find_node == T->nil_node)
		return 0;

	cur_key = &(find_node->keys[find_key % (GROUP_SIZE)]);
	return cur_key;
}
