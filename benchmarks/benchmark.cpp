/*
 *  benchmark.cpp – written by Colin Ford
 *    see github.com/colinrford/bitvector
 *    currently unlicensed
 */
import std;
import lam.bitvector;

using lam::bitvec::bitvector;

constexpr int iterations = 5;

// Generic Sieve Implementation
template <std::uint64_t N, typename BitSet>
auto run_sieve(BitSet& bv) 
{
  if constexpr (requires { bv.reset(); }) 
    bv.reset();
  else if constexpr (requires { bv.clear_bv(); }) 
    bv.clear_bv();
  
  std::uint64_t firstprime = 2;
  auto prime = firstprime;
  std::uint64_t running_count = 0;
  
  while (prime + 1 < N - 1)
  {
    auto primecopy = prime;
    while (prime * primecopy < N - 1)
    {
      bv.set(prime * primecopy);
      primecopy++;
    }
    // Access optimization match
    if constexpr (requires { bv[0]; }) 
    {
      while (prime + 1 < (N - 1) && bv[++prime]);
    }
    running_count++;
  }
  return running_count;
}

template <std::uint64_t N>
void run_comparison() 
{
  std::println("--- Benchmarking N = {} ---", N);
  
  // Custom Lambda
  auto run_custom = []() {
    bitvector<> bv(N);
    return run_sieve<N>(bv);
  };
  
  // Std Lambda
  auto run_std = []() {
    std::bitset<N> bv; // Stack allocation might fail for huge N, but 1M is fine (125KB)
    return run_sieve<N>(bv);
  };

  // Validation
  auto count = run_custom();
  if (count != run_std()) 
  {
    std::println("ERROR: Mismatch for N={}", N);
    return;
  }
  std::println("Count for N={}: {}", N, count);

  // Measure Custom
  auto start_custom = std::chrono::steady_clock::now();
  for(int i=0; i<iterations; ++i) volatile auto v = run_custom();
  auto end_custom = std::chrono::steady_clock::now();
  
  // Measure Std
  auto start_std = std::chrono::steady_clock::now();
  for(int i=0; i<iterations; ++i) volatile auto v = run_std();
  auto end_std = std::chrono::steady_clock::now();

  auto dur_custom = std::chrono::duration<double, std::milli>(end_custom - start_custom).count() / iterations;
  auto dur_std = std::chrono::duration<double, std::milli>(end_std - start_std).count() / iterations;
  
  std::println("lam::bitvector : {:.3f} ms", dur_custom);
  std::println("std::bitset    : {:.3f} ms", dur_std);
  std::println("Ratio          : {:.2f}x\n", dur_custom / dur_std);
}

int main()
{
  std::println("lam::bitvector v{} ({}) [{}]", 
               lam::bitvec::config::version_string,
               lam::bitvec::config::build_type,
               lam::bitvec::config::git_hash);
  std::println("Benchmarking Sieve of Eratosthenes (Averaging {} runs)\n", iterations);
  
  run_comparison<100'000>();
  run_comparison<262'144>(); // 2^18
  run_comparison<524'288>(); // 2^19
  run_comparison<1'000'000>();
  
  run_comparison<10'000'000>();
  run_comparison<15'000'000>();
  run_comparison<20'000'000>();
}
