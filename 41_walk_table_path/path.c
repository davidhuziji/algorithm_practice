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

const int matrix[] = {1, 3, 5, 9,
                      2, 1, 3, 4,
		      5, 2, 6, 7,
		      6, 8, 4, 3};

const int matrix_side_len = 4;
int min_path;

static void __walk_table_back_trace(int val, int idx, int nr)
{
	int prev_nr;

	printf("Go through node row %d col %d val %d\n", idx / matrix_side_len,
	       idx % matrix_side_len, matrix[idx]);
	val += matrix[idx];

	/* Reach tho bottom layer */
	if (idx == nr - 1) {
		printf("path weight %d\n", val);
		if (val < min_path)
			min_path = val;

		return;
	}

	if ((idx + 1) % matrix_side_len)
		/* Continue to right */
		__walk_table_back_trace(val, idx + 1, nr);

	if ((idx + matrix_side_len) < nr)
		/* Continue to down */
		__walk_table_back_trace(val, idx + matrix_side_len, nr);

}

static void walk_table_back_trace(void)
{
	int path_val = matrix[0];
	int nr = matrix_side_len * matrix_side_len;

	min_path = INT_MAX;

	/* Go right */
	__walk_table_back_trace(path_val, 1, nr);
	/* Go down */
	__walk_table_back_trace(path_val, matrix_side_len, nr);
}

static void __walk_table_dp(int *path, int row, int col)
{
	int up_val, left_val, idx  = row * matrix_side_len + col;

	printf("Go through node row %d col %d val %d\n", row, col, matrix[idx]);

	if (idx == 0)
		return;

	if (row == 0)
		/* No up node */
		up_val = INT_MAX;
	else
		up_val = path[(row - 1) * matrix_side_len + col];

	if (col == 0)
		/* No left node */
		left_val = INT_MAX;
	else
		left_val = path[row * matrix_side_len + col - 1];

	path[idx] = matrix[idx] + ((left_val < up_val) ? left_val : up_val);
}

static void walk_table_dp(void)
{
	int nr, i, j, prev_nr;
	int *path;

	min_path = INT_MAX;
	nr = matrix_side_len * matrix_side_len;

	path = malloc(sizeof(*path) * nr);
	if (!path)
		return;

	path[0] = matrix[0];
	for (i = 1; i < nr; i++)
		path[i] = INT_MAX;

	for (i = 0; i < matrix_side_len; i++) {
		for (j = 0; j < matrix_side_len; j++) {
			__walk_table_dp(path, i, j);
		}
	}

	min_path = path[nr - 1];
	free(path);
}

int main(void)
{
	printf("\nWalk through the table by back-tracing: \n");
	walk_table_back_trace();
	printf("The min path value %d\n", min_path);

	printf("\nWalk through the table by dynamic programming: \n");
	walk_table_dp();
	printf("The min path value %d\n", min_path);

	return 0;
}
