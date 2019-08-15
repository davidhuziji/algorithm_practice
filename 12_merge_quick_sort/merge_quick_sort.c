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

#define MAX_NR_ELEMENTS		0x100
#define MIN_NR_ELEMENTS		0x50
#define BASE_VALUE		0x50

static void *generate_array(int *nr)
{
	int num, i;
	char *array;

	if (!nr)
		return NULL;

	num = rand() % MAX_NR_ELEMENTS + MIN_NR_ELEMENTS;

	array = malloc(num * sizeof(*array));
	if (!array)
		return NULL;

	printf("\nGenerate an array length %d:\n", num);
	for (i = 0; i < num; i++) {
		array[i] = rand() % BASE_VALUE;
		printf("%d ", array[i]);
	}
	printf("\n\n");

	*nr = num;
	return array;
}

static int __merge_parts(char *array, int start_idx, int end_idx)
{
	int i = start_idx, j, k = 0, n, mid_idx, p;
	char *tmp;

	mid_idx = (start_idx + end_idx) / 2;
	j = mid_idx + 1;

	tmp = malloc((end_idx - start_idx + 1) * sizeof(*tmp));
	if (!tmp)
		return -ENOMEM;

	while (k < (end_idx - start_idx + 1)){
		/*
		 * Add an additional condition check to deal with the case that
		 * array[i] is longer than array[j].
		 */
		if ((i <= mid_idx) &&
		    ((j > end_idx) || (array[i] <= array[j]))) {
			tmp[k] = array[i];
			i++;
		} else {
			tmp[k] = array[j];
#if 0
			/*
			 * Add a watch element here to deal with the case that
			 * the second half is shorter than the first half.
			 * So when array[i] is overflow to array[j], the
			 * array[i] will be always larger than array[j] and thus
			 * array[j] is put into tmp[].
			 */
			array[j] = array[end_idx];
#endif
			j++;
		}
		k++;
	}

	memcpy(array + start_idx, tmp,
	       (end_idx - start_idx + 1) * sizeof(*tmp));

	free(tmp);

	return 0;
}

static int __merge_sort(char *array, int start_idx, int end_idx)
{
	int mid_idx;
	int ret;

	if (start_idx >= end_idx)
		return 0;

	mid_idx = (start_idx + end_idx) / 2;

	ret = __merge_sort(array, start_idx, mid_idx);
	if (ret)
		return ret;

	ret = __merge_sort(array, mid_idx + 1, end_idx);
	if (ret)
		return ret;

	ret = __merge_parts(array, start_idx, end_idx);

	return ret;
}

static void merge_sort(char *array, int nr, int k)
{
	char *local;
	int i;

	if (!array || (nr == 0))
		return;

	local = malloc(nr * sizeof(*local));
	if (!local)
		return;

	memcpy(local, array, nr);

	__merge_sort(local, 0, nr - 1);

	printf("\nAfter merge sort:\t");
	for (i = 0; i < nr; i++)
		printf("%d ", local[i]);
	printf("\n");
	printf("The largest %d element is %d\n", k + 1, local[k]);


	free(local);
}

/*
 * Optimized algorithm as suggested in the training.
 * However, in the original algorithm, if the last element is the large one in
 * the part, every element has to be exchanged and it will be worse case.
 * Thus make a little further optimization:
 * 1. Compare the first element and the last element. Take the smaller one as
 * the pivot.
 * 2. Only exchange of current index doesn't equal to large anchor.
 */
static int __quick_partition(char *array, int start_idx, int end_idx)
{
	int pivot = array[end_idx], large_idx = start_idx, i, tmp;

	if (array[start_idx] < pivot) {
		array[end_idx] = array[start_idx];
		array[start_idx] = pivot;
		pivot = array[end_idx];
	}

	for (i = start_idx; i < end_idx; i++) {
		/*
		 * Exchange the smaller element with the larger element anchor
		 */
		if (array[i] < pivot) {
			if (i != large_idx) {
				tmp = array[large_idx];
				array[large_idx] = array[i];
				array[i] = tmp;
			}

			large_idx++;
		}
	}

	array[end_idx] = array[large_idx];
	array[large_idx] = pivot;

	return large_idx;
}

static int __quick_sort(char *array, int start_idx, int end_idx)
{
	int pivot_idx;
	int ret;

	if (start_idx >= end_idx)
		return 0;

	pivot_idx = __quick_partition(array, start_idx, end_idx);

	ret = __quick_sort(array, start_idx, pivot_idx - 1);
	if (ret)
		return ret;

	ret = __quick_sort(array, pivot_idx + 1, end_idx);

	return ret;
}

static void quick_sort(char *array, int nr, int k)
{
	char *local;
	int i;

	if (!array || (nr == 0))
		return;

	local = malloc(nr * sizeof(*local));
	if (!local)
		return;

	memcpy(local, array, nr);

	__quick_sort(local, 0, nr - 1);

	printf("\nAfter quick sort:\t");
	for (i = 0; i < nr; i++)
		printf("%d ", local[i]);
	printf("\n");
	printf("The largest %d element is %d\n", k + 1, local[k]);

	free(local);
}

static void partition_k_large(char *array, int start_idx, int end_idx, int k)
{
	int pivot_idx;

	if (start_idx >= end_idx) {
		if (start_idx == k)
			/*
			 * Special case when the largest K element is in a
			 * partition with a single element
			 */
			printf("\nThe largest %d data is %d\n", k + 1,
			       array[start_idx]);
		return;
	}

	pivot_idx = __quick_partition(array, start_idx, end_idx);
	if (pivot_idx == k) {
		printf("\nThe largest %d data is %d\n", k + 1,
		       array[pivot_idx]);
		return;
	}

	if (k < pivot_idx)
		partition_k_large(array, start_idx, pivot_idx - 1, k);
	else
		partition_k_large(array, pivot_idx + 1, end_idx, k);
}

static void search_k_large(char *array, int nr, int k)
{
	char *local;

	if (!array || (nr == 0))
		return;

	local = malloc(nr * sizeof(*local));
	if (!local)
		return;

	memcpy(local, array, nr);

	partition_k_large(local, 0, nr - 1, k);

	free(local);
}

int main(void)
{
	char *array;
	int i, nr, k, round = 0x5;

	srand(time(NULL));

	for (i = 0; i < round; i++) {
		array = (char *)generate_array(&nr);
		if (!array)
			return -ENOMEM;

		k = rand() % nr;

		merge_sort(array, nr, k);

		quick_sort(array, nr, k);

		search_k_large(array, nr, k);

		free(array);
	}

	return 0;
}