#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "queue.h"

void initQueue(Queue* queue) {
    queue->front = NULL;
    queue->rear = NULL;
}

void enqueue(Queue* queue, int element) {
    Node* newNode = (Node*) malloc(sizeof(Node));
    newNode->data = element;
    newNode->next = NULL;
    if (queue->rear == NULL) {
        queue->front = newNode;
        queue->rear = newNode;
    } else {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }
}

int dequeue(Queue* queue) {
    if (queue->front == NULL) {
        printf("Error: Queue is empty!\n");
        exit(1);
    }
    int data = queue->front->data;
    Node* temp = queue->front;
    queue->front = queue->front->next;
    if (queue->front == NULL) {
        queue->rear = NULL;
    }
    free(temp);
    return data;
}

bool isEmpty(Queue* queue) {
    return queue->front == NULL;
}