/*
 * Copyright @ 2019 Hu Ziji. All rights reserved.
 * Auther: Hu Ziji <davidhuzj@gmail.com>
 */

#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "list.h"

static void print_list(struct single_list_node *head)
{
	struct single_list_node *node;

	if (!head)
		return;

	node = head->next;
	while (node) {
		printf("%d->", node->data);
		node = node->next;
	}

	printf("\n");
}

static void print_ring_list(struct single_list_node *head, size_t len)
{
	struct single_list_node *node;
	int i;

	if (!head)
		return;

	node = head;
	for (i = 0; i <= len; i++) {
		node = node->next;
		printf("%d->", node->data);
	}

	printf("\n");
}

static void free_list(struct single_list_node *head)
{
	struct single_list_node *next, *node = head;

	while (node) {
		next = node->next;
		free(node);
		node = next;
	}
}

static struct single_list_node *generate_list(int start, size_t len, int step,
					      bool random)
{
	struct single_list_node *head, *node, *prev;
	int i, interval, data = start;

	head = malloc(sizeof(*head));
	if (!head)
		return NULL;

	prev = head;

	for (i = 0; i < len; i++) {
		node = malloc(sizeof(*node));
		if (!node) {
			free_list(head);
			return NULL;
		}

		node->data = data;
		if (!random) {
			data += step;
		} else {
			interval = rand() % step;
			data += interval;
		}

		prev->next = node;
		prev = node;
	}

	node->next = NULL;

	return head;
}

static void reverse_single_list(int start, size_t len)
{
	struct single_list_node *head, *node, *last;

	head = generate_list(start, len, 1, false);
	if (!head) {
		printf("Fail to generate a singly linked list\n");
		return;
	}

	print_list(head);

	last = get_list_tail(head);
	if (!last) {
		free_list(head);
		return;
	}

	while (1) {
		if (head->next == last)
			break;

		node = list_del_head(head);
		list_insert_node(last, node);
	}

	print_list(head);

	free_list(head);
}

#if 0
static bool detect_list_ring(struct single_list_node *head)
{
	struct single_list_node tmp_head, *tmp_tail, *node, *point;
	bool ret;

	if (!head || !head->next)
		return false;

	tmp_head.next = NULL;
	tmp_tail = &tmp_head;
	while (1) {
		node = list_del_head(head);
		if (!node) {
			ret = false;
			break;
		}

		list_insert_node(tmp_tail, node);
		tmp_tail = node;

		point = head->next;
		if (!point) {
			ret = false;
			break;
		}

		node = &tmp_head;
		while (node->next) {
			if (point == node->next) {
				printf("Ring point value %d\n", point->data);
				ret = true;
				goto recover_list;
			}

			node = node->next;
		}
	}

recover_list:
	head->next = tmp_head.next;

	return ret;
}
#else
/*
 * Basic idea: IStarting from head of list. In each step, single-step search
 * jumps to next node and double-step search jumps to next two node.
 * Compare the two nodes of single-step and double-step search in each step.
 * After several steps, if single-step search and double-step search stay at
 * the same node, the list has a ring.
 * If double-step search reachs next node pointer as NULL, it proves that the
 * list has no ring.
 *
 * Prove:
 * Assume ring lengh = R > 1 and total number of nodes = N.
 * After s step when single search already passed the conjuction point but
 * hasn't finished its first loop yet, then
 * single-step search: s = N - R + p
 * double-step search: 2 * s = r * R + (N - R) + q
 * in which, r is the number of loops double-step search has finished. p is the
 * number of nodes single-step search passed after crossing the conjunction
 * point. q is the number of nodes double-step search passed after crossing the
 * conjunction point at last time.
 *
 * To achieve p = q, then we get:
 * s = r * R
 * s <= N
 * r = N/R - 1 + p/R and r > 0
 *
 * As long a set r = floor(N/R), p = q can be satisfied.
 * As a result, it can prove that after s = r *  R steps, single-step search
 * and double-step search can meet at the same node on the ring.
 *
 * Besides, it can also prove that steps <= N, which means O(n)
 */
static bool detect_list_ring(struct single_list_node *head)
{
	struct single_list_node *single_node, *double_node;

	if (!head)
		return false;

	single_node = head->next;
	if (!single_node)
		return false;

	double_node = single_node->next;
	if (!double_node)
		return false;

	while (1) {
		single_node = single_node->next;

		double_node = double_node->next;
		if (!double_node)
			return false;
		double_node = double_node->next;
		if (!double_node)
			return false;

		if (single_node->data == double_node->data)
			return true;
	}
}
#endif

