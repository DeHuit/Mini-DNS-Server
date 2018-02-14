#include <stdio.h>

int main(int argc, char const *argv[]) {

  char adr[18], *start, *end;
  strcpy(adr, "192.0.0.1");

  start = adr;

  printf("%d\n", atoi(start));

  return 0;
}
