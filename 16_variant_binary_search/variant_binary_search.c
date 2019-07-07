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

#define NR_MAX_ELEMENT		0x20
#define NR_MIN_ELEMENT		0x10
#define BASE			0x10

static void *generate_sort_array(int *len)
{
	int data, step, i, nr;
	int32_t *array;

	if (!len)
		return NULL;

	nr = rand() % NR_MAX_ELEMENT + NR_MIN_ELEMENT;

	array = malloc(nr * sizeof(*array));
	if (!array)
		return NULL;

	data = rand() % BASE;
	for (i = 0; i < nr; i++) {
		step = rand() % 0x2;
		if (step)
			data += 0x2;

		array[i] = data;
	}

	*len = nr;

	printf("\nArray data: ");
	for (i = 0; i < nr; i++)
		printf("%d ", array[i]);
	printf("\n");

	return array;
}

static int binary_search_first(const int32_t *array, int len, int32_t val)
{
	int start = 0, end = len - 1, mid;

	while (start <= end) {
		mid = start + ((end - start) >> 1);

		if (array[mid] == val) {
			if ((mid == 0) || (array[mid - 1] < val))
				return mid;
			else
				end = mid - 1;
		} else if (array[mid] < val) {
			start = mid + 1;
		} else if (array[mid] > val) {
			end = mid - 1;
		}
	}

	return -1;
}

static bool test_binary_search_first(const int32_t *array, int len,
				     int32_t val, int idx)
{
	int i;

	for (i = 0; i < len; i++) {
		if (array[i] == val) {
			if (i == idx)
				return true;
			else
				return false;
		}
	}

	return false;
}

static int binary_search_last(const int32_t *array, int len, int32_t val)
{
	int start = 0, end = len - 1, mid;

	while (start <= end) {
		mid = start + ((end - start) >> 1);

		if (array[mid] == val) {
			if ((mid == (len - 1)) || (array[mid + 1] > val))
				return mid;
			else
				start = mid + 1;
		} else if (array[mid] < val) {
			start = mid + 1;
		} else if (array[mid] > val) {
			end = mid - 1;
		}
	}

	return -1;
}

static bool test_binary_search_last(const int32_t *array, int len,
				     int32_t val, int idx)
{
	int i;

	for (i = 0; i < len; i++) {
		if ((array[i] == val) && (i == idx)) {
			if ((i == len - 1) || (array[i + 1] > val))
				return true;
			else
				return false;
		}
	}

	return false;
}

static int binary_search_large_equal(const int32_t *array, int len,
				     int32_t val)
{
	int start = 0, end = len - 1, mid;

	while (start <= end) {
		mid = start + ((end - start) >> 1);

		if (array[mid] >= val) {
			if ((mid == 0) || (array[mid - 1] < val))
				return mid;
			else
				end = mid - 1;
		} else if (array[mid] < val) {
			start = mid + 1;
		} else if (array[mid] > val) {
			end = mid - 1;
		}
	}

	return -1;
}

static bool test_binary_search_large_equal(const int32_t *array, int len,
					   int32_t val, int idx)
{
	int i;
	bool found = false;

	for (i = 0; i < len; i++) {
		if (array[i] >= val)
			found = true;
	}

	if (!found && (idx == -1))
		return true;
	else if (!found && (idx >= 0))
		return false;
	else if (found && (idx == -1))
		return false;

	for (i = 0; i < len; i++) {
		if (array[i] >= val) {
			if (i == idx)
				return true;
			else
				return false;
		}
	}

	return false;
}

static int binary_search_small_equal(const int32_t *array, int len,
				     int32_t val)
{
	int start = 0, end = len - 1, mid;

	while (start <= end) {
		mid = start + ((end - start) >> 1);

		if (array[mid] <= val) {
			if ((mid == (len - 1)) || (array[mid + 1] > val))
				return mid;
			else
				start = mid + 1;
		} else if (array[mid] < val) {
			start = mid + 1;
		} else if (array[mid] > val) {
			end = mid - 1;
		}
	}

	return -1;
}

static bool test_binary_search_small_equal(const int32_t *array, int len,
					   int32_t val, int idx)
{
	int i;
	bool found = false;

	for (i = 0; i < len; i++) {
		if (array[i] <= val)
			found = true;
	}

	if (!found && (idx == -1))
		return true;
	else if (!found && (idx >= 0))
		return false;
	else if (found && (idx == -1))
		return false;

	for (i = 0; i < len; i++) {
		if (array[i] <= val) {
			if (i == idx) {
				if ((i == (len - 1)) || (array[i + 1] > val))
					return true;
				else
					return false;	
			}
		}
	}

	return false;
}

static void create_circular_array(int32_t *array, int len)
{
	int first_tail, i;
	int32_t *tmp;

	first_tail = rand() % (len - 1);

	tmp = malloc(len * sizeof(*array));
	if (!tmp)
		return;

	memcpy(tmp, array, len * sizeof(*array));

	memcpy(array, tmp + len - first_tail - 1,
	       (first_tail + 1) * sizeof(*array));
	memcpy(array + first_tail + 1, tmp, (len - first_tail - 1) *
	       sizeof(*array));

	free(tmp);

	printf("\nCreate a circuler array: ");
	for (i = 0; i < len; i++)
		printf("%d ", array[i]);
	printf("\n");
}

