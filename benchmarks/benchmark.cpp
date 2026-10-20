/*
 *  benchmark.cpp – Colin Ford
 *    see github.com/colinrford/bitvector
 *    currently unlicensed
 */

import std;
import lam.bitvector;

using lam::bitvec::bitvector;

// keeps results live so the timed work can't be optimized away
volatile std::uint64_t sink = 0;

// more repetitions for cheap sizes, fewer for expensive ones
constexpr int reps_for(std::uint64_t n)
{
  if (n <= 1'000'000)
    return 25;
  if (n <= 20'000'000)
    return 11;
  if (n <= 100'000'000)
    return 7;
  return 5;
}

// time one call; the result is consumed after the clock stops
template<typename F>
double time_once(F&& fn)
{
  auto start = std::chrono::steady_clock::now();
  auto result = fn();
  auto end = std::chrono::steady_clock::now();
  if constexpr (std::integral<decltype(result)>)
    sink = sink + result;
  else
    sink = sink + result.count();
  return std::chrono::duration<double, std::milli>(end - start).count();
}

struct summary
{
  double min_ms;
  double median_ms;
};

summary summarize(std::vector<double> ms)
{
  std::ranges::sort(ms);
  return {ms.front(), ms[ms.size() / 2]};
}

template<std::uint64_t N, typename BitSet>
void run_sieve(BitSet& bv)
{
  if constexpr (requires { bv.reset(); })
    bv.reset();
  else if constexpr (requires { bv.clear_bv(); })
    bv.clear_bv();

  for (std::uint64_t p = 2; p * p < N; ++p)
  {
    bool is_prime = false;
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
  constexpr int reps = reps_for(N);
  std::println("--- Benchmarking N = {} ({} runs each) ---", N, reps);

  // lam's sieve sets a bit for each prime
  auto run_custom = []() {
    auto bv = lam::bitvec::sieve_of_eratosthenes(N);
    return static_cast<std::uint64_t>(bv.count());
  };

  // run_sieve marks composites in the std::bitset
  auto run_std = []() {
    std::bitset<N> bv;
    run_sieve<N>(bv);
    return (N - 2) - bv.count();
  };

  auto count = run_custom();
  if (count != run_std())
  {
    std::println("ERROR: Mismatch for N={}", N);
    return;
  }
  std::println("Count for N={}: {}", N, count);

  // alternate the two so neither always runs second
  std::vector<double> custom_ms;
  std::vector<double> std_ms;
  for (int i = 0; i < reps; ++i)
  {
    custom_ms.push_back(time_once(run_custom));
    std_ms.push_back(time_once(run_std));
  }

  auto custom = summarize(custom_ms);
  auto stdbs = summarize(std_ms);

  std::println("                 min        median");
  std::println("lam::bitvector : {:8.3f} ms {:8.3f} ms", custom.min_ms, custom.median_ms);
  std::println("std::bitset    : {:8.3f} ms {:8.3f} ms", stdbs.min_ms, stdbs.median_ms);
  std::println("Ratio (min)    : {:.2f}x\n", custom.min_ms / stdbs.min_ms);
}

template<std::uint64_t N>
void run_seg_bench()
{
  constexpr int reps = reps_for(N);
  std::println("--- Benchmarking N = {} ({} runs each) ---", N, reps);

  auto expected = lam::bitvec::sieve_of_eratosthenes(N).count();
  if (lam::bitvec::sieve_of_eratosthenes_segmented(N).count() != expected
      || lam::bitvec::sieve_of_atkin(N).count() != expected)
  {
    std::println("ERROR: sieves disagree for N={}", N);
    return;
  }

  // alternate the three so none always runs in the same position
  std::vector<double> simple_ms;
  std::vector<double> seg_ms;
  std::vector<double> atkin_ms;
  for (int i = 0; i < reps; ++i)
  {
    simple_ms.push_back(time_once([] { return lam::bitvec::sieve_of_eratosthenes(N); }));
    seg_ms.push_back(time_once([] { return lam::bitvec::sieve_of_eratosthenes_segmented(N); }));
    atkin_ms.push_back(time_once([] { return lam::bitvec::sieve_of_atkin(N); }));
  }

  auto simple = summarize(simple_ms);
  auto seg = summarize(seg_ms);
  auto atkin = summarize(atkin_ms);

  std::println("            min          median       vs simple (min)");
  std::println("Simple:    {:9.2f} ms {:9.2f} ms", simple.min_ms, simple.median_ms);
  std::println("Segmented: {:9.2f} ms {:9.2f} ms   {:.2f}x", seg.min_ms, seg.median_ms, simple.min_ms / seg.min_ms);
  std::println("Atkin:     {:9.2f} ms {:9.2f} ms   {:.2f}x\n", atkin.min_ms, atkin.median_ms,
               simple.min_ms / atkin.min_ms);
}

int main()
{
  std::println("lam::bitvector v{} ({}) [{}]", lam::bitvec::config::version_string, lam::bitvec::config::build_type,
               lam::bitvec::config::git_hash);
  std::println("Benchmarking Sieve of Eratosthenes (min and median of repeated runs)\n");

  run_comparison<100'000>();
  run_comparison<262'144>(); // 2^18
  run_comparison<524'288>(); // 2^19
  run_comparison<1'000'000>();

  run_comparison<10'000'000>();
  run_comparison<15'000'000>();
  run_comparison<20'000'000>();

  std::println("\n--- Sieve Comparison: simple, segmented, Atkin ---");
  run_seg_bench<10'000'000>();
  run_seg_bench<100'000'000>();
  run_seg_bench<1'000'000'000>();
}
