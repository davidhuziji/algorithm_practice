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

const int weights[] = {2, 2, 4, 6, 3};
const int weight_limit = 9;
int weight_max;

static void __back_trace(int weight, int nr, int step, bool *check)
{
	printf("weight %d at step %d\n", weight, step);

	if ((step == nr) || (weight == weight_limit)) {
		if (weight > weight_max)
			weight_max = weight;

		return;
	}

	if (check[step * nr + weight] == true)
		return;

	check[step * nr + weight] = true;

	/* Do not pick this item */
	__back_trace(weight, nr, step + 1, check);

	/* pick this item */
	if ((weight + weights[step]) <= weight_limit)
		__back_trace(weight + weights[step], nr, step + 1, check);
}

static void back_trace(void)
{
	int nr = sizeof(weights) / sizeof(*weights);
	int weight = 0;
	bool *check; 

	weight_max = INT_MIN;

	check = malloc(sizeof(*check) * nr * (weight_limit + 1));
	if (!check)
		return;

	memset(check, sizeof(check), false);

	__back_trace(weight, nr, 0, check);

	free(check);
}

static void dp_package(void)
{
	int nr = sizeof(weights) / sizeof(*weights);
	int weight = 0, i, j, idx;
	bool *fill_table;

	weight_max = INT_MIN;

	fill_table = malloc(sizeof(*fill_table) * nr * (weight_limit + 1));
	if (!fill_table)
		return;

	memset(fill_table, sizeof(fill_table), false);

	/* Special init of the first item */
	fill_table[0] = true;
	if (weights[0] < weight_limit)
		fill_table[weights[0]] = true;

	for (i = 1; i < nr; i++) {
		for (j = 0; j <= weight_limit - weights[i]; j++) {
			if (fill_table[(i - 1) * (weight_limit + 1) + j]) {
				idx = i * (weight_limit + 1) + j + weights[i];
				printf("Fill idx %d weight %d\n", i, j + weights[i]);
				fill_table[idx] = true;
			}
		}

		for (j = 0; j <= weight_limit; j++) {
			if (fill_table[(i - 1) * (weight_limit + 1) + j]) {
				idx = i * (weight_limit + 1) + j;
				printf("Fill idx %d weight %d\n", i, j);
				fill_table[idx] = true;
			}
		}
	}

	idx = (nr - 1) * (weight_limit + 1);
	for (j = weight_limit; j >= 0; j--) {
		if (fill_table[idx + j]) {
			weight_max = j;
			goto dp_return;
		}
	}

dp_return:
	free(fill_table);
	return;
}

static void opt_dp_package(void)
{
	int nr = sizeof(weights) / sizeof(*weights);
	int weight = 0, i, j, idx;
	bool *fill_table;

	weight_max = INT_MIN;

	fill_table = malloc(sizeof(*fill_table) * (weight_limit + 1));
	if (!fill_table)
		return;

	memset(fill_table, sizeof(fill_table), false);

	/* Special init of the first item */
	fill_table[0] = true;
	if (weights[0] < weight_limit)
		fill_table[weights[0]] = true;

	for (i = 1; i < nr; i++) {
		for (j = weight_limit - weights[i]; j >= 0; j--) {
			if (fill_table[j]) {
				printf("Fill weight %d\n", j + weights[i]);
				fill_table[j + weights[i]] = true;
			}
		}
	}

	for (j = weight_limit; j >= 0; j--) {
		if (fill_table[j]) {
			weight_max = j;
			goto dp_return;
		}
	}

dp_return:
	free(fill_table);
	return;
}

int main(void)
{
	printf("Back trace: \n");
	back_trace();
	printf("Back trace max weight limit %d\n", weight_max);

	printf("\nDynamic programming: \n");
	dp_package();
	printf("DP max weight limit %d\n", weight_max);

	printf("\nOptimized dynamic programming: \n");
	opt_dp_package();
	printf("Optimized DP max weight limit %d\n", weight_max);

	return 0;
}
