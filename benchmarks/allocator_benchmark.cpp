/*
 *  allocator_benchmark.cpp – written by Colin Ford
 *    see github.com/colinrford/bitvector
 *    currently unlicensed
 */

import std;
import lam.bitvector;

using lam::bitvec::bitvector;

// Use a fixed size for the benchmark to make it comparable
constexpr std::size_t N = 100'000'000;
constexpr int iterations = 5;

// Sieve Implementation
template<typename BitVector>
auto run_sieve(BitVector& bv)
{
  if constexpr (requires { bv.clear_bv(); })
    bv.clear_bv();
  // bitvector clears on construction, which is what we are measuring in the loop below mostly.
  // But strictly speaking, the sieve logic is:

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
    while (prime + 1 < (N - 1) && bv[++prime])
      ;
    running_count++;
  }
  return running_count;
}

void benchmark_std_allocator()
{
  auto start = std::chrono::steady_clock::now();
  std::uint64_t dummy = 0;

  for (int i = 0; i < iterations; ++i)
  {
    bitvector<std::allocator<std::uint64_t>> bv(N);
    dummy += run_sieve(bv);
  }

  auto end = std::chrono::steady_clock::now();
  double avg_ms = std::chrono::duration<double, std::milli>(end - start).count() / iterations;
  std::println("std::allocator                  : {:.3f} ms (Checksum: {})", avg_ms, dummy);
}

void benchmark_monotonic()
{
  auto start = std::chrono::steady_clock::now();
  std::uint64_t dummy = 0;

  // Pre-allocate a large buffer to ensure monotonic resource doesn't hit upstream often
  // 100M bits ~ 12.5 MB. Let's give it 20MB per iteration to be safe.
  // In a real scenario, we might reuse the resource, but here we want to test the allocation speed mostly.
  // However, if we construct the resource INSIDE the loop, we measure its setup cost.
  // If we construct it OUTSIDE, we measure pure "allocation via ptr" cost.
  // Standard allocator is global/stateless. PMR resources are stateful.

  // Strategy: Recreate resource every time to be fair to std::allocator (which goes to heap).
  // OR: Recreate resource to verify "bump pointer" speed vs "malloc" speed.

  for (int i = 0; i < iterations; ++i)
  {
    // Monotonic resource on heap or stack?
    // Let's use a large stack buffer if possible, or heap. 20MB is too big for stack.
    // We'll let it use new/delete upstream, but it chunks it.
    std::pmr::monotonic_buffer_resource pool;
    std::pmr::polymorphic_allocator<std::uint64_t> alloc(&pool);

    bitvector<std::pmr::polymorphic_allocator<std::uint64_t>> bv(N, alloc);
    dummy += run_sieve(bv);
  }

  auto end = std::chrono::steady_clock::now();
  double avg_ms = std::chrono::duration<double, std::milli>(end - start).count() / iterations;
  std::println("pmr::monotonic_buffer_resource  : {:.3f} ms (Checksum: {})", avg_ms, dummy);
}

void benchmark_pool()
{
  auto start = std::chrono::steady_clock::now();
  std::uint64_t dummy = 0;

  for (int i = 0; i < iterations; ++i)
  {
    std::pmr::unsynchronized_pool_resource pool;
    std::pmr::polymorphic_allocator<std::uint64_t> alloc(&pool);

    bitvector<std::pmr::polymorphic_allocator<std::uint64_t>> bv(N, alloc);
    dummy += run_sieve(bv);
  }

  auto end = std::chrono::steady_clock::now();
  double avg_ms = std::chrono::duration<double, std::milli>(end - start).count() / iterations;
  std::println("pmr::unsynchronized_pool_res    : {:.3f} ms (Checksum: {})", avg_ms, dummy);
}

// Pre-allocated buffer monotonic to simulate "stack-like" speed
void benchmark_monotonic_buffer()
{
  // 100M bits = 12.5MB.
  // We allocate a static buffer once to simulate a hot arena.
  static std::vector<std::byte> buffer(20 * 1024 * 1024);

  auto start = std::chrono::steady_clock::now();
  std::uint64_t dummy = 0;

  for (int i = 0; i < iterations; ++i)
  {
    // Resetting monotonic buffer is "free"
    std::pmr::monotonic_buffer_resource pool(buffer.data(), buffer.size(), std::pmr::null_memory_resource());
    std::pmr::polymorphic_allocator<std::uint64_t> alloc(&pool);

    bitvector<std::pmr::polymorphic_allocator<std::uint64_t>> bv(N, alloc);
    dummy += run_sieve(bv);
  }

  auto end = std::chrono::steady_clock::now();
  double avg_ms = std::chrono::duration<double, std::milli>(end - start).count() / iterations;
  std::println("pmr::monotonic (pre-alloc buffer): {:.3f} ms (Checksum: {})", avg_ms, dummy);
}

