#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
struct stackelem{
  int i;
  struct stackelem *prev;
};
typedef struct stackelem Elem;
struct thestack{
  Elem *tp;
};
typedef struct thestack Stack;
void InitialiseStack(Stack *s);
void Push(Stack *s, int n);
int Pop(Stack *s);
/* main is eactly the same as before */
int main(void)
{
  //printf("1\n");
  Stack s;
  //printf("1\n");
  InitialiseStack(&s);
  //printf("1\n");
  Push(&s, 12);
  Push(&s, 117);
  Push(&s, 13);
  Push(&s, 15);

  printf("%d\n", Pop(&s));
  printf("%d\n", Pop(&s));
  printf("%d\n", Pop(&s));
  printf("%d\n", Pop(&s));
  printf("%d\n", Pop(&s));
  return 0;
}
void InitialiseStack(Stack *s)
{
  s->tp = (Elem *)calloc(1, sizeof(Elem));
  s->tp->prev = NULL;
}
void Push(Stack *s, int n)
{
  Elem *e;
  e = (Elem *)calloc(1, sizeof(Elem));
  e->prev = s->tp;
  s->tp->i = n;
  s->tp = e;
}
int Pop(Stack *s)
{
  s->tp = s->tp->prev;
  assert(s->tp != NULL);
  return s->tp->i;
}
