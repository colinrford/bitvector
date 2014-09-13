/* Colin Ford
 *
 * My bitvector header file.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;
#define BITSIZE 1    // Potentially useful
#define SIZEOFBYTE 8 // Useful when converting between bits and bytes
#define ONE 1        // tried to rid any hardcoding of 1's
#define ZERO 0       // and zeros

// My bitvector struct. Although supposedly most bitvectors are less than 
// 30 bits, I found it convenient to include the number of bits within the 
// bitvector struct. I used a char* since each char is only a single byte, 
// making it simple to create smaller bitvectors as well as keep the standard 
// 8:1 ratio between bits and bytes.

struct bitvector
{
  unsigned int num_bits;
  unsigned char* bitarray;
};

class Bitvector {

  bitvector* _bv;

  public:

  // creates, mallocs and returns bitvector with specified number of bits
  Bitvector(int num_bits);

  // retrieves ("gets") the value of the bit at given index
  int get(int index);

  // sets the value of the bit at given index to "1"
  void set(int index);

  // clears the value of the bit at given index (sets bit to "0")
  void clear(int index);

  // toggles the value of the bit at given index (sets 0 to 1 and 1 to 0)
  void toggle(int index);

  // returns the number of bits in given bitvector
  int length();

  // print the given bitvector
  void print_bv();

  // frees the given bitvector
  void clear_bv();

  ~Bitvector();

};
