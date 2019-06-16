/*
 * Copyright @ 2019 Hu Ziji. All rights reserved.
 * Auther: Hu Ziji <davidhuzj@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "palindrome_list.h"

static void print_list(struct palind_list_node *head)
{
	struct palind_list_node *node;

	if (!head)
		return;

	node = head;

	printf("Word in list\n");
	while (1) {
		fputc(node->ch, stdout);

		if (!node->next) {
			fputc('\n', stdout);
			return;
		}

		node = node->next;
	}
}

struct palind_list_node *organize_palind_list(const char *word, size_t len)
{
	int i;
	struct palind_list_node *current, *head, *prev = NULL;

	if (!word || (len == 0x0))
		return NULL;

	/* The first node: head of the list */
	head = malloc(sizeof(*head));
	if (!head) {
		printf("Fail to allocate a list node\n");
		return NULL;
	}

	head->ch = word[i];
	prev = head;

	for (i = 1; i < len; i++) {
		current = malloc(sizeof(*current));
		if (!current) {
			printf("Fail to allocate a list node\n");
			printf("Warning: the previous nodes are not freed\n");

			return NULL;
		}

		current->ch = word[i];
		prev->next = current;

		prev = current;
	}

	current->next = NULL;

	print_list(head);

	return head;
}

bool check_palind_list(const struct palind_list_node *head)
{
	size_t nr = 0, nr_pair, sec_start, i, j;
	struct palind_list_node *node, *sec_node, *first_node;

	if (!head)
		return false;

	node = (struct palind_list_node *)head;

	/* Get the number of characters */
	while (1) {
		nr++;
		if (!node->next)
			break;

		node = node->next;
	}

	if (nr == 1)
		return true;

	/* A simple comparasion between the first character and last one */
	if (head->ch != node->ch)
		return false;

	if (nr <= 3)
		return true;

	nr_pair = nr / 2;
	sec_start = nr - nr_pair;

	node = (struct palind_list_node *)head;
	/* Locate the first node of the second half */
	for (i = 1; i < sec_start; i++)
		node = node->next;

	/* Start compare */
	first_node = head->next;
	sec_node = node;
	for (i = 1; i < nr_pair; i++) {
		node = first_node;
		/* Locate the node in the first half */
		for (j = 0; j < nr_pair - i; j++)
			node = node->next;

		if (node->ch != sec_node->ch)
			return false;

		sec_node = sec_node->next;
	}

	return true;
}

void free_palind_list(struct palind_list_node *head)
{
	struct palind_list_node *node, *next;

	if (!head)
		return;

	node = head;
	while (node) {
		next = node->next;
		free(node);
		node = next;
	}
}