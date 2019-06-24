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

#if 0
static void list_insert(struct list_node *prev, struct list_node *node)
{
	if (!prev || !next)
		return;


}

static void *generate_list(struct list_node *head, char *array, int nr)
{
	if (!head || !array || !nr)
		return NULL;

	head->data = 0;
	head->next = NULL;


}
#endif

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

	for (i = 0; i < nr; i++) {
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

	printf("\nAfter bubble sort: ");
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

	printf("\nAfter insertion sort: ");
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

	printf("\nAfter slection sort: ");
	for (i = 0; i < nr; i++)
		printf("%d ", local[i]);
	printf("\n");

	free(local);
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
#if 0
	generate_list(&head, array, nr);
#endif

	return 0;
}