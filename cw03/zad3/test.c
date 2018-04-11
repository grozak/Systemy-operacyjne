#include <stdio.h>
#include <stdlib.h>

void haps() {
  char* lol=malloc(1024*1024);
  for(int i=0;i<1000000;i++)
    lol[i]='a';
  haps();
}

int main() {
  haps();

  return 0;
}
