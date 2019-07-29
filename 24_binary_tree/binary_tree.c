/*
 * Copyright (c) 2019 Hu Ziji. All rights reserved.
 * Auther: Hu Ziji <davidhuzj@gmail.com>
 */

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#define MAX_NODE_VAL		0x100
#define NODE_VAL_BASE		0x80

#define BASE_NR_NODE		0x3

#define NODE_RED		0x1
#define NODE_BLACK		0x2
#define NODE_READ_BLACK		0x3
#define NODE_BLACK_BLACK	0x4
#define NODE_BLACK_NULL		0x5

struct tree_node {
	uint32_t val;
	struct tree_node *left;
	struct tree_node *right;

	uint8_t color_type;
	uint8_t layer;
	uint32_t idx;
};

struct array_node {
	struct tree_node *node;

	bool is_valid;
};

struct binary_tree {
	struct tree_node *root;

	uint32_t nr;

	struct array_node *array;
	uint32_t len;
};

const static struct tree_node black_null_leaf = {
	.val		= 0,
	.left		= NULL,
	.right		= NULL,
	.color_type	= NODE_BLACK_NULL,
	.layer		= 0,
	.idx		= 0,
};

struct binary_tree *create_tree_root(void)
{
	struct binary_tree *tree;

	tree = malloc(sizeof(*tree));
	if (!tree)
		return NULL;

	tree->root = malloc(sizeof(*tree->root));
	if (!tree->root) {
		free(tree);
		return NULL;
	}

	tree->array = malloc(sizeof(*tree->array) * BASE_NR_NODE);
	if (!tree->array) {
		free(tree->root);
		free(tree);
		return NULL;
	}

	tree->root->val = NODE_VAL_BASE;
	tree->root->left = &black_null_leaf;
	tree->root->right = &black_null_leaf;
	tree->root->color_type = NODE_BLACK;
	tree->root->layer = 0x0;
	tree->root->idx = 0x0;

	tree->nr = 0x1;

	memset(tree->array, 0, sizeof(*tree->array) * BASE_NR_NODE);

	tree->array[0].node = tree->root;
	tree->array[0].is_valid = true;
	tree->len = BASE_NR_NODE;

	return tree;
}

struct tree_node *alloc_tree_node(void)
{
	struct tree_node *node;

	node = malloc(sizeof(*node));
	if (!node)
		return NULL;

	node->val = rand() % MAX_NODE_VAL;
	node->left = &black_null_leaf;
	node->right = &black_null_leaf;
	node->color_type = NODE_RED;
	node->layer = 0x0;
	node->idx = 0x0;

	printf("New node 0x%02x\n", node->val);

	return node;
}

void print_binary_tree(const struct binary_tree *tree)
{
	int i, j, k, nr_space;

	if (!tree)
		return;

	nr_space = tree->len;
	for (i = 0; i < tree->len; i = i * 2 + 1) {
		nr_space /= 2;

		for (j = 0; j < nr_space; j++)
			printf(" ");

		for (j = i; j <= i * 2; j++) {
			printf("%02x ", j);
			if (j < i * 2) {
				for (k = 0; k < nr_space; k++)
					printf("   ");
			}
		}

		printf("\n");
		for (j = 0; j < nr_space; j++)
			printf(" ");

		for (j = i; j <= i * 2; j++) {
			if (tree->array[j].is_valid) {
				if (tree->array[j].node->color_type ==
				    NODE_BLACK)
					printf("\033[31m%02x ", tree->array[j].node->val);
				else if (tree->array[j].node->color_type ==
					 NODE_RED)
					printf("\033[40m%02x ", tree->array[j].node->val);
			} else {
				printf("|| ");
			}

			if (j < i * 2) {
				for (k = 0; k < nr_space; k++)
					printf("   ");
			}

		}
		printf("\n");
	}
	printf("\n");
}

static inline bool is_leaf_node(const struct tree_node *node)
{
	if ((node->color_type == NODE_BLACK_NULL) && !node->left &&
	    !node->right)
		return true;

	return false;
}

static void insert_node_array(struct binary_tree *tree, struct tree_node *node)
{
	struct array_node *array;
	uint32_t len = tree->len;

	/* Array is not enough to hold new node */
	if (node->idx >= tree->len) {
		len = len * 2 + 1;

		array = malloc(sizeof(*array) * len);
		if (!array)
			return;

		memcpy(array, tree->array, tree->len * sizeof(*array));
		memset(array + tree->len, 0, (tree->len + 1) * sizeof(*array));

		free(tree->array);
		tree->array = array;
		tree->len = len;
	}

	tree->array[node->idx].val = node->val;
	tree->array[node->idx].is_valid = true;
}

static void binary_tree_add_node(struct binary_tree *tree,
				 struct tree_node *ins_node)
{
	struct tree_node *node;

	if (!tree)
		return;

	node = tree->root;

	if (!node || !ins_node)
		return;

	while (!is_leaf_node(node)) {
		if (ins_node->val >= node->val) {
			if (node->right)
				node = node->right;
			else
				break;
		} else {
			if (node->left)
				node = node->left;
			else
				break;
		}	
	}

	if (ins_node->val >= node->val) {
		node->right = ins_node;
		ins_node->idx = node->idx * 2 + 2;
	} else {
		node->left = ins_node;
		ins_node->idx = node->idx * 2 + 1;
	}

