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

int init(Queue *q){
  q->front = q->back = NULL;
}

int enqueue(Queue *q, int i){
  Node *p;
  p = (Node *)malloc(sizeof(Node));
  p->i = i;

  if(q->front == NULL){
    q->front = q->back = p;
    return 0;
  }
  q->back->next = p;
  q->back = p;
  return 1;

}
int dequeue(Queue *q){

  if(q->front == NULL){
    abort();
  }
  if(q->front == q->back){
    return q->front->i;
  }
  int k;
  k = q->front->i;
  q->front = q->front->next;
  return k;
}



int main(void){
  Queue q;
  init(&q);
  enqueue(&q,10);
  printf("%d\n", dequeue(&q));

  return 0;
}
