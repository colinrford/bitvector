#include <stdio.h>
#define EIGHT 8

int main()
{
  int nine = 9;

  printf("%d / %d = %d\n", nine, EIGHT, nine/EIGHT);
  printf("%d mod %d = %d\n", EIGHT, nine, (int) EIGHT % nine);
  return 0;
}