	ins_node->layer = node->layer + 1;
	tree->nr++;

	insert_node_array(tree, ins_node);
}

static void __move_left_branch(struct tree_node *node)
{
	struct tree_node *lnode, *lleaf;

	if (!node->left)
		return;

	lnode = node->left;

	lleaf = node->right;
	while (lleaf->left)
		lleaf = lleaf->left;

	lleaf->left = lnode;
}

static void __del_right_node(struct tree_node *node, struct tree_node *rnode)
{
	if (!node || !rnode)
		return;

	if (rnode->right) {
		node->right = rnode->right;

		__move_left_branch(rnode);
	} else if (rnode->left) {
		node->right = rnode->left;
	} else {
		node->right = NULL;
	}

	free(rnode);
}

static void __del_left_node(struct tree_node *node, struct tree_node *lnode)
{
	if (!node || !lnode)
		return;

	if (lnode->right) {
		node->left = lnode->right;

		__move_left_branch(lnode);
	} else if (lnode->left) {
		node->left = lnode->left;
	} else {
		node->left = NULL;
	}

	free(lnode);
}

static uint32_t __binary_tree_reload(struct tree_node *node, uint32_t idx,
				     uint32_t layer, bool is_left)
{
	uint32_t left_layer = 0, right_layer = 0;

	node->layer = layer + 1;

	if (is_left)
		node->idx = idx * 2 + 1;
	else
		node->idx = idx * 2 + 2;


	if (node->left)
		left_layer = __binary_tree_reload(node->left, node->idx,
						  node->layer, true);
	if (node->right)
		right_layer = __binary_tree_reload(node->right, node->idx,
						   node->layer, false);

	return left_layer > right_layer ? left_layer + 1 : right_layer + 1;
}

static void __binary_tree_array_reload(struct tree_node *node,
				       struct array_node *array)
{
	array[node->idx].is_valid = true;
	array[node->idx].val = node->val;

	if (node->left)
		__binary_tree_array_reload(node->left, array);
	if (node->right)
		__binary_tree_array_reload(node->right, array);
}

static void binary_tree_reload(struct binary_tree *tree)
{
	int last_start, i;
	uint32_t len, layer, left_layer = 0, right_layer = 0;
	struct array_node *new_array;

	tree->root->idx = 0x0;
	tree->root->layer = 0x0;

	if (tree->root->left)
		left_layer = __binary_tree_reload(tree->root->left, 0x0, 0x0,
						  true);
	if (tree->root->right)
		right_layer = __binary_tree_reload(tree->root->right, 0x0, 0x0,
						   false);

	layer = left_layer > right_layer ? left_layer + 1 : right_layer + 1;

	len = 0;
	for (i = 0; i < layer; i++) {
		len = len * 2 + 1;
	}

	if (len != tree->len) {
		new_array = malloc(sizeof(*new_array) * len);
		free(tree->array);

		memset(new_array, 0, sizeof(*tree->array) * len);
		__binary_tree_array_reload(tree->root, new_array);
		tree->array = new_array;
		tree->len = len;
	}
}

static bool binary_tree_del_node(struct binary_tree *tree, uint32_t val)
{
	struct tree_node *node;
	bool found = false;

	if (!tree)
		return false;

	node = tree->root;
	if (!node)
		return false;

	if (node->val == val) {
		found = true;
		if (node->right) {
			__move_left_branch(node);

			tree->root = node->right;
		} else if (node->left) {
			tree->root = node->left;
		} else {
			tree->root = NULL;
		}

		tree->nr--;
		free(node);
	}

	while (!is_leaf_node(node)) {
		if (node->val < val) {
			if (node->right) {
				if (node->right->val == val) {
					found = true;
					__del_right_node(node, node->right);
					tree->nr--;
				} else {
					node = node->right;
				}
			} else {
				break;
			}
		} else {
			if (node->left) {
				if (node->left->val == val) {
					found = true;
					__del_left_node(node, node->left);
					tree->nr--;
				} else {
					node = node->left;
				}
			} else {
				break;
			}
		}
	}

	if (found)
		binary_tree_reload(tree);

	return found;
}

static void free_node(struct tree_node *node)
{
	if (!is_leaf_node(node)) {
		free(node);
		return;
	}
	if (node->left)
		free_node(node->left);

	if (node->right)
		free_node(node->right);

	free(node);
}

static void free_tree(struct binary_tree *tree)
{
	free(tree->array);

	free_node(tree->root);

	free(tree);
}

int main(void)
{
	struct binary_tree *tree;
	struct tree_node *node;
	uint32_t val;
	int i, add_round = 0x20, del_round = 0x10;

	srand(time(NULL));

	tree = create_tree_root();
	if (!tree)
		return -ENOMEM;

	/* Insert tree node */
	for (i = 0; i < add_round; i++) {
		node = alloc_tree_node();
		if (!node)
			goto free_tree;

		binary_tree_add_node(tree, node);
	}

	print_binary_tree(tree);

	i = 0;
	/* Delete tree nodes */
	while (1) {
		val = rand() % MAX_NODE_VAL;

		if (binary_tree_del_node(tree, val)) {
			i++;
			printf("Delete node 0x%02x\n", val);
			print_binary_tree(tree);
		}

		if (i > del_round)
			break;
	}

free_tree:
	free_tree(tree);
}