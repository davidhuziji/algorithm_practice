/*
 * Copyright @ 2019 Hu Ziji. All rights reserved.
 * Auther: Hu Ziji <davidhuzj@gmail.com>
 */

#ifndef _LIST_H_
#define _LIST_H_

#include <stdlib.h>

struct single_list_node {
	int data;
	struct single_list_node *next;
};

struct single_list_node *get_list_tail(const struct single_list_node *head);

int list_insert_node(struct single_list_node *prev,
		     struct single_list_node *node);

struct single_list_node *list_del_head(struct single_list_node *head);

#endif