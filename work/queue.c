#include <stdio.h>
#include <stdlib.h>



struct node{
  int i;
  struct node *next;
};
typedef struct node Node;

struct queue{
  Node *front;
  Node *back;
};
typedef struct queue Queue;


int initQueue(Queue *q){
  q->back = q->front = NULL;
  return 1;
}

int insertNode(Queue *q, int i){
  Node *p;
  if(q->front == NULL){
    q->front = q->back = p;
    return 0;
  }
  q->back->next = p;
  q->back = p;
  return 1;
}

Node *removeNode(Queue *q){
  Node *p;
  p = q->front;
  if(q->front ==NULL){
    return NULL;
  }
  if(q->front == q->back){
    q->front = q->back = NULL;
    return p;
  }
  q->front = p->next;
  return p;
}

int empty(Queue *q){
  if(q->front==NULL){
    return 1;
  }
  return 0;
}


int main(int argc, char *argv[]){

  Queue q;
  Node *n;
  initQueue(&q);
  insertNode(&q,1);
  n = removeNode(&q);
  printf("%d\n",n->i);
  return 0;
}
