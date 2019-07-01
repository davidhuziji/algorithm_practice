/*
 * Copyright (c) 2019 Hu Ziji. All rights reserved.
 * Auther: Hu Ziji <davidhuzj@gmail.com>
 */

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NR_LOG			10
#define NR_LOG_ENTRY		100
#define STEP			100
#define MAX_LOG_VAL		0x1000

static void *generate_log(void)
{
	int data, step, i;
	uint32_t *array;

	array = malloc(NR_LOG_ENTRY * sizeof(*array));
	if (!array)
		return NULL;

	data = rand() % MAX_LOG_VAL;
	for (i = 0; i < NR_LOG_ENTRY; i++) {
		step = rand() % STEP;

		if (data + step < data)
			step = 0x0;

		data += step;
		array[i] = data;
	}

	return array;
}

static int __merge_logs(uintptr_t *log_array, int log_idx, int log1_len,
			int log2_len)
{
	uint32_t *log1_ptr, *log2_ptr, *log;
	uintptr_t merge_array[NR_LOG];
	int i, j, k, n, log1_idx, log2_idx;
	int ret;

	if (!log_array)
		return -EINVAL;

	log1_ptr = (uint32_t *)log_array[log_idx + log1_len - 1];
	log2_ptr = (uint32_t *)log_array[log_idx + log1_len];

	if (!log1_ptr || !log2_ptr)
		return -EINVAL;
	/*
	 * The data in the second logs are larger than those in the
	 * first logs
	 */
	if (log1_ptr[NR_LOG_ENTRY - 1] < *log2_ptr)
		return 0;

	log1_ptr = (uint32_t *)log_array[log_idx];
	log2_ptr = (uint32_t *)log_array[log_idx + log1_len + log2_len - 1];

	if (!log1_ptr || !log2_ptr)
		return -EINVAL;
	/*
	 * All the data in the second logs are smaller than those in the first
	 * logs.
	 */
	if (*log1_ptr > log2_ptr[NR_LOG_ENTRY - 1]) {
		/* Switch the first log and second log */
		for (i = log_idx + log1_len; i < log_idx + log1_len + log2_len;
		     i++)
			merge_array[i] = log_array[i];
		for (i = log_idx + log1_len - 1; i >= log_idx; i--)
			log_array[i + log2_len] = log_array[i];
		for (i = log_idx + log1_len; i < log_idx + log1_len + log2_len;
		     i++)
		     	log_array[i - log1_len] = merge_array[i];
		return 0;
	}

	log1_ptr = (uint32_t *)log_array[log_idx];
	log2_ptr = (uint32_t *)log_array[log_idx + log1_len];
	if (!log1_ptr)
		return -EINVAL;

	for (i = 0; i < log1_len + log2_len; i++)
		merge_array[i] = (uintptr_t)NULL;

	i = 0;
	j = 0;
	k = 0;
	n = 0;
	log1_idx = 0;
	log2_idx = 0;
	while (1) {
		if ((k % NR_LOG_ENTRY) == 0) {
			log = malloc(NR_LOG_ENTRY * sizeof(*log));
			if (!log) {
				ret = -ENOMEM;
				goto free_merge_log;
			}

			merge_array[n] = (uintptr_t)log;
			n++;
			k = 0;
		}

		if ((log1_idx < log1_len) &&
		    ((log2_idx >= log2_len) || (log1_ptr[i] <= log2_ptr[j]))) {
			log[k] = log1_ptr[i];
			i++;

			if ((i % NR_LOG_ENTRY) == 0) {
				log1_idx++;
				/* Switch to next log */
				log1_ptr = (uint32_t *)log_array[log_idx +
								 log1_idx];
				i = 0;
			}
		} else {
			log[k] = log2_ptr[j];
			j++;

			if ((j % NR_LOG_ENTRY) == 0) {
				log2_idx++;
				/* Switch to next log */
				log2_ptr = (uint32_t *)log_array[log_idx +
								 log1_len +
								 log2_idx];
				j = 0;
			}
		}

		k++;
		if ((log1_idx >= log1_len) && (log2_idx >= log2_len))
			break;
	}

replace_log_ptr:
	for (i = 0; i < log1_len + log2_len; i++) {
		free((void *)log_array[log_idx + i]);
		log_array[log_idx + i] = merge_array[i];
	}

	return 0;

free_merge_log:
	for (i = 0; i < log1_len + log2_len; i++)
		free((void *)merge_array[i]);

	return ret;
}

static void merge_sort_logs(uintptr_t *log_array)
{
	int log_len = 1, log_idx, ret;

	if (!log_array)
		return;

	while (log_len <= NR_LOG) {
		for (log_idx = 0; (log_idx + log_len * 2) <= NR_LOG;
		     log_idx += log_len * 2) {
			ret = __merge_logs(log_array, log_idx, log_len,
					   log_len);
			if (ret)
				return;
		}

		log_len <<= 1;
	}

	log_len >>= 1;

	if (NR_LOG % log_len)
		__merge_logs(log_array, 0, log_len, NR_LOG % log_len);
}

static void check_sort_result(uintptr_t *log_array)
{
	uint32_t prev_val = 0, *log;
	bool success = true;
	int i, j;

	printf("\nAfter sort:\n");
	for (i = 0; i < NR_LOG; i++) {
		log = (uint32_t *)log_array[i];
		printf("Log %d:\t", i);
		for (j = 0; j < NR_LOG_ENTRY; j++) {
			if (log[j] < prev_val)
				success = false;

			printf("%d ", log[j]);
			prev_val = log[j];
		}
		printf("\n");
	}

	if (success)
		printf("Sort successfully\n");
	else
		printf("Fail to sort\n");
}

int main(void)
{
	uintptr_t log_array[NR_LOG];
	uint32_t *log;
	int i, j;

	srand(time(NULL));

	for (i = 0; i < NR_LOG; i++) {
		log = generate_log();
		log_array[i] = (uintptr_t)log;
		if (!log_array[i])
			goto free_log;

		printf("Log %d:\t", i);
		for (j = 0; j < NR_LOG_ENTRY; j++)
			printf("%d ", log[j]);
		printf("\n");
	}

	merge_sort_logs(log_array);

	check_sort_result(log_array);

free_log:
	for (i = 0; i < NR_LOG; i++)
		free((void *)log_array[i]);

	return 0;
}