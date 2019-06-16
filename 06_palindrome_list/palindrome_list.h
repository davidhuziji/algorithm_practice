/*
 * Copyright @ 2019 Hu Ziji. All rights reserved.
 * Auther: Hu Ziji <davidhuzj@gmail.com>
 */

#ifndef _PALINDROME_LIST_H_
#define _PALINDROME_LIST_H_

#include <stdbool.h>

struct palind_list_node {
	char ch;
	struct palind_list_node *next;
};

struct palind_list_node *organize_palind_list(const char *word, size_t len);

bool check_palind_list(const struct palind_list_node *head);

void free_palind_list(struct palind_list_node *head);

#endif