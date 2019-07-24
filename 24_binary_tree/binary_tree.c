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

struct tree_node {
	uint32_t val;
	struct tree_node *left;
	struct tree_node *right;

	uint8_t layer;
	uint32_t idx;
};

struct array_node {
	uint32_t val;

	bool is_valid;
};

struct binary_tree {
	struct tree_node *root;

	uint32_t nr;

	struct array_node *array;
	uint32_t len;
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
	tree->root->left = NULL;
	tree->root->right = NULL;
	tree->root->layer = 0x0;
	tree->root->idx = 0x0;

	tree->nr = 0x1;

	memset(tree->array, 0, sizeof(*tree->array) * BASE_NR_NODE);

	tree->array[0].val = tree->root->val;
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
	node->left = NULL;
	node->right = NULL;
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
			for (k = 0; k < nr_space; k++)
				printf("   ");

		}

		printf("\n");
		for (j = 0; j < nr_space; j++)
			printf(" ");

		for (j = i; j <= i * 2; j++) {
			if (tree->array[j].is_valid)
				printf("%02x ", tree->array[j].val);
			else
				printf("|| ");

			for (k = 0; k < nr_space; k++)
				printf("   ");

		}
		printf("\n");
	}
	printf("\n");
}

static inline bool is_leaf_node(const struct tree_node *node)
{
	if (!node->left && !node->right)
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

	print_binary_tree(tree);
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
	int i, add_round = 0x20;

	srand(time(NULL));

	tree = create_tree_root();
	if (!tree)
		return -ENOMEM;

	for (i = 0; i < add_round; i++) {
		node = alloc_tree_node();
		if (!node)
			goto free_tree;

		binary_tree_add_node(tree, node);
	}

	print_binary_tree(tree);

free_tree:
	free_tree(tree);
}