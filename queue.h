// queue.h
#include <stdbool.h>

#ifndef QUEUE_H
#define QUEUE_H

typedef struct Node {
    int data;
    struct Node* next;
} Node;

typedef struct Queue {
    Node* front;
    Node* rear;
} Queue;

void initQueue(Queue* queue);
void enqueue(Queue* queue, int element);
int dequeue(Queue* queue);
bool isEmpty(Queue* queue);

#endif /* QUEUE_H */