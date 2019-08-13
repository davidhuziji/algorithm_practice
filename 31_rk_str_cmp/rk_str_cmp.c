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
#include <math.h>

#define MAIN_LEN		0x20
#define PATTEN_LEN		10

#define TEST_ROUND		0x30
#define HASH_FACTOR		3

static int hash_cal(const char *str, size_t len)
{
	int res = 0, i;

	for (i = 0; i < len; i++) {
		res *= HASH_FACTOR;
		res += str[i] - 'A';
	}

	return res;
}

static int bk_search(const char *main_str, size_t main_len,
		     const char *pat_str, size_t pat_len)
{
	int pat_hash, main_hash, i, j;

	pat_hash = hash_cal(pat_str, pat_len);
	main_hash = hash_cal(main_str, pat_len);
	if (pat_hash == main_hash) {
		/*
		 * Compare the pattern and main strings char by char
		 * to avoid hash table conflict.
		 */
		for (j = 0; j < pat_len; j++) {
			if (main_str[j] != pat_str[j])
				break;
		}
		if (j == pat_len)
			return 0;
	}

	for (i = 1; i < main_len - pat_len; i++) {
		main_hash -= (main_str[i - 1] - 'A') *
			     pow(HASH_FACTOR, pat_len - 1);
		main_hash *= HASH_FACTOR;
		main_hash += main_str[i + pat_len - 1] - 'A';

		if (main_hash == pat_hash) {
			/*
			 * Compare the pattern and main strings char by char
			 * to avoid hash table conflict.
			 */
			for (j = 0; j < pat_len; j++) {
				if (main_str[j + i] != pat_str[j])
					break;
			}
			if (j == pat_len)
				return i;
		}
	}

	printf("Fail to find substring\n");
	return -1;
}

void main(void)
{
	int i, test_idx, start_idx, res_idx;
	char ch;
	char *pattern, *main_str;

	srand(time(NULL));

	pattern = malloc(sizeof(*pattern) * (PATTEN_LEN + 1));
	for (i = 0; i < PATTEN_LEN; i++) {
		ch = rand() % ('z' - 'a');
		pattern[i] = ch + 'A';
	}
	pattern[i] = '\0';
	printf("pattern string %s\n", pattern);

	for (test_idx = 0; test_idx < TEST_ROUND; test_idx++) {
		main_str = malloc(sizeof(*main_str) * (MAIN_LEN + 1));
		start_idx = rand() % (MAIN_LEN - PATTEN_LEN);
		for (i = 0; i < start_idx; i++) {
			ch = rand() % ('Z' - 'A');
			main_str[i] = ch + 'a';
		}
		for (i = start_idx; i < start_idx + PATTEN_LEN; i++)
			main_str[i] = pattern[i - start_idx];
		for (i = start_idx + PATTEN_LEN; i < MAIN_LEN; i++) {
			ch = rand() % ('Z' - 'A');
			main_str[i] = ch + 'a';
		}
		main_str[MAIN_LEN] = '\0';

		printf("Round %d: Start idx %d\n", test_idx, start_idx);
		printf("main string %s\n", main_str);

		res_idx = bk_search(main_str, MAIN_LEN, pattern, PATTEN_LEN);
		if (res_idx == start_idx)
			printf("Success\n");
		else
			printf("Fail: result idx %d\n", res_idx);

		free(main_str);
	}

	free(pattern);
}