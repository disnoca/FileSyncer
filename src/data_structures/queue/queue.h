#ifndef QUEUE_H_INCLUDED
#define QUEUE_H_INCLUDED

#include <windows.h>

typedef struct QueueNode QueueNode;

typedef struct {
	QueueNode *head, *tail;
	unsigned length;
	HANDLE edMutex;
} Queue;

struct QueueNode {
	void* data;
	QueueNode* next;
};

/**
 * Creates a returns a pointer to a new Queue.
 * 
 * @return a pointer to a new Queue
*/
Queue* QueueCreate();

/**
 * Adds the specified element to the end of the specified Queue.
 * 
 * @param q the Queue to add the element to
 * @param data the element to add to the Queue
*/
void Enqueue(Queue* q, void* data);

/**
 * Removes and returns the element at the front of the specified Queue.
 * 
 * @param q the Queue to remove the element from
 * @return the element at the front of the specified Queue
*/
void* Dequeue(Queue* q);

/**
 * Adds the specified element to the end of the specified Queue.
 * 
 * This function is thread-safe only for concurrent enqueue and dequeue calls, not for concurrent enqueue calls.
 * 
 * @param q the Queue to add the element to
 * @param data the element to add to the Queue
*/
void ConcurrentEnqueue(Queue* q, void* data);

/**
 * Removes and returns the element at the front of the specified Queue.
 * 
 * This function is thread-safe only for concurrent enqueue and dequeue calls, not for concurrent dequeue calls.
 * 
 * @param q the Queue to remove the element from
 * @return the element at the front of the specified Queue
*/
void* ConcurrentDequeue(Queue* q);

#endif
