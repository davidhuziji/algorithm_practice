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

static void free_list(struct single_list_node *head)
{
	struct single_list_node *next, *node = head->next;

	while (node) {
		next = node->next;
		free(node);
		node = next;
	}
}

struct queue_t {
	struct single_list_node list_head;
	/*
	 * Enqueue: put the new node in the list tail. The head pointer actually
	 * points to the list tail.
	 * Dequeue: delete the first node in the list. The tail pointer actually
	 * points to the first node.
	 */
	struct single_list_node *head;
};

static struct queue_t queue;

static void init_queue(struct queue_t *queue_ptr)
{
	if (!queue_ptr)
		return;

	queue_ptr->list_head.next = NULL;
	queue_ptr->head = &queue_ptr->list_head;
}

static void enqueue(struct queue_t *queue_ptr, int data)
{
	struct single_list_node *node;

	printf("Enqueue %d\n", data);

	node = malloc(sizeof(*node));
	node->data = data;
	node->next = NULL;

	list_insert_node(queue_ptr->head, node);
	queue_ptr->head = node;

	printf("After enqueue: ");
	print_list(&queue_ptr->list_head);
}

static void dequeue(struct queue_t *queue_ptr)
{
	struct single_list_node *node;

	node = list_del_head(&queue_ptr->list_head);
	if (!node) {
		printf("Queue is empty\n");
		return;
	}

	printf("Dequeue %d\n", node->data);
	free(node);

	printf("After dnqueue: ");
	print_list(&queue_ptr->list_head);

	/*
	 * XXX
	 * Please note a special case here. When queue is empty, the head
	 * pointer should be also adjusted.
	 */
	if (!queue_ptr->list_head.next)
		queue_ptr->head = &queue_ptr->list_head;

}

int main(void)
{
	int i, enqueue_flag, data, test_round = 0x20;

	srand(time(NULL));

	init_queue(&queue);

	for (i = 0; i < test_round; i++) {
		enqueue_flag = rand() % 2;
		if (enqueue_flag) {
			data = rand() % 0x20;
			enqueue(&queue, data);
		} else {
			dequeue(&queue);
		}
	}

	free_list(&queue.list_head);

	return 0;
}