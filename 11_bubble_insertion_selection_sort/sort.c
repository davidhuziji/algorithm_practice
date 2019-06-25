/*
 * Copyright (c) 2019 Hu Ziji. All rights reserved.
 * Auther: Hu Ziji <davidhuzj@gmail.com>
 */

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_NR_ELEMENTS		0x20
#define MIN_NR_ELEMENTS		0x10
#define BASE_VALUE		0x20

struct list_node {
	int data;
	struct list_node *next;
};

static void *generate_array(int *nr)
{
	int num, i;
	char *array;

	if (!nr)
		return NULL;

	srand(time(NULL));

	num = rand() % MAX_NR_ELEMENTS + MIN_NR_ELEMENTS;

	array = malloc(num * sizeof(*array));
	if (!array)
		return NULL;

	printf("Generate an array: ");
	for (i = 0; i < num; i++) {
		array[i] = rand() % 0x20;
		printf("%d ", array[i]);
	}
	printf("\n");

	*nr = num;
	return array;
}

static void bubble_sort_array(char *array, int nr)
{
	char *local, i, j, tmp;
	bool change;

	if (!array || (nr == 0))
		return;

	local = malloc(nr * sizeof(*local));
	if (!local)
		return;

	memcpy(local, array, nr);

	/*
	 * XXX
	 * The outer loop limit should be n - 1, instead of n. After n - 1 loop,
	 * the largest n - 1 elements have been moved.
	 */
	for (i = 0; i < nr - 1; i++) {
		change = false;
		/*
		 * Take care of the limit of j.
		 * The last i elements have been compared and settled
		 */
		for (j = 0; j < nr - i - 1; j++) {
			if (local[j] > local[j + 1]) {
				tmp = local[j];
				local[j] = local[j + 1];
				local[j + 1] = tmp;

				change = true;
			}
		}

		/*
		 * Optimization.
		 * If no exchange occurred in this loop, it means the sort is
		 * completed
		 */
		if (!change)
			break;
	}

	printf("\nAfter bubble sort with array: \t\t");
	for (i = 0; i < nr; i++)
		printf("%d ", local[i]);
	printf("\n");

	free(local);
}

static void insertion_sort_array(char *array, int nr)
{
	char *local, i, j, tmp;

	if (!array || (nr == 0))
		return;

	local = malloc(nr * sizeof(*local));
	if (!local)
		return;

	memcpy(local, array, nr);

	/* Start from local[1] since local[0] will be naturally be sorted. */
	for (i = 1; i < nr; i++) {
		/*
		 * The value to be compared should be saved in advance.
		 * Otherwise, it will be overwritten by moving elements.
		 */
		tmp = local[i];
		/*
		 * Search from the tail of sorted part to the array head.
		 * It can save the cost of moving elements each time when
		 * comparing from the sorted array head.
		 */
		for (j = i - 1; j >= 0; j--) {
			if (tmp < local[j])
				/* Move the element to next one behind it */
				local[j + 1] = local[j];
			else
				break;
		}
		/*
		 * There is a special case that local[i] is smaller than all the
		 * elements in sorted part. The setting must be put outside the
		 * j looping. Othwise, if it is put inside break branch in above
		 * j looping, the local[i] will be lost in that specail case.
		 */
		local[j + 1] = tmp;
	}

	printf("\nAfter insertion sort with array: \t");
	for (i = 0; i < nr; i++)
		printf("%d ", local[i]);
	printf("\n");

	free(local);
}

static void selection_sort_array(char *array, int nr)
{
	char *local, i, j, least, least_idx;

	if (!array || (nr == 0))
		return;

	local = malloc(nr * sizeof(*local));
	if (!local)
		return;

	memcpy(local, array, nr);

	for (i = 0; i < nr - 1; i++) {
		least = local[i];
		least_idx = i;

		for (j = i + 1; j < nr; j++) {
			if (least > local[j]) {
				least = local[j];
				least_idx = j;
			}
		}

		least = local[i];
		local[i] = local[least_idx];
		local[least_idx] = least;
	}

	printf("\nAfter selection sort with array: \t");
	for (i = 0; i < nr; i++)
		printf("%d ", local[i]);
	printf("\n");

	free(local);
}

static void list_insert(struct list_node *prev, struct list_node *node)
{
	struct list_node *tmp;

	if (!prev || !node)
		return;

	tmp = prev->next;
	prev->next = node;
	node->next = tmp;
}

static struct list_node *list_del(struct list_node *node)
{
	struct list_node *next;

