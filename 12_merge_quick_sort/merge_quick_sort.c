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

	srand(time(NULL));

	num = rand() % MAX_NR_ELEMENTS + MIN_NR_ELEMENTS;

	array = malloc(num * sizeof(*array));
	if (!array)
		return NULL;

	printf("Generate an array:\t");
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
	int i = start_idx, j, k = 0, mid_idx, p;
	char *tmp;

	mid_idx = (start_idx + end_idx) / 2;
	j = mid_idx + 1;

	tmp = malloc((end_idx - start_idx + 1) * sizeof(*tmp));
	if (!tmp)
		return -ENOMEM;

	while (k <= (end_idx - start_idx)){
		/*
		 * Add an additional condition check to deal with the case that
		 * array[i] is longer than array[j].
		 */
		if ((array[i] <= array[j]) || (j > end_idx)) {
			tmp[k] = array[i];
			i++;
		} else {
			tmp[k] = array[j];
			/*
			 * Add a watch element here to deal with the case that
			 * the second half is shorter than the first half.
			 * So when array[i] is overflow to array[j], the
			 * array[i] will be always larger than array[j] and thus
			 * array[j] is put into tmp[].
			 */
			array[j] = array[end_idx];
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

static void merge_sort(char *array, int nr)
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
		if ((array[i] <= pivot) && (i != large_idx)) {
			tmp = array[large_idx];
			array[large_idx] = array[i];
			array[i] = tmp;

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

static void quick_sort(char *array, int nr)
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

	free(local);
}

int main(void)
{
	char *array;
	int nr;

	array = (char *)generate_array(&nr);
	if (!array)
		return -ENOMEM;

	merge_sort(array, nr);

	quick_sort(array, nr);

	free(array);

	return 0;
}