/*
 * The basic idea is to squeeze the circular array until start, mid and end
 * point stay in the same sorted array.
 * Then do the normal binary search.
 */
static int binary_search_circular(const int32_t *array, int len, int32_t val)
{
	int start = 0, end = len - 1, mid;

	/* Find the tail of the first half */
	while (start <= end) {
		if (array[end] == val)
			return end;
		if (array[start] == val)
			return start;

		mid = start + ((end - start) >> 1);
		if (array[mid] == val)
			return mid;

		/*
		 * Val sits in the larger half of the circular.
		 * Keep cutting off the end until val locates into the same
		 * part with the end.
		 */
		if (val > array[end]) {
			if (array[mid] < val) {
				if (array[mid] > array[start]) {
					/*
					 * mid sits in the same half as start.
					 * Thus val is at the right of mid.
					 */
					start = mid + 1;
				} else if (array[mid] < array[start]) {
					/*
					 * mid sits in the different half from
					 * start.
					 * Thus val is at the left of mid.
					 */
					end = mid - 1;
				} else {
					/*
					 * Cannot tell whether start and mid
					 * stay in the same half. Just sequeeze
					 * the array little by little.
					 */
					start++;
					end--;
				}
			} else {
				end = mid - 1;
			}

			continue;
		}

		/*
		 * Keep cutting off the head until val locates into the same
		 * part of start
		 */
		if (val < array[start]) {
			if (array[mid] < val) {
				start = mid + 1;
			} else {
				if (array[mid] > array[start]) {
					/*
					 * mid sits in the same half as start.
					 * Thus val is at the right of mid.
					 */
					start = mid + 1;
				} else if (array[mid] < array[start]) {
					/*
					 * mid sits in the different half from
					 * start.
					 * Thus val is at the left of mid.
					 */
					end = mid - 1;
				} else {
					/*
					 * Cannot tell whether start and mid
					 * stay in the same half. Just sequeeze
					 * the array little by little.
					 */
					start++;
					end--;
				}
			}

			continue;
		}

		/*
		 * start, mid and end sits in the same half of the circular
		 * The same as normal binary search
		 */
		if (array[mid] < val)
			start = mid + 1;
		else
			end = mid - 1;
	}

	return -1;
}

int main(void)
{
	int32_t *array;
	int nr, k, test_data, idx, i, round = 0x10;
	int ret = 0;

	srand(time(NULL));

//	for (i = 0; i < round; i++) {
	while (1) {
		array = generate_sort_array(&nr);
		if (!array)
			return -ENOMEM;

		k = rand() % nr;
		test_data = array[k];

		idx = binary_search_first(array, nr, test_data);

		if (test_binary_search_first(array, nr, test_data, idx)) {
			printf("\nSucceed to find the first match data %d in idx %d\n",
			       test_data, idx);
		} else {
			printf("\nFail to find the first match data %d in idx %d\n",
			       test_data, k);
			ret = -1;
			goto free_array;
		}

		k = rand() % nr;
		test_data = array[k];

		idx = binary_search_last(array, nr, test_data);

		if (test_binary_search_last(array, nr, test_data, idx)) {
			printf("\nSucceed to find the last match data %d in idx %d\n",
			       test_data, idx);
		} else {
			printf("\nFail to find the last match data %d in idx %d\n",
			       test_data, k);
			ret = -1;
			goto free_array;
		}

		k = rand() % nr;
		test_data = array[k];
		--test_data;

		idx = binary_search_large_equal(array, nr, test_data);

		if (test_binary_search_large_equal(array, nr, test_data, idx)) {
			if (idx >= 0)
				printf("\nSucceed to find the first larger/equal data %d in array[%d] %d\n",
				       test_data, idx, array[idx]);
			else
				printf("\nSucceed. Not found.\n");
		} else {
			printf("\nFail to find the first larger/equal data %d\n",
			       test_data);
			ret = -1;
			goto free_array;
		}

		k = rand() % nr;
		test_data = array[k];
		test_data++;

		idx = binary_search_small_equal(array, nr, test_data);

		if (test_binary_search_small_equal(array, nr, test_data, idx)) {
			if (idx >= 0)
				printf("\nSucceed to find the last smaller/equal data %d in array[%d] %d\n",
				       test_data, idx, array[idx]);
			else
				printf("\n Succeed. Not found.\n");
		} else {
			printf("\nFail to find the last smaller/equal data %d\n",
			       test_data);
			ret = -1;
			goto free_array;
		}

		create_circular_array(array, nr);

		k = rand() % nr;
		test_data = array[k];

		idx = binary_search_circular(array, nr, test_data);
		if (test_data == array[idx]) {
			printf("\nSucceed to find array[%d] %d\n", idx,
			       array[idx]);
		} else {
			printf("\nFail to find the array[%d] %d. idx %d\n", k,
			       array[k	], idx);
			ret = -1;
			goto free_array;
		}
	}

free_array:
	free(array);

	if (!ret)
		printf("\nSucceeds!\n");
	else
		printf("\nFail!\n");
	
	return ret;
}