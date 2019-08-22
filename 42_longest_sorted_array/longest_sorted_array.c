/*
 * Copyright (c) 2019 Hu Ziji. All rights reserved.
 *
 * Auther: Hu Ziji <davidhuzj@gmail.com>
 */

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int array[] = {2, 9, 3, 6, 5, 1, 8, 6, 7};
int max_len;

static void __sub_array_len_bt(int idx, int last_idx, int sub_len, int len)
{
	if (idx == len) {
		if (sub_len > max_len)
			max_len = sub_len;

		return;
	}

	printf("check array[%d] %d\n", idx, array[idx]);

	if ((last_idx < 0) || (array[idx] >= array[last_idx])) {
		/* Not pick current element */
		__sub_array_len_bt(idx + 1, last_idx, sub_len, len);
		/* Pick the current element */
		__sub_array_len_bt(idx + 1, idx, sub_len + 1, len);
	} else {
		/* Cannot pick current element */
		__sub_array_len_bt(idx + 1, last_idx, sub_len, len);
	}
}

static void sub_array_len_bt(void)
{
	int len;
	int *sub_array;

	len = sizeof(array) / sizeof(*array);
	max_len = INT_MIN;

	/* Not pick the first element */
	__sub_array_len_bt(1, -1, 0, len);
	/* Pick the first element */
	__sub_array_len_bt(1, 0, 1, len);
}

static void __sub_array_len_dp(int *len_matrix, int *max_len_array, int idx,
			       int last_idx, int len)
{
	if (array[last_idx] <= array[idx])
		/*
		 * If current element is larger than/equals to the "last"
		 * element of current sub array, the length + 1;
		 */
		len_matrix[idx * len + last_idx] = max_len_array[last_idx] + 1;
	else
		/*
		 * If current element is smaller than the "last" element of
		 * current sub array, the length keeps the same;
		 */
		len_matrix[idx * len + last_idx] = max_len_array[last_idx];
}

static void sub_array_len_dp(void)
{
	int len, i, j, curr_max_len;
	int *len_matrix, *max_len_array;

	len = sizeof(array) / sizeof(*array);
	max_len = INT_MIN;

	len_matrix = malloc(sizeof(*len_matrix) * len * len);
	if (!len_matrix)
		return;
	len_matrix[0] = 1;

	max_len_array = malloc(sizeof(*max_len_array) * len);
	if (!max_len_array)
		goto free_matrix;
	max_len_array[0] = 1;

	for (i = 1; i < len; i++) {
		curr_max_len = INT_MIN;
		for (j = 0; j < i; j++) {
			/*
			 * Update the length if j is added and i is the last
			 * one in previous sub sorted array.
			 */
			__sub_array_len_dp(len_matrix, max_len_array, i, j,
					   len);
			if (len_matrix[i * len + j] > curr_max_len)
				curr_max_len = len_matrix[i * len + j];
		}
		max_len_array[i] = curr_max_len;
	}

	/*
	 * The max length of sub sorted array is the max value of the length
	 * in current index.
	 */
	max_len = max_len_array[len - 1];

	free(max_len_array);
free_matrix:
	free(len_matrix);
}

int main(void)
{
	printf("\nBack-tracing: The longest sub sorted array: \n");
	sub_array_len_bt();
	printf("The max lenght of sub sorted array %d\n", max_len);

	printf("\nDynamic-Programming: The longest sub sorted array: \n");
	sub_array_len_dp();
	printf("The max lenght of sub sorted array %d\n", max_len);

	return 0;
}
