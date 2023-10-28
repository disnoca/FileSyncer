#include "queue.h"
#include "../../wrapper_functions.h"

/* Helper Functions */

static queue_node* queue_node_create(void* data) {
	queue_node* node = Calloc(1, sizeof(queue_node));
	node->data = data;
	return node;
}

/* Header Implementation */

queue* queue_create() {
	queue* q = Calloc(1, sizeof(queue));
	q->ed_mutex = _CreateMutexW(NULL, FALSE, NULL);
	return q;
}

void enqueue(queue* q, void* data) {
	queue_node* node = queue_node_create(data);

	if(q->length == 0)
		q->head = node;
	else
		q->tail->next = node;

	q->tail = node;
	q->length++;
}

void* dequeue(queue* q) {
	queue_node* node = q->head;
	void* data = node->data;

	q->head = node->next;
	q->length--;

	Free(node);
	return data;
}

/* Concurrency Methods */

void concurrent_enqueue(queue* q, void* data) {
	if(q->length > 1) {
		enqueue(q, data);
		return;
	}

	_WaitForSingleObject(q->ed_mutex, INFINITE);
	enqueue(q, data);
	_ReleaseMutex(q->ed_mutex);	
}

void* concurrent_dequeue(queue* q) {
	if(q->length > 1)
		return dequeue(q);

	_WaitForSingleObject(q->ed_mutex, INFINITE);
	void* data = dequeue(q);
	_ReleaseMutex(q->ed_mutex);
	return data;
}
