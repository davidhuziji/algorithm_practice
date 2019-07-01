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

#define NR_MAX_ELEMENT		0x1000
#define NR_MIN_ELEMENT		0x500
#define STEP			0x10
#define BASE			0x20
#define MAX_LOG_VAL		0x50000
#define TEST_DATA		0xABCDE

static void *generate_array(int *len)
{
	int data, step, i, nr;
	uint32_t *array;

	if (!len)
		return NULL;

	nr = rand() % NR_MAX_ELEMENT + NR_MIN_ELEMENT;

	array = malloc(nr * sizeof(*array));
	if (!array)
		return NULL;

	data = rand() % BASE;
	for (i = 0; i < nr; i++) {
		step = rand() % STEP;

		if (data + step >= MAX_LOG_VAL)
			step = 0x0;

		data += step;
		array[i] = data;
	}

	*len = nr;
	return array;
}

static int binary_search(uint32_t *array, int len, uint32_t val)
{
	int start = 0, end = len - 1, mid;

	/*
	 * It must be paid attention here that the condition check is <=,
	 * instead of <.
	 */
	while (start <= end) {
		mid = start + ((end - start) >> 1);
		if (array[mid] == val)
			return mid;

		if (array[mid] < val)
			start = mid + 1;
		else
			end = mid - 1;
	}

	return -1;
}

static int binary_search_recur(uint32_t *array, int start, int end,
			       uint32_t val)
{
	int mid, ret;

	/*
	 * It must be paid attention here that the condition check is >, instead
	 * of ==. Otherwise, when (start + 1 = end) && (arra[start] > val), the
	 * next round will be [start, start - 1], which cause infinite loop.
 	 */
	if (start > end)
		return -1;

	mid = start + ((end - start) >> 1);
	if (array[mid] == val)
		return mid;

	if (array[mid] < val)
		ret = binary_search_recur(array, mid + 1, end, val);
	else
		ret = binary_search_recur(array, start, mid - 1, val);

	return ret;
}

int main(void)
{
	uint32_t *array;
	int i, nr, k, idx, round = 0x5;

	srand(time(NULL));

	array = generate_array(&nr);
	if (!array)
		return -ENOMEM;

	k = rand() % nr;
	printf("Test point index %d value %d\n", k, array[k]);

	idx = binary_search(array, nr, array[k]);
	if (idx < 0)
		printf("Search failed\n");

	if (k == idx) {
		printf("Successfully find the test data\n");
	} else {
		if (array[idx] == array[k])
			printf("Successfully find the same test data\n");
		else
			printf("Search failed\n");
	}

	idx = binary_search(array, nr, TEST_DATA);
	if (idx < 0)
		printf("Search succeeded\n");
	else
		printf("Search failed\n");

	idx = binary_search_recur(array, 0, nr - 1, array[k]);
	if (idx < 0)
		printf("Recursive search failed\n");

	if (k == idx) {
		printf("Recursive find the data successfully\n");
	} else {
		if (array[idx] == array[k])
			printf("Recursive find the same data successfully\n");
		else
			printf("Recursive search failed\n");
	}

	idx = binary_search_recur(array, 0, nr - 1, TEST_DATA);
	if (idx < 0)
		printf("Recursive search succeeded\n");
	else
		printf("Recursive search failed\n");

	free(array);

	return 0;
}