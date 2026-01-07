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
template<std::uint64_t N, typename BitSet>
void run_sieve(BitSet& bv)
{
  if constexpr (requires { bv.reset(); })
    bv.reset();
  else if constexpr (requires { bv.clear_bv(); })
    bv.clear_bv();

  // 0 and 1 are ignored (assumed 0/prime-like in bitset state)
  // Sieve marks composites starting from 4

  for (std::uint64_t p = 2; p * p < N; ++p)
  {
    bool is_prime = false;
    // Check if p is prime (bit not set)
    // std::bitset and bitvector interface access
    if constexpr (requires { bv.test(p); })
      is_prime = !bv.test(p);
    else
      is_prime = !bv[p];

    if (is_prime)
    {
      for (std::uint64_t i = p * p; i < N; i += p)
        bv.set(i);
    }
  }
}

template<std::uint64_t N>
void run_comparison()
{
  std::println("--- Benchmarking N = {} ---", N);

  // Custom Lambda
  auto run_custom = []() {
    auto bv = lam::bitvec::sieve_of_eratosthenes(N);
    // Count primes: (N-2) - composites
    return (N - 2) - bv.count();
  };

  // Std Lambda
  auto run_std = []() {
    std::bitset<N> bv; // Stack allocation might fail for huge N
    run_sieve<N>(bv);
    return (N - 2) - bv.count();
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
  for (int i = 0; i < iterations; ++i)
    volatile auto v = run_custom();
  auto end_custom = std::chrono::steady_clock::now();

  // Measure Std
  auto start_std = std::chrono::steady_clock::now();
  for (int i = 0; i < iterations; ++i)
    volatile auto v = run_std();
  auto end_std = std::chrono::steady_clock::now();

  auto dur_custom = std::chrono::duration<double, std::milli>(end_custom - start_custom).count() / iterations;
  auto dur_std = std::chrono::duration<double, std::milli>(end_std - start_std).count() / iterations;

  std::println("lam::bitvector : {:.3f} ms", dur_custom);
  std::println("std::bitset    : {:.3f} ms", dur_std);
  std::println("Ratio          : {:.2f}x\n", dur_custom / dur_std);
}

int main()
{
  std::println("lam::bitvector v{} ({}) [{}]", lam::bitvec::config::version_string, lam::bitvec::config::build_type,
               lam::bitvec::config::git_hash);
  std::println("Benchmarking Sieve of Eratosthenes (Averaging {} runs)\n", iterations);

  run_comparison<100'000>();
  run_comparison<262'144>(); // 2^18
  run_comparison<524'288>(); // 2^19
  run_comparison<1'000'000>();

  run_comparison<10'000'000>();
  run_comparison<15'000'000>();
  run_comparison<20'000'000>();

  std::println("\n--- Segmented Sieve Comparison ---");
  auto run_seg_bench = []<std::uint64_t N>() {
    std::println("--- Benchmarking N = {} ---", N);

    // Simple
    auto t1 = std::chrono::steady_clock::now();
    for (int i = 0; i < 3; ++i)
      volatile auto b = lam::bitvec::sieve_of_eratosthenes(N);
    auto t2 = std::chrono::steady_clock::now();

    // Segmented (using default size)
    auto t3 = std::chrono::steady_clock::now();
    for (int i = 0; i < 3; ++i)
      volatile auto b = lam::bitvec::sieve_of_eratosthenes_segmented(N);
    auto t4 = std::chrono::steady_clock::now();

    double ms_simple = std::chrono::duration<double, std::milli>(t2 - t1).count() / 3.0;
    double ms_seg = std::chrono::duration<double, std::milli>(t4 - t3).count() / 3.0;

    std::println("Simple:    {:.2f} ms", ms_simple);
    std::println("Segmented: {:.2f} ms", ms_seg);
    std::println("Speedup:   {:.2f}x\n", ms_simple / ms_seg);
  };

  run_seg_bench.template operator()<10'000'000>();
  run_seg_bench.template operator()<100'000'000>();
  run_seg_bench.template operator()<1'000'000'000>();
}
