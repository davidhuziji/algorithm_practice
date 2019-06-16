/*
 * Copyright @ 2019 Hu Ziji. All rights reserved.
 * Auther: Hu Ziji <davidhuzj@gmail.com>
 *
 * Check if a input word is palindrome.
 */

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "palindrome_list.h"

#define INPUT_BUF_LEN       0x100

static int collect_input_word(char *buf, size_t len)
{
	char *ch;
	size_t actual_len = 0;

	if (!buf || (len == 0x0))
		return -EINVAL;

	printf("Please input a word for test with at most %ld characters\n",
	       len - 1);

	while (!fgets(buf, len, stdin))
		printf("Cannot recoginze the input. Please try again.\n");

	ch = buf;

	while((*ch != '\n') && (*ch != '\0')) {
		actual_len++;

		if ((*ch < 'A') || ((*ch > 'Z') && (*ch < 'a')) ||
		    (*ch > 'z')) {
			    printf("Invalid characters %c!\n", *ch);
			    return -EINVAL;
		}

		ch++;
	}

	if (*ch == '\n')
		*ch = '\0';

	printf("Input word: %s\n", buf);
	return actual_len;
}

int main(void)
{
	char buf[INPUT_BUF_LEN];
	int len;
	struct palind_list_node *head;

	while (1) {
		len = collect_input_word(buf, INPUT_BUF_LEN);
		if (len <= 0)
			continue;

		head = organize_palind_list(buf, len);

		if (check_palind_list(head))
			printf("%s is a palindrome word.\n", buf);
		else
			printf("%s is NOT a plindrome word.\n", buf);

		free_palind_list(head);
	}
}