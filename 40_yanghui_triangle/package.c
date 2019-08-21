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

const int yanghui_array[] = {5, 7, 8, 2, 3, 4, 4, 9, 6, 1, 2, 7, 9, 4, 5};
const int yanghui_layer = 5;
int min_path;

static void __yanghui_back_trace(int val, int idx, int layer)
{
	int prev_nr, nr;

	printf("Go through node %d at layer %d\n", idx, layer);
	val += yanghui_array[idx];

	/* Reach tho bottom layer */
	if ((layer + 1) == yanghui_layer) {
		if (val < min_path)
			min_path = val;

		return;
	}

	prev_nr = layer * (layer + 1) / 2;
	nr = prev_nr + layer + 1;

	/* Continue to left */
	__yanghui_back_trace(val, nr + (idx - prev_nr), layer + 1);
	/* Continue to right */
	__yanghui_back_trace(val, nr + (idx - prev_nr) + 1, layer + 1);

}

static void yanghui_triangle_back_trace(void)
{
	int path_val = yanghui_array[0];

	min_path = INT_MAX;

	/* Go to left */
	__yanghui_back_trace(path_val, 1, 1);
	/* Go to right */
	__yanghui_back_trace(path_val, 2, 1);
}

static void __yanghui_triagnle_dp(int *path, int prev_nr, int layer,
				  int idx)
{
	int left_up_val, right_up_val;

	printf("Go through node %d at layer %d\n", idx, layer);

	if (idx == prev_nr)
		/* No left up node */
		left_up_val = INT_MAX;
	else
		left_up_val = path[idx - layer - 1];

	if (idx == (prev_nr + layer))
		/* No right up node */
		right_up_val = INT_MAX;
	else
		right_up_val = path[idx - layer];

	path[idx] = yanghui_array[idx] +
		    ((right_up_val < left_up_val) ? right_up_val : left_up_val);
	if ((layer + 1) == yanghui_layer) {
		if (path[idx] < min_path)
			min_path = path[idx];
	}
}

static void yanghui_triagnle_dp(void)
{
	int nr, i, j, prev_nr;
	int *path;

	min_path = INT_MAX;
	nr = sizeof(yanghui_array) / sizeof(*yanghui_array);

	path = malloc(sizeof(*path) * nr);
	if (!path)
		return;

	path[0] = yanghui_array[0];
	for (i = 1; i < nr; i++)
		path[i] = INT_MAX;

	for (i = 1; i < yanghui_layer; i++) {
		prev_nr = i * (i + 1) / 2;
		for (j = 0; j <= i; j++)
			__yanghui_triagnle_dp(path, prev_nr, i, prev_nr + j);
	}

	free(path);
}

int main(void)
{
	printf("\nYanghui triangle back trace: \n");
	yanghui_triangle_back_trace();
	printf("The min path value %d\n", min_path);

	printf("\nYanghui triangle dynamic programming: \n");
	yanghui_triagnle_dp();
	printf("The min path value %d\n", min_path);

	return 0;
}
