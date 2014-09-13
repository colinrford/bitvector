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
  string decision(10, ' ');

  cout << "How many bits would you like to make your bitvector? (enter an integer from 1 to 1,000,000)\n";

  cin >> num_bits;

  cout << "Making bitvector of size " << num_bits << ".\n";

  Bitvector* bv = new Bitvector(num_bits);

  cout << "Bitvector created.\n";
  
  while (TRUE)
  {
    cout << "Enter what you would like to do:\n[get] [set] [clear] [toggle] [length] [print] [quit]\n";
    cin >> decision;
    if (!decision.compare(GET))
    {
      cout << "Which bit would you like to get? Enter index of bit (0 to " << num_bits - ONE << ".\n";
      cin >> index;
      if (index >= num_bits || index < ZERO)
        cout << "Number entered out of range.\n";
      else
        cout << "The bit at index " << index << " is set to " << bv -> get(index) << endl;
    }
    else if (!decision.compare(SET))
    {
      cout << "Which bit would you like to set? Enter index of bit (0 to " << num_bits - ONE << ").\n";
      cin >> index;
      if (index >= num_bits || index < ZERO)
        cout << "Number entered out of range.\n";
      else
      {
        bv -> set(index);
        cout << "The bit at index " << index << "has been set to " << bv -> get(index) << ".\n";
      }
    }
    else if (!decision.compare(CLEAR))
    {
      cout << "Which bit would you like to clear? Enter index of bit (0 to " << num_bits - ONE << ").\n";
      cin >> index;
      if (index >= num_bits || index < ZERO)
        cout << "Number entered out of range.\n";
      else
      {
        bv -> clear(index);
        cout << "The bit at index " << index << " has been cleared.\n";
      }
    }
    else if(!decision.compare(TOGGLE))
    {
      cout << "Which bit would you like to toggle? Enter index of bit (0 to " << num_bits - ONE << ").\n";
      cin >> index;
      if (index >= num_bits || index < ZERO)
        cout << "Number entered out of range.\n";
      else
      {
        bv -> toggle(index);
        cout << "The bit at index " << index << " has been toggled to " << bv -> get(index) << ").\n";
      }
    }
    else if (!decision.compare(LENGTH))
    {
      cout << "This bitvector has " << bv -> length() << " bits.\n";
    }
    else if (!decision.compare(PRINT))
    {
      bv -> print_bv();
    }
    else if (!decision.compare(QUIT))
    {
      cout << "Quitting.\n";
      break;
    }
    else
      cout << "Invalid input.\n";
    
  };

  delete bv;

  cout << "Memory freed.\n";

  return 0;
}