static void test_list_ring(int start, size_t len)
{
	struct single_list_node *head, *node, *tail;
	int i, j, ring_len, rand_test = 10;

	head = generate_list(start, len, 1, false);
	if (!head)
		return;

	print_list(head);

	if (detect_list_ring(head))
		printf("Incorrectly report a ring list.\n");
	else
		printf("Succussfully report a non-ring list.\n");

	free_list(head);

	for (j = 0; j < rand_test; j++) {
		head = generate_list(start, len, 1, false);
		if (!head)
			return;

		ring_len = rand() % (len - 1) + 2;
		printf("Create a ring list. Ring length %d\n", ring_len);

		/* Force the ring */
		node = head->next;
		for (i = 0; i < len - ring_len; i++) {
			node = node->next;
		}

		tail = get_list_tail(head);
		tail->next = node;
		print_ring_list(head, len);

		if (!detect_list_ring(head))
			printf("Fail to detect a ring list.\n");
		else
			printf("Successfully detect a ring list.\n");

		/* break the ring and free the list */
		node = head;
		for (i = 0; i < len; i++)
			node = node->next;
		node->next = NULL;

		free_list(head);
	}
}

static void merge_sort_lists(struct single_list_node *head1,
			     struct single_list_node *head2)
{
	struct single_list_node *node1, *node2, *prev;

	if (!head1 || !head2)
		return;

	if (!head1->next || !head2->next)
		return;

	if (head1->next->data <= head2->next->data) {
		prev = head1;
		head1 = head2;
		head2 = prev;
	}

	node1 = head1->next;
	node2 = head2->next;

	while (1) {
		node1 = list_del_head(head1);
		if (!node1)
			goto finish;

		prev = node2;
		while (node1->data > node2->data) {
			prev = node2;

			node2 = node2->next;
			if (node2 == NULL)
				goto finish;
		}

		list_insert_node(prev, node1);
		node2 = node1;
	}

finish:
	printf("After merge:\n");
	print_list(head2);
	printf("\n");
}

static void merge_sort_list_practice(int start, size_t len)
{
	struct single_list_node *head1, *head2;
	int step, sec_len, j, rand_test = 10;

	for (j = 0; j < rand_test; j++) {
		head1 = generate_list(start, len, 5, true);
		if (!head1)
			return;
		printf("list 1: ");
		print_list(head1);

		start--;

		step = rand() % 3;
		start += step;

		sec_len = rand() % len;
		sec_len += len;
		head2 = generate_list(start, sec_len, 5, true);
		if (!head2)
			goto cleanup_list1;
		printf("list 2: ");
		print_list(head2);

		merge_sort_lists(head1, head2);

		free_list(head2);
cleanup_list1:
		free_list(head1);
	}
}

/*
 * Record the last node in interval N and the previous last node in interval N.
 * Go through the list from the previous last intveral node, instead of from
 * the head of list.
 */
static void find_last_n_node(void)
{
	struct single_list_node *head, *node, *interval_node = NULL;
	struct single_list_node *prev_interval_node;
	int i, j = 0, k, len, interval, rand_test = 0x10;

	for (i = 0; i < rand_test; i++) {
		len = rand() % 0x50 + 0x50;

		head = generate_list(0x1, len, 1, false);
		if (!head)
			return;
		print_list(head);

		interval = rand() % 0x5 + 0x5;

		node = head;
		j = 0;
		while (node->next) {
			node = node->next;

			j++;
			if (j == interval) {
				prev_interval_node = interval_node;
				interval_node = node;
				j = 0;
			}
		}

		if (!prev_interval_node)
			printf("The lenght is too short\n");

		node = prev_interval_node;
		for (k = 0; k <= j; k++)
			node = node->next;

		printf("Last %d node is %d\n", interval, node->data);

		free_list(head);
	}
}

/*
 * Continue using single-step and double-step search.
 * When double-step search reaches the end of list, the current single-step is
 * the middle node.
 * Implement different output for odd number and even numer of nodes in list.
 */
static void find_middle_node(void)
{
	struct single_list_node *head, *single_node, *double_node;
	int i, len, rand_test = 0x10;
	bool odd = false;

	for (i = 0; i < rand_test; i++) {
		len = rand() % 0x10 + 0x10;

		head = generate_list(0x1, len, 1, false);
		if (!head)
			return;
		print_list(head);

		single_node = head;
		double_node = head;
		while (1) {
			double_node = double_node->next;
			if (!double_node) {
				odd = false;
				break;
			}
			double_node = double_node->next;
			if (!double_node) {
				odd = true;
				break;
			}
			single_node = single_node->next;
		}

		printf("Totally %d nodes. ", len);
		if (odd)
			printf("The middle node is %d\n",
			       single_node->next->data);
		else
			printf("The middle nodes are %d and %d\n",
			       single_node->data, single_node->next->data);

		free_list(head);
	}
}

int main(void)
{
	srand(time(NULL));

	printf("Practice how to reverse list\n");
	reverse_single_list(0x0, 10);
	reverse_single_list(0x1, 1);

	printf("\nPractice how to detect a ring in a list\n");
	test_list_ring(0x1, 20);

	printf("\nPractice how to merge two sorted lists\n");
	merge_sort_list_practice(0x1, 10);

	printf("\nFind the last N node\n");
	find_last_n_node();

	printf("\nFind the middle node\n");
	find_middle_node();

	return 0;
}