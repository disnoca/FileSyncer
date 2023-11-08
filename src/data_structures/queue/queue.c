#include "../../wrapper_functions.h"
#include "queue.h"

/* Helper Functions */

static QueueNode* CreateQueueNode(void* data) {
	QueueNode* node = Calloc(1, sizeof(QueueNode));
	node->data = data;
	return node;
}

/* Header Implementation */

Queue* CreateQueue() {
	return Calloc(1, sizeof(Queue));
}

void Enqueue(Queue* q, void* data) {
	QueueNode* node = CreateQueueNode(data);

	if(q->length == 0)
		q->head = node;
	else
		q->tail->next = node;

	q->tail = node;
	q->length++;
}

void* Dequeue(Queue* q) {
	QueueNode* node = q->head;
	void* data = node->data;

	q->head = node->next;
	q->length--;

	Free(node);
	return data;
}
