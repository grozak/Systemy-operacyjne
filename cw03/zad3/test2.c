#include <stdlib.h>

int main() {
  int i=0;
  while(1) {
    while(i<1000000)
      i++;
    i=0;
  }

  return 0;
}