	if (!node || !node->next)
		return NULL;

	next = node->next;
	node->next = next->next;
	next->next = NULL;

	return next;
}

static void free_list(struct list_node *head)
{
	struct list_node *tmp, *node;

	node = head->next;
	while (node) {
		tmp = node->next;
		free(node);
		node = tmp;
	}
}

static void print_list(struct list_node *head)
{
	struct list_node *node;

	node = head->next;
	while (node) {
		printf("%d ", node->data);
		node = node->next;
	}
	printf("\n");
}

static void generate_list(struct list_node *head, char *array, int nr)
{
	int i;
	struct list_node *node, *last;

	if (!head || !array || !nr)
		return;

	head->data = 0;
	head->next = NULL;
	last = head;

	for (i = 0; i < nr; i++) {
		node = malloc(sizeof(*node));
		if (!node)
			goto free_list;

		node->data = array[i];
		node->next = NULL;

		list_insert(last, node);
		last = node;
	}

	return;

free_list:
	free_list(head->next);
}

/*
 * It is complext to exchange two list nodes.
 * It is necessary to keep tack of the previous node before the two nodes to
 * be exchanged.
 */
static void bubble_sort_list(struct list_node *head, int nr)
{
	int i, j;
	bool exchange;
	struct list_node *prev, *curr, *next;

	if (!head || !head->next || (nr == 0))
		return;

	for (i = 0; i < nr - 1; i++) {
		exchange = false;
		prev = head;
		curr = head->next;
		next = curr->next;

		for (j = 0; j < nr - i - 1; j++) {
			if (curr->data > next->data) {
				prev->next = next;
				curr->next = next->next;
				next->next = curr;
				exchange = true;
			}

			prev = prev->next;
			curr = prev->next;
			next = curr->next;
		}

		if (!exchange)
			break;
	}

	printf("\nAfter buble sort with list: \t\t");
	print_list(head);
}

/*
 * Link the sorted part in the new list.
 * It is still necessary to keep track of the previous node after which the
 * node under comparasion is inserted.
 * To keep stability of list, the node under comparasion must be insert before
 * the node larger than it.
 */
static void insertion_sort_list(struct list_node *head, int nr)
{
	struct list_node new_head, *node, *prev, *cmp;
	bool insert;

	if (!head || !head->next || (nr == 0))
		return;

	new_head.data = 0;
	new_head.next = NULL;

	node = list_del(head);
	list_insert(&new_head, node);

	while (head->next) {
		node = list_del(head);
		prev = &new_head;
		cmp = prev->next;
		insert = false;

		while (cmp) {
			if (node->data < cmp->data) {
				list_insert(prev, node);
				insert = true;
				break;
			}

			prev = cmp;
			cmp = prev->next;
		}

		if (!insert)
			list_insert(prev, node);
	}

	printf("\nAfter insertion sort with list: \t");
	print_list(&new_head);

	head->next = new_head.next;
}

/*
 * Link the sorted part to a new list.
 * It is necessary to keep track of the previous node thus the least node can
 * be deleted from the original list.
 * Also keep track of the tail of new list thus the least node can be added
 * to the tail of new list more easilier.
 */
static void selection_sort_list(struct list_node *head, int nr)
{
	struct list_node new_head, *node, *prev, *least_prev_node, *tail;
	int least;

	if (!head || !head->next || (nr == 0))
		return;

	new_head.data = 0;
	new_head.next = NULL;
	tail = &new_head;

	while (head->next) {
		least = head->next->data;
		least_prev_node = head;
		prev = head->next;

		while (prev->next) {
			if (prev->next->data < least) {
				least = prev->next->data;
				least_prev_node = prev;
			}

			prev = prev->next;
		}

		node = list_del(least_prev_node);
		list_insert(tail, node);
		tail = node;
	}

	printf("\nAfter selection sort with list: \t");
	print_list(&new_head);

	head->next = new_head.next;
}

int main(void)
{
	char *array;
	int nr;
	struct list_node head;

	array = (char *)generate_array(&nr);
	if (!array)
		return -ENOMEM;

	bubble_sort_array(array, nr);

	insertion_sort_array(array, nr);

	selection_sort_array(array, nr);

	generate_list(&head, array, nr);
	bubble_sort_list(&head, nr);
	free_list(&head);

	generate_list(&head, array, nr);
	insertion_sort_list(&head, nr);
	free_list(&head);

	generate_list(&head, array, nr);
	selection_sort_list(&head, nr);
	free_list(&head);

	free(array);

	return 0;
}