#include "queue.h"
#include "../../wrapper_functions.h"

/* Helper Functions */

static QueueNode* CreateQueueNode(void* data) {
	QueueNode* node = Calloc(1, sizeof(QueueNode));
	node->data = data;
	return node;
}

/* Header Implementation */

Queue* QueueCreate() {
	Queue* q = Calloc(1, sizeof(Queue));
	q->edMutex = _CreateMutexW(NULL, FALSE, NULL);
	return q;
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

/* Concurrency Methods */

void ConcurrentEnqueue(Queue* q, void* data) {
	_WaitForSingleObject(q->edMutex, INFINITE);
	Enqueue(q, data);
	_ReleaseMutex(q->edMutex);	
}

void* ConcurrentDequeue(Queue* q) {
	_WaitForSingleObject(q->edMutex, INFINITE);
	void* data = Dequeue(q);
	_ReleaseMutex(q->edMutex);
	return data;
}
