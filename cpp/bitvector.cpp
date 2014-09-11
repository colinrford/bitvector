/* Colin Ford
 *
 * This is my bitvector library. 
 *
 * To keep from being redundant below:
 * index/SIZEOFBYTE = the index in the char array (a.k.a. the byte)
 * corresponding to the index specified (a.k.a. the bit)
 * Example: For char* with 4 chars, 
 * 00100001 00111001 10110011 01010101
 * If we want to access the 16th bit (or index 15 in bitarray), then
 * we know it will be located in the char (or byte) at index 1.
 * index/SIZEOFBYTE = 1 (index = 15)
 * 
 * index % SIZEOFBYTE = the index in the char (byte) where the 
 * specified bit is located.
 * Example: Continuing from the above example, we are in byte 1,
 * 00111001
 * If we want to access the 16th bit (or index 15 in bitarray), then
 * we know it will be located in the index (or bit) at index 7.
 * index % SIZEOFBYTE = 7 (index = 15)
 * Thus, the bit at index 15 in this example is equal to 1.
 *
 * ONE << (index % SIZEOFBYTE) this will left shift 1 a value of
 * (index % SIZEOFBYTE).
 * Example: Let (index % SIZEOFBYTE) = 5. Then ONE << 5 will left 
 * shift 1 to 10000 (00000001 to 00010000). 
 *
 */

#include "bitvector.h"

Bitvector::Bitvector(int num_bits)
{
  _bv = new bitvector;
  _bv -> num_bits = num_bits;

  // Here we want to make sure to malloc the right amount of memory.
  // In the case that num_bits is less than 8, we would want to make
  // sure to still malloc memory, else num_bits/SIZEOFBYTE = 0, hence
  // the reason for num_bits + 7 (although, it still worked in
  // practice for num_bits < 8 even when + 7 was not included)
  _bv -> bitarray = (unsigned char*) malloc(sizeof(char) * (num_bits + 7)/SIZEOFBYTE);
}

int Bitvector::get(int index)
{
  // And (&) the bit at corresponding index. This will return 0 or 1,
  // depending on whether not the bit at index is equal to 0 or 1
  if (_bv -> bitarray[index/SIZEOFBYTE] & (ONE << (index % SIZEOFBYTE)))
    return ONE;
  else
    return ZERO;
}

void Bitvector::set(int index)
{
  // Or (|) equals will set bit at index equal to 1. 
  _bv -> bitarray[index/SIZEOFBYTE] |= ONE << (index % SIZEOFBYTE);
}

void Bitvector::clear(int index)
{
  // And (&) equals will clear bit at index to 0. The ~ will flip
  // each bit in the ONE << (index % SIZEOFBYTE) byte, so the
  // corresponding bit at index will be set to zero, while the others
  // will be left untouched (thanks to the ~ operator)
  _bv -> bitarray[index/SIZEOFBYTE] &= ~(ONE << (index % SIZEOFBYTE));
}

void Bitvector::toggle(int index)
{
  // XOR (^) equals will set bit at index equal to 0 or 1, depending
  // on what the bit is currently set at (0 ^ 1 = 1; 1 ^ 1 = 0).
  _bv -> bitarray[index/SIZEOFBYTE] ^= ONE << (index % SIZEOFBYTE);
}

int Bitvector::length()
{
  return _bv -> num_bits;
}

void Bitvector::print_bv()
{
  for (unsigned i = 0; i < _bv -> num_bits; i++)
  {
    if (get(i)) // bit i is set to 1
      printf("1");
    else
      printf("0");
  }
  
  printf("\n");
}

void Bitvector::clear_bv()
{
  if (_bv == NULL)
    return;
  for (unsigned i = 0; i < _bv -> num_bits; i++)
    clear(i);
}

Bitvector::~Bitvector()
{
  delete _bv;
}
