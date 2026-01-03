/* Colin Ford
 *
 * This is the test function that I got slightly carried away with.
 * Nonetheless, it was useful to do simple tests with my bitvector
 * library.
 *
 * It's useful for doing quick and simple tests.
 *
 * Note: I have not tried any numbers larger than 1,000,000. I
 *       imagine it would still work, but did not find it necessary 
 *       to create any bitvectors that large.
 *
 */

#include "bitvector.h"
#include <string.h>
#define TRUE 1
#define GET "get"
#define SET "set"
#define CLEAR "clear"
#define TOGGLE "toggle"
#define LENGTH "length"
#define PRINT "print"
#define QUIT "quit"

int main()
{
  int num_bits, index;
  char* decision;

  printf("How many bits would you like to make your bitvector? (enter an integer from 1 to 1,000,000)\n");

  scanf("%d", &num_bits);

  printf("Making bitvector of size %d.\n", num_bits);

  Bitvector* bv = make_bv(num_bits);

  printf("Bitvector created.\n");

  decision = malloc(sizeof(char) * 10);
  
  while (TRUE)
  {
    printf("Enter what you would like to do:\n[get] [set] [clear] [toggle] [length] [print] [quit]\n");
    scanf("%s", decision);
    if (!strcmp(GET, decision))
    {
      printf("Which bit would you like to get? Enter index of bit (0 to %d).\n", num_bits - ONE);
      scanf("%d", &index);
      if (index >= num_bits || index < ZERO)
        printf("Number entered out of range.\n");
      else
        printf("The bit at index %d is set to %d\n", index, get(bv, index));
    }
    else if (!strcmp(SET, decision))
    {
      printf("Which bit would you like to set? Enter index of bit (0 to %d).\n", num_bits - ONE);
      scanf("%d", &index);
      if (index >= num_bits || index < ZERO)
        printf("Number entered out of range.\n");
      else
      {
        set(bv, index);
        printf("The bit at index %d has been set to %d.\n", index, get(bv, index));
      }
    }
    else if (!strcmp(CLEAR, decision))
    {
      printf("Which bit would you like to clear? Enter index of bit (0 to %d).\n", num_bits - ONE);
      scanf("%d", &index);
      if (index >= num_bits || index < ZERO)
        printf("Number entered out of range.\n");
      else
      {
        clear(bv, index);
        printf("The bit at index %d has been cleared.\n", index);
      }
    }
    else if(!strcmp(TOGGLE, decision))
    {
      printf("Which bit would you like to toggle? Enter index of bit (0 to %d).\n", num_bits - ONE);
      scanf("%d", &index);
      if (index >= num_bits || index < ZERO)
        printf("Number entered out of range.\n");
      else
      {
        toggle(bv, index);
        printf("The bit at index %d has been toggled to %d.\n", index, get(bv, index));
      }
    }
    else if (!strcmp(LENGTH, decision))
    {
      printf("This bitvector has %d bits.\n", bv -> num_bits);
    }
    else if (!strcmp(PRINT, decision))
    {
      print_bv(bv);
    }
    else if (!strcmp(QUIT, decision))
    {
      printf("Quitting.\n");
      break;
    }
    else
      printf("Invalid input.\n");
    free(decision);
    decision = malloc(sizeof(char) * 10);
  };

  free_bv(bv);
  free(decision);

  printf("Memory freed.\n");

  return 0;
}
