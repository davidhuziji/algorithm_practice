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

#define MAIN_LEN_BASE		0x40
#define MAIN_LEN_RAND		0x20
#define PATTEN_LEN		0x20

#define BAD_CH_MAP_LEN		('z' - 'a' + 1)

#define TEST_ROUND		0x20
#define DEBUG			0

static void bad_ch_map_create(int *map, int size, const char *patten,
			       size_t pat_len)
{
	int i;
	char chidx;

	for (i = 0; i < size; i++)
		map[i] = -1;

	/*
	 * The key point is that: when moving the patten string, the moving
	 * step is the distance between the last smae char and last char.
	 */
	for (i = 0; i < pat_len; i++) {
		chidx = patten[i] - 'a';
		map[chidx] = i;
	}

#if DEBUG
	printf("bad char map array: ");
	for (i = 0; i < BAD_CH_MAP_LEN; i++)
		printf("%d ", map[i]);
	printf("\n");
#endif
}

static void suffix_map_create(int *suffix_map, bool *prefix_map,
			      const char *patten, size_t pat_len)
{
	int i, j, k;

	for (i = 0; i < pat_len; i++) {
		suffix_map[i] = -1;
		prefix_map[i] = false;
	}

	for (i = 0; i < pat_len - 2; i++) {
		k = 0;
		j = i;
		while (j >= 0) {
			if (patten[j] == patten[pat_len - k - 1])
				/* The suffix map arrary starts from index = 1*/
				suffix_map[k + 1] = j;
			else
				break;

			j--;
			k++;
		}

		if (j == -1)
			prefix_map[k + 1] = true;
	}

#if DEBUG
	printf("suffix array: ");
	for (i = 0; i < pat_len; i++)
		printf("%d ", suffix_map[i]);
	printf("\n");
	printf("prefix array: ");
	for (i = 0; i < pat_len; i++)
		printf("%d ", prefix_map[i]);
	printf("\n");
#endif
}

static int get_bad_step(const int *map, char ch, int bad_pos)
{
	int pos;

	pos = map[ch - 'a'];

#if DEBUG
	printf("bad char %c pos %d bad step %d\n", ch, pos, bad_pos - pos);
#endif

	return bad_pos - pos;
}

static int get_good_str_step(int *suffix, bool *prefix, int len, int bad_pos)
{
	int suffix_idx, i;

	if (bad_pos = (len - 1))
		return 1;

	/* Search if duplicated end stirng exists or sub-string existis */
	suffix_idx = suffix[len - bad_pos - 1];
	if (suffix_idx != -1) {
#if DEBUG
		printf("good string start from %d. good step %d\n", suffix_idx,
		       bad_pos - suffix_idx + 1);
#endif
		return bad_pos - suffix_idx + 1;
	}

	for (i = len - bad_pos - 2; i > 0; i--) {
		if (prefix[i]) {
#if DEBUG
			printf("good substring start from %d. good step %d\n",
			       suffix_idx, len - i);
#endif
			return len - i;
		}
	}

#if DEBUG
	printf("no good string. good step %d\n", len);
#endif
	return len;
}

static int bm_search(const char *main_str, size_t main_len,
		     const char *pat_str, size_t pat_len)
{
	int i = 0, j, good_step, bad_step, *bad_ch_map, *suffix_map;
	bool *prefix_map;

	/* Create the bad char mapping array */
	bad_ch_map = malloc(sizeof(*bad_ch_map) * BAD_CH_MAP_LEN);
	bad_ch_map_create(bad_ch_map, BAD_CH_MAP_LEN, pat_str, pat_len);

	/* Create the index of duplicated suffix */
	suffix_map = malloc(sizeof(*suffix_map) * pat_len);
	prefix_map = malloc(sizeof(*prefix_map) * pat_len);
	suffix_map_create(suffix_map, prefix_map, pat_str, pat_len);

	while (i <= main_len - pat_len) {
		for (j = pat_len - 1; j >= 0; j--) {
#if DEBUG
			printf("main idx %d char %c\n", i + j, main_str[i + j]);
			printf("patten idx %d char %c\n", j, pat_str[j]);
#endif
			if (pat_str[j] != main_str[i + j])
				break;
		}
		if (j < 0)
			return i;

		bad_step = get_bad_step(bad_ch_map, main_str[i + j], j);
		/* If the last char doesn't match, good string cannot work? */
		good_step = get_good_str_step(suffix_map, prefix_map,
					      pat_len, j);

		i += (bad_step > good_step) ? bad_step : good_step;
	}

	printf("Fail to find substring\n");
	return -1;
}

void main(void)
{
	int i, test_idx, start_idx, res_idx, main_len;
	char ch;
	char *pattern, *main_str;

	srand(time(NULL));

	pattern = malloc(sizeof(*pattern) * (PATTEN_LEN + 1));
	for (i = 0; i < PATTEN_LEN; i++) {
		ch = rand() % ('z' - 'a');
		pattern[i] = ch + 'a';
	}
	pattern[i] = '\0';
	printf("pattern string %s\n", pattern);

	for (test_idx = 0; test_idx < TEST_ROUND; test_idx++) {
		main_len = MAIN_LEN_BASE + rand() % MAIN_LEN_RAND;
		main_str = malloc(sizeof(*main_str) * (main_len + 1));
		start_idx = rand() % (main_len - PATTEN_LEN);
		for (i = 0; i < start_idx; i++) {
			ch = rand() % ('z' - 'a');
			main_str[i] = ch + 'a';
		}
		for (i = start_idx; i < start_idx + PATTEN_LEN; i++)
			main_str[i] = pattern[i - start_idx];
		for (i = start_idx + PATTEN_LEN; i < main_len; i++) {
			ch = rand() % ('z' - 'a');
			main_str[i] = ch + 'a';
		}
		main_str[main_len] = '\0';

		printf("Round %d: Start idx %d\n", test_idx, start_idx);
		printf("main string %s\n", main_str);

		res_idx = bm_search(main_str, main_len, pattern, PATTEN_LEN);
		if (res_idx == start_idx) {
			printf("Success\n");
		} else {
			for (i = 0; i < PATTEN_LEN; i++) {
				if (pattern[i] != main_str[i + res_idx])
					break;
			}
			if (i == PATTEN_LEN) {
				printf("Success");
			} else {
				printf("Fail: result idx %d\n", res_idx);
			}
		}

		free(main_str);
	}

	free(pattern);
}