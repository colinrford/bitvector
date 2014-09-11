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
#include <string>
#include <iostream>
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
  std::string decision(10, ' ');

  printf("How many bits would you like to make your bitvector? (enter an integer from 1 to 1,000,000)\n");

  scanf("%d", &num_bits);

  printf("Making bitvector of size %d.\n", num_bits);

  Bitvector* bv = new Bitvector(num_bits);

  printf("Bitvector created.\n");
  
  while (TRUE)
  {
    printf("Enter what you would like to do:\n[get] [set] [clear] [toggle] [length] [print] [quit]\n");
    std::cin >> decision;
    if (!decision.compare(GET))
    {
      printf("Which bit would you like to get? Enter index of bit (0 to %d).\n", num_bits - ONE);
      scanf("%d", &index);
      if (index >= num_bits || index < ZERO)
        printf("Number entered out of range.\n");
      else
        printf("The bit at index %d is set to %d\n", index, bv -> get(index));
    }
    else if (!decision.compare(SET))
    {
      printf("Which bit would you like to set? Enter index of bit (0 to %d).\n", num_bits - ONE);
      scanf("%d", &index);
      if (index >= num_bits || index < ZERO)
        printf("Number entered out of range.\n");
      else
      {
        bv -> set(index);
        printf("The bit at index %d has been set to %d.\n", index, bv -> get(index));
      }
    }
    else if (!decision.compare(CLEAR))
    {
      printf("Which bit would you like to clear? Enter index of bit (0 to %d).\n", num_bits - ONE);
      scanf("%d", &index);
      if (index >= num_bits || index < ZERO)
        printf("Number entered out of range.\n");
      else
      {
        bv -> clear(index);
        printf("The bit at index %d has been cleared.\n", index);
      }
    }
    else if(!decision.compare(TOGGLE))
    {
      printf("Which bit would you like to toggle? Enter index of bit (0 to %d).\n", num_bits - ONE);
      scanf("%d", &index);
      if (index >= num_bits || index < ZERO)
        printf("Number entered out of range.\n");
      else
      {
        bv -> toggle(index);
        printf("The bit at index %d has been toggled to %d.\n", index, bv -> get(index));
      }
    }
    else if (!decision.compare(LENGTH))
    {
      printf("This bitvector has %d bits.\n", bv -> length());
    }
    else if (!decision.compare(PRINT))
    {
      bv -> print_bv();
    }
    else if (!decision.compare(QUIT))
    {
      printf("Quitting.\n");
      break;
    }
    else
      printf("Invalid input.\n");
    //delete decision;
  };

  delete bv;
  //delete decision;

  printf("Memory freed.\n");

  return 0;
}
