/*
 * Copyright @ 2019 Hu Ziji. All rights reserved.
 * Auther: Hu Ziji <davidhuzj@gmail.com>
 */

/* Practise using recursion to solve eight queens puzzle */

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_NR_QUEENS		16
#define MAX_NR_LEN		4

static int input_nr_queens(void)
{
	char nr_input[MAX_NR_LEN];
	int nr_queens = 0, actual_len = 0;
	char *ch;

	while (1) {
		printf("Please input number of queens.\n");
		printf("The number should be less than %d.\n",
		       MAX_NR_QUEENS);

		while (!fgets(nr_input, MAX_NR_LEN, stdin))
			printf("Cannot recoginze the input. Please try again.\n");

		ch = nr_input;
		while ((*ch != '\n') && (*ch != '\0')) {
			if ((*ch < '0') || (*ch > '9')) {
				printf("Invalid numberic character %c!\n", *ch);
				return 0;
			}

			nr_queens *= 10;
			nr_queens += (*ch - 48);
			ch++;
		}

		if (nr_queens >= MAX_NR_QUEENS)
			printf("The number %d is too large.\n", nr_queens);

		break;
	}

	printf("Input number: %d\n", nr_queens);
	return nr_queens;
}

static void fill_table(char *row, char *col, int row_idx, int col_idx)
{
	int byte_idx, bit_idx;

	if (!row || !col)
		return;

	row[row_idx] = col_idx;
	col[col_idx] = 1;
}

static void clear_table(char *row, char *col, int row_idx, int col_idx)
{
	if (!col || !row)
		return;

	row[row_idx] = -1;
	col[col_idx] = 0;
}

/* Check if the queens position follows rules after setting setting the spot */
static bool is_valid(int nr_queens, char *row, char *col, int row_idx,
		     int col_idx)
{
	int i, col_left = col_idx, col_right = col_idx;

	if (col[col_idx])
		return false;

	for (i = row_idx - 1; i >= 0; i--) {
		if (col_left > 0) {
			col_left--;
			if (row[i] == col_left)
				return false;
		}
		if (col_right < (nr_queens - 1)) {
			col_right++;
			if (row[i] == col_right)
				return false;
		}
	}

	return true;
}

static void print_table(int nr, char *row)
{
	int row_idx, col_idx;

	printf(" |");
	for (col_idx = 0; col_idx < nr; col_idx++)
		printf("%x|", (unsigned int)col_idx);
	printf("\n");

	for (row_idx = 0; row_idx < nr; row_idx++) {
		printf("%x|", (unsigned int)row_idx);
		for (col_idx = 0; col_idx < nr; col_idx++) {
			if (row[row_idx] == col_idx)
				printf("*|");
			else
				printf(" |");		
		}
		printf("\n");
	}
}

static int nr_valid_res = 0x0;

static bool search_queens(int nr, char *row, char *col, int row_idx)
{
	int i;
	bool res = false;

	if ((nr == 0) || !row || !col || (row_idx > nr))
		return false;

	if (row_idx == (nr - 1)) {
		for (i = 0; i < nr; i++) {
			if (is_valid(nr, row, col, row_idx, i)) {
				nr_valid_res++;
				printf("Found %d valid result.\n",
				       nr_valid_res);
				res = true;
				fill_table(row, col, row_idx, i);
				print_table(nr, row);
				clear_table(row, col, row_idx, i);
			}
		}

		return res;
	}

	for (i = 0; i < nr; i++) {
		if (!is_valid(nr, row, col, row_idx, i))
			continue;

		fill_table(row, col, row_idx, i);
		if (search_queens(nr, row, col, row_idx + 1))
			res = true;
		clear_table(row, col, row_idx, i);
	}

	return res;
}

int main(void)
{
	int nr_queens, i;
	char *row, *col;
	int ret = 0;

	while (1) {
		nr_queens = input_nr_queens();
		if ((nr_queens <= 0) || (nr_queens > MAX_NR_QUEENS))
			printf("Incorrect number\n");

		break;
	}

	row = malloc(nr_queens * sizeof(*row));
	if (!row) {
		printf("Cannot allocate row arrary\n");
		return -ENOMEM;
	}

	for (i = 0; i < nr_queens; i++)
		row[i] = -1;

	col = malloc(nr_queens * sizeof(*col));
	if (!col) {
		printf("Cannot allocate column arrary\n");
		ret = -ENOMEM;
		goto free_col;
	}
	for (i = 0; i < nr_queens; i++)
		col[i] = 0;

	search_queens(nr_queens, row, col, 0);

	printf("Totally found %d results.\n", nr_valid_res);

	free(row);
free_col:
	free(col);

	return ret;
}