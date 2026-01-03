#include "bitvector.h"
#include <time.h>

int main()
{
  int num_bits;
  double startTime, endTime, timeElapsed;

  num_bits = 3000;
  startTime = (double) clock();// / CLOCKS_PER_SEC;

  Bitvector* bv = make_bv(num_bits);

  set(bv, 1);
  set(bv, 3);
  set(bv, 5);
  set(bv, 7);
  set(bv, 9);
  print_bv(bv);

  free_bv(bv);

  endTime = (double) clock();// / CLOCKS_PER_SEC;

  timeElapsed = endTime - startTime;

  printf("Start time = %f; end time = %f, time elapsed = %f\n", startTime, endTime, timeElapsed);

  return 0;
}