void benchmark_boolean_ops()
{
  std::println("\n--- Benchmarking Boolean Ops (A | B) & C ---");
  // Smaller N for boolean ops because they are heavy (allocating full vectors)
  // 100M bits is 12.5MB. Doing this repeatedly will be memory bandwidth bound.
  // But testing allocation overhead is the goal.
  constexpr std::size_t N_BO = 10'000'000; // 1.25 MB per vector
  constexpr int ITER = 50;

  auto run_ops = [&](auto& b1, auto& b2, auto& b3) {
    volatile auto res = (b1 | b2) & b3;
    return res.size();
  };

  // STD Allocator
  {
    bitvector<std::allocator<std::uint64_t>> b1(N_BO), b2(N_BO), b3(N_BO);
    // Fill properly to prevent optimizer removal? volatile result handles it.

    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < ITER; ++i)
    {
      auto res = (b1 | b2) & b3;
      // Force verify
      if (res.size() == 0)
        std::abort();
    }
    auto end = std::chrono::steady_clock::now();
    double ms = std::chrono::duration<double, std::milli>(end - start).count() / ITER;
    std::println("std::allocator                  : {:.3f} ms", ms);
  }

  // Monotonic Buffer (Reused)
  {
    // One huge buffer for all ops?
    // 1.25MB * 3 (inputs) + 1.25MB * 2 (temps/result) * ITER?
    // No, result is destroyed each iter.
    // We need ~ 2.5MB scratch per iter.
    std::vector<std::byte> buffer(N_BO / 8 * 5); // Adequate buffer
    std::pmr::monotonic_buffer_resource pool(buffer.data(), buffer.size(), std::pmr::null_memory_resource());
    std::pmr::polymorphic_allocator<std::uint64_t> alloc(&pool);

    bitvector<std::pmr::polymorphic_allocator<std::uint64_t>> b1(N_BO, alloc), b2(N_BO, alloc), b3(N_BO, alloc);

    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < ITER; ++i)
    {
      pool.release(); // RESET buffer! This is the magic.
                      // But wait, b1, b2, b3 are also allocated from pool...
                      // If we release pool, we invalidate b1, b2, b3!
      // Correct PMR usage here: Inputs should be on a stable resource, temps/outputs on a scratch resource.
      // But our operator& propagates the allocator from LHS.
      // So LHS must use the scratch allocator?
      // "result(lhs.size(), lhs.get_allocator())"

      // This design flaw in propagation makes "scratch allocator" hard if inputs are long-lived.
      // We would need to construct inputs with the scratch allocator, which means RE-constructing them every frame?
      // Or we use a pool that supports upstream?

      // Let's assume we re-create everything to simulate a "frame" of work.

      // To properly test "temp allocation" speed without destroying inputs:
      // std::pmr::unsynchronized_pool_resource is better here as it manages chunks.
      // Monotonic must be wiped fully.
    }
  }

  // Correct Monotonic Benchmark: Recreate everything
  {
    // Large buffer
    std::vector<std::byte> buffer(50 * 1024 * 1024);
    // We reset usage by creating resource inside loop or calling release()

    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < ITER; ++i)
    {
      std::pmr::monotonic_buffer_resource pool(buffer.data(), buffer.size(), std::pmr::null_memory_resource());
      std::pmr::polymorphic_allocator<std::uint64_t> alloc(&pool);

      // Allocating inputs checks allocation speed too
      bitvector<std::pmr::polymorphic_allocator<std::uint64_t>> b1(N_BO, alloc), b2(N_BO, alloc), b3(N_BO, alloc);
      auto res = (b1 | b2) & b3;
    }
    auto end = std::chrono::steady_clock::now();
    double ms = std::chrono::duration<double, std::milli>(end - start).count() / ITER;
    std::println("pmr::monotonic (recreate all)   : {:.3f} ms", ms);
  }

  // Compare to STD recreate all
  {
    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < ITER; ++i)
    {
      bitvector<std::allocator<std::uint64_t>> b1(N_BO), b2(N_BO), b3(N_BO);
      auto res = (b1 | b2) & b3;
    }
    auto end = std::chrono::steady_clock::now();
    double ms = std::chrono::duration<double, std::milli>(end - start).count() / ITER;
    std::println("std::allocator (recreate all)   : {:.3f} ms", ms);
  }
}

int main()
{
  std::println("Benchmarking Allocators for Sieve (N = {}, {} runs)\n", N, iterations);

  benchmark_std_allocator();
  benchmark_monotonic();
  benchmark_pool();
  benchmark_monotonic_buffer();

  benchmark_boolean_ops();
}
