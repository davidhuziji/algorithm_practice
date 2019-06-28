/*
 * Copyright @ 2019 Hu Ziji. All rights reserved.
 * Auther: Hu Ziji <davidhuzj@gmail.com>
 */

#include <errno.h>

#include "list.h"

struct single_list_node *get_list_tail(const struct single_list_node *head)
{
	struct single_list_node *node;

	if (!head || !head->next)
		return NULL;

	node = head->next;
	while (node->next) {
		node = node->next;
	}

	return node;
}

int list_add_node(struct single_list_node *prev, struct single_list_node *node)
{
	struct single_list_node *next;

	if (!prev || !node)
		return -EINVAL;

	next = prev->next;

	prev->next = node;
	node->next = next;

	return 0;
}

struct single_list_node *list_del_head(struct single_list_node *head)
{
	struct single_list_node *next;

	if (!head || !head->next)
		return NULL;

	next = head->next;
	head->next = next->next;
	next->next = NULL;

	return next;
}