
public class Bitvector 
{
	private char[] bv;
	
	private int SIZEOFBYTE = 8;
	
	private int ONE = 1;
	
	public Bitvector(int num_bits)
	{
	  bv = new char[(num_bits + 7)/SIZEOFBYTE];

	  // _bv -> bitarray = (unsigned) malloc(sizeof() * (num_bits + 7)/SIZEOFBYTE);
	}

	public int get(int index)
	{
	  // And (&) the bit at corresponding index. This will return 0 or 1,
	  // depending on whether not the bit at index is equal to 0 or 1
		if ((bv[index/SIZEOFBYTE] & (ONE << (index % SIZEOFBYTE))) == 1)
			return 1;
		else
			return 0;
	}

	public void set(int index)
	{
	  // Or (|) equals will set bit at index equal to 1. 
	  bv[index/SIZEOFBYTE] |= ONE << (index % SIZEOFBYTE);
	}

	public void clear(int index)
	{
	  // And (&) equals will clear bit at index to 0. The ~ will flip
	  // each bit in the ONE << (index % SIZEOFBYTE) byte, so the
	  // corresponding bit at index will be set to zero, while the others
	  // will be left untouched (thanks to the ~ operator)
	  bv[index/SIZEOFBYTE] &= ~(ONE << (index % SIZEOFBYTE));
	}

	public void toggle(int index)
	{
	  // XOR (^) equals will set bit at index equal to 0 or 1, depending
	  // on what the bit is currently set at (0 ^ 1 = 1; 1 ^ 1 = 0).
	  bv[index/SIZEOFBYTE] ^= ONE << (index % SIZEOFBYTE);
	}

	public int length()
	{
	  return bv.length;
	}

	public void print()
	{
	  for (int i = 0; i < length(); i++)
	  {
	    if (get(i) == 1) // bit i is set to 1
	      System.out.printf("1");
	    else
	      System.out.printf("0");
	  }
	  
	  System.out.printf("\n");
	}

	public void clear_bv()
	{
	  if (bv == null)
	    return;
	  for (int i = 0; i < length(); i++)
	    clear(i);
	}

}
