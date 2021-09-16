
#ifndef queue
#include <stdio.h>
#include <stdlib.h>

// Structure of the Request from the client
typedef struct request {
	char* dll_name; // Name of a dynamically loaded library
	char* func_name; // Name of a function to call from the DLL
	char** func_args; // Arguments to pass to the function
}Request; 

// Structure of Data stored in the queue
typedef struct data{
    int *client_soc; // Client File Descriptor
    Request* req; // Client Request
}Data;

// Queue Node
typedef struct node{
    Data* data;
    struct node* prev;
    struct node* next; 
}Node;

// Queue Structure
typedef struct queue{
    Node* front;
    Node* rear;
    int cap; // Max capacity
    int curr_cap; // Capacity at any instance
}Queue;

// Creating Request
Request* createRequest(char* dll, char* func, char** params){
    Request* r = (Request*)malloc(sizeof(Request));
    r-> dll_name = dll;
    r->func_name = func;
    r->func_args = params;
    return r;
}

// Creating Data
Data* createData(int* client, Request* req){
    Data* d = (Data*)malloc(sizeof(Data));
    d->client_soc = client;
    d->req = req;
    return d;
}

// Create Queue Node
Node* createNode(Data* d){
    Node* tmp = (Node*)malloc(sizeof(Node));
    tmp->data = d;
    tmp->prev = tmp->next = NULL;
    return tmp;
}

// Create a Queue
Queue* createQueue(int cap){
    Queue* q = (Queue*)malloc(sizeof(Queue));
    q->front = q->rear = NULL;
    q->cap = cap;
    q->curr_cap = 0;
    return q;
}

/** Function for Enqueue Operation
* @param Queue_Pointer
* @param Data_Pointer
* @return 1 for successful enqueue -1 otherwise 
*/
int enqueue(Queue* q, Data* d){
    if(q->curr_cap==q->cap)
        return -1;
    q->curr_cap += 1;
    Node* tmp = createNode(d);
    if(q->front==NULL && q->rear==NULL){
        q->front = tmp;
        q->rear = tmp;
    }
    else{
        q->rear->next = tmp;
        tmp->prev = q->rear;
        q->rear = tmp;
    }
    return 1;
}

/** Function for Dequeue Operation
* @param Queue_Pointer
* @return Data pointer if queue not empty NULL otherwise
*/
Data* dequeue(Queue* q){
    if(q->curr_cap==0)
        return NULL;
    
    q->curr_cap -= 1;
    Node* tmp=q->front;
    // printf("YO");
    if(q->front==q->rear)
        q->front = q->rear = NULL;
    else{
        q->front = q->front->next;
        q->front->prev = NULL;
    }
    tmp->next = NULL;
    Data* d = tmp->data;
    free(tmp);
    return d;
} 

#define queue
#endif