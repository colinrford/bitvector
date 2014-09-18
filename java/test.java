import java.util.Scanner;

public class test 
{
	public static void main(String[] args)
	{
		Scanner in = new Scanner(System.in);
		int num_bits, index;
		
		String decision = new String();

		System.out.println("How many bits would you like to make your bitvector? (enter an integer from 1 to 1,000,000)");

		cin >> num_bits;

		System.out.println("Making bitvector of size " + num_bits + ".");

		Bitvector bv = new Bitvector(num_bits);

		  System.out.println("Bitvector created.");
		  
		  while (true)
		  {
		    System.out.println("Enter what you would like to do:\n[get] [set] [clear] [toggle] [length] [print] [quit]");
		    cin >> decision;
		    if (decision.equals("get"))
		    {
		      System.out.println("Which bit would you like to get? Enter index of bit (0 to " + (num_bits - 1) + ".");
		      cin >> index;
		      if (index >= num_bits || index < 0)
		        System.out.println("Number entered out of range.");
		      else
		    	  System.out.println("The bit at index " + index + " is set to " + bv.get(index) + ".");
		    }
		    else if (decision.equals("set"))
		    {
		    	System.out.println("Which bit would you like to set? Enter index of bit (0 to " + (num_bits - 1) + ").");
		      cin >> index;
		      if (index >= num_bits || index < 0)
		    	  System.out.println("Number entered out of range.");
		      else
		      {
		        bv.set(index);
		        System.out.println("The bit at index " + index + "has been set to " + bv.get(index) + ".");
		      }
		    }
		    else if (decision.equals("clear"))
		    {
		    	System.out.println("Which bit would you like to clear? Enter index of bit (0 to " + (num_bits - 1) + ").");
		      cin >> index;
		      if (index >= num_bits || index < 0)
		    	  System.out.println("Number entered out of range.");
		      else
		      {
		        bv.clear(index);
		        System.out.println("The bit at index " + index + " has been cleared.");
		      }
		    }
		    else if(decision.equals("toggle"))
		    {
		    	System.out.println("Which bit would you like to toggle? Enter index of bit (0 to " + (num_bits - 1) + ").");
		      cin >> index;
		      if (index >= num_bits || index < 0)
		    	  System.out.println("Number entered out of range.");
		      else
		      {
		        bv.toggle(index);
		        System.out.println("The bit at index " + index + " has been toggled to " + bv.get(index) + ").");
		      }
		    }
		    else if (decision.equals("length"))
		    {
		    	System.out.println("This bitvector has " + (bv.length()) + " bits.");
		    }
		    else if (decision.equals("print"))
		    {
		      bv.print();
		    }
		    else if (decision.equals("quit"))
		    {
		    	System.out.println("Quitting.");
		      break;
		    }
		    else
		    	System.out.println("Invalid input.");
		    
		  };

		  System.out.println("Program terminating.");		
	}
}
