#include<stdio.h>
#include<stdlib.h>

struct node{
  int i;
  struct node *next;
};
typedef struct node Node;

struct stack{
  Node *p;
};
typedef struct stack Stack;


int init(Stack *s){
  
  s->p = (Node *)malloc(sizeof(Node));
  s->p->next = NULL;

}

int push(Stack *s, int i){
  Node *new;
  new = (Node *)malloc(sizeof(Node));
  new->next = s->p;
  s->p->i = i;
  s->p = new;

  return 0;
}

int pop(Stack *s){

  if(s->p->next != NULL){
    s->p = s->p->next;
  }
  else{
    abort();
  }
    return s->p->i;
}





int main(void){
  Stack s;
  init(&s);
  push(&s, 15);
 push(&s, 5);
 push(&s, 6);

 printf("%d\n",pop(&s));


  return 0;
}

