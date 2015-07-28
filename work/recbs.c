#include <stdio.h>
int RecBin(int k, int *a, int l, int r);
int main(void)
{
  int a[] = {4,7,19,25,36,37,50,100,
	     101,205,220,271,301,321};
  if(RecBin(101, a, 0,13) > 0){
    printf("Found It !\n");
  }
  else
    printf("Not in List\n");
  return 0;
}


int RecBin(int k, int *a, int l, int r)
{
  int m;
  if(l > r) return -1;
  m = (l+r)/2;
  if(k == a[m])
    return m;
  else{
    if (k > a[m])
      return RecBin(k, a, m+1, r);
    else
      return RecBin(k, a, l, m-1);
  }
}
