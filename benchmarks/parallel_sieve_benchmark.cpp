/*
 *  parallel_sieve_benchmark.cpp – written by Colin Ford
 *    see github.com/colinrford/bitvector
 *    currently unlicensed
 */

import std;
import lam.bitvector;

using lam::bitvec::bitvector;

// Configuration
constexpr std::uint64_t N = 100'000'000;  // Total range
constexpr int NUM_THREADS = 8;
constexpr int ITERATIONS = 3;

// =============================================================================
// Segmented Sieve Algorithm
// =============================================================================

// Compute base primes up to sqrt(N) using simple sieve
template <typename Allocator = std::allocator<std::uint64_t>>
std::vector<std::uint64_t> sieve_base_primes(std::uint64_t limit, const Allocator& alloc = Allocator())
{
  bitvector<Allocator> is_composite(limit + 1, alloc);
  std::vector<std::uint64_t> primes;
  
  for (std::uint64_t p = 2; p <= limit; ++p)
  {
    if (!is_composite[p])
    {
      primes.push_back(p);
      for (std::uint64_t m = p * p; m <= limit; m += p)
        is_composite.set(m);
    }
  }
  return primes;
}

// Sieve a segment [low, high) using pre-computed base primes
template <typename Allocator = std::allocator<std::uint64_t>>
std::uint64_t sieve_segment(
    std::uint64_t low, 
    std::uint64_t high,
    const std::vector<std::uint64_t>& base_primes,
    const Allocator& alloc = Allocator())
{
  std::uint64_t segment_size = high - low;
  bitvector<Allocator> is_composite(segment_size, alloc);
  
  // Mark composites in this segment
  for (std::uint64_t p : base_primes)
  {
    // Find first multiple of p >= low
    std::uint64_t start = ((low + p - 1) / p) * p;
    if (start == p) 
      start = p * p;  // Don't mark prime itself
    if (start < low) 
      start += p;
    
    for (std::uint64_t m = start; m < high; m += p)
      is_composite.set(m - low);
  }
  
  // Count primes in segment
  std::uint64_t count = 0;
  for (std::uint64_t i = 0; i < segment_size; ++i)
    if (!is_composite[i] && (low + i) >= 2)  // 0 and 1 are not prime
      ++count;
  return count;
}

// =============================================================================
// Benchmarks
// =============================================================================

void benchmark_sequential()
{
  std::println("--- Sequential Baseline ---");
  
  auto sqrt_n = static_cast<std::uint64_t>(std::sqrt(static_cast<double>(N))) + 1;
  auto base_primes = sieve_base_primes(sqrt_n);
  
  auto start = std::chrono::steady_clock::now();
  std::uint64_t total_count = 0;
  
  for (int iter = 0; iter < ITERATIONS; ++iter)
  {
    total_count = base_primes.size();  // Count base primes
    total_count += sieve_segment(sqrt_n + 1, N, base_primes);
  }
  
  auto end = std::chrono::steady_clock::now();
  double avg_ms = std::chrono::duration<double, std::milli>(end - start).count() / ITERATIONS;
  
  std::println("Prime count: {}", total_count);
  std::println("Time: {:.2f} ms\n", avg_ms);
}

// Cache-line aligned count to avoid false sharing
struct alignas(std::hardware_destructive_interference_size) PaddedCount
{ std::uint64_t value = 0; };

void benchmark_parallel_std_allocator()
{
  std::println("--- Parallel (std::allocator, {} threads) ---", NUM_THREADS);
  
  auto sqrt_n = static_cast<std::uint64_t>(std::sqrt(static_cast<double>(N))) + 1;
  auto base_primes = sieve_base_primes(sqrt_n);
  
  auto start = std::chrono::steady_clock::now();
  std::uint64_t total_count = 0;
  
  for (int iter = 0; iter < ITERATIONS; ++iter)
  {
    std::vector<std::jthread> threads;
    std::vector<PaddedCount> counts(NUM_THREADS);  // Cache-line padded
    
    std::uint64_t segment_size = (N - sqrt_n - 1) / NUM_THREADS;
    
    for (int t = 0; t < NUM_THREADS; ++t)
    {
      std::uint64_t low = sqrt_n + 1 + t * segment_size;
      std::uint64_t high = (t == NUM_THREADS - 1) ? N : low + segment_size;
      
      threads.emplace_back([&, t, low, high]() {
        counts[t].value = sieve_segment(low, high, base_primes);
      });
    }
    
    threads.clear();  // Join all threads
    
    total_count = base_primes.size();
    for (auto& c : counts) 
      total_count += c.value;
  }
  
  auto end = std::chrono::steady_clock::now();
  double avg_ms = std::chrono::duration<double, std::milli>(end - start).count() / ITERATIONS;
  
  std::println("Prime count: {}", total_count);
  std::println("Time: {:.2f} ms\n", avg_ms);
}

void benchmark_parallel_pmr_thread_local()
{
  std::println("--- Parallel (PMR thread-local arenas, {} threads) ---", NUM_THREADS);
  
  auto sqrt_n = static_cast<std::uint64_t>(std::sqrt(static_cast<double>(N))) + 1;
  auto base_primes = sieve_base_primes(sqrt_n);
  
  // Pre-allocate buffers for each thread (avoid allocation during timing)
  std::uint64_t segment_size = (N - sqrt_n - 1) / NUM_THREADS;
  std::size_t buffer_size = (segment_size / 8) + 4096;  // bits -> bytes + padding
  std::vector<std::vector<std::byte>> thread_buffers(NUM_THREADS);
  for (auto& buf : thread_buffers) 
    buf.resize(buffer_size);
  
  auto start = std::chrono::steady_clock::now();
  std::uint64_t total_count = 0;
  
  for (int iter = 0; iter < ITERATIONS; ++iter)
  {
    std::vector<std::jthread> threads;
    std::vector<PaddedCount> counts(NUM_THREADS);  // Cache-line padded
    
    for (int t = 0; t < NUM_THREADS; ++t)
    {
      std::uint64_t low = sqrt_n + 1 + t * segment_size;
      std::uint64_t high = (t == NUM_THREADS - 1) ? N : low + segment_size;
      
      threads.emplace_back([&, t, low, high]() {
        // Each thread gets its own arena - zero contention!
        std::pmr::monotonic_buffer_resource pool(
            thread_buffers[t].data(), 
            thread_buffers[t].size(),
            std::pmr::null_memory_resource());
        std::pmr::polymorphic_allocator<std::uint64_t> alloc(&pool);
        
        counts[t].value = sieve_segment(low, high, base_primes, alloc);
      });
    }
    
    threads.clear();  // Join all threads
    
    total_count = base_primes.size();
    for (auto& c : counts) total_count += c.value;
  }
  
  auto end = std::chrono::steady_clock::now();
  double avg_ms = std::chrono::duration<double, std::milli>(end - start).count() / ITERATIONS;
  
  std::println("Prime count: {}", total_count);
  std::println("Time: {:.2f} ms\n", avg_ms);
}

// =============================================================================
// Chunked Allocation Benchmark - Cache-sized chunks
// =============================================================================

// Chunk sizes to test (bits)
constexpr std::size_t CHUNK_32KB = 32 * 1024 * 8;    // 32KB = 256K bits (L1 data cache)
constexpr std::size_t CHUNK_64KB = 64 * 1024 * 8;    // 64KB = 512K bits (total L1 cache)
constexpr std::size_t CHUNK_256KB = 256 * 1024 * 8;  // 256KB = 2M bits (L2 cache)

template <std::size_t CHUNK_BITS, typename Allocator = std::allocator<std::uint64_t>>
std::uint64_t sieve_segment_chunked(
    std::uint64_t low, 
    std::uint64_t high,
    const std::vector<std::uint64_t>& base_primes,
    const Allocator& alloc = Allocator())
{
  std::uint64_t count = 0;
  
  for (std::uint64_t chunk_low = low; chunk_low < high; chunk_low += CHUNK_BITS)
  {
    std::uint64_t chunk_high = std::min(chunk_low + CHUNK_BITS, high);
    std::uint64_t chunk_size = chunk_high - chunk_low;
    
    bitvector<Allocator> is_composite(chunk_size, alloc);
    
    for (std::uint64_t p : base_primes)
    {
      std::uint64_t start = ((chunk_low + p - 1) / p) * p;
      if (start == p) start = p * p;
      if (start < chunk_low) start += p;
      
      for (std::uint64_t m = start; m < chunk_high; m += p)
        is_composite.set(m - chunk_low);
    }
    
    for (std::uint64_t i = 0; i < chunk_size; ++i)
      if (!is_composite[i] && (chunk_low + i) >= 2)
        ++count;
  }
  
  return count;
}

template <std::size_t CHUNK_BITS>
void benchmark_chunked_std_allocator(const char* chunk_name)
{
  std::println("--- Chunked std::allocator ({}, {} threads) ---", chunk_name, NUM_THREADS);
  
  auto sqrt_n = static_cast<std::uint64_t>(std::sqrt(static_cast<double>(N))) + 1;
  auto base_primes = sieve_base_primes(sqrt_n);
  
  auto start = std::chrono::steady_clock::now();
  std::uint64_t total_count = 0;
  
  for (int iter = 0; iter < ITERATIONS; ++iter)
  {
    std::vector<std::jthread> threads;
    std::vector<PaddedCount> counts(NUM_THREADS);
    
    std::uint64_t segment_size = (N - sqrt_n - 1) / NUM_THREADS;
    
    for (int t = 0; t < NUM_THREADS; ++t)
    {
      std::uint64_t low = sqrt_n + 1 + t * segment_size;
      std::uint64_t high = (t == NUM_THREADS - 1) ? N : low + segment_size;
      
      threads.emplace_back([&, t, low, high]() {
        counts[t].value = sieve_segment_chunked<CHUNK_BITS>(low, high, base_primes);
      });
    }
    
    threads.clear();
    
    total_count = base_primes.size();
    for (auto& c : counts) 
      total_count += c.value;
  }
  
  auto end = std::chrono::steady_clock::now();
  double avg_ms = std::chrono::duration<double, std::milli>(end - start).count() / ITERATIONS;
  
  std::println("Prime count: {}", total_count);
  std::println("Time: {:.2f} ms\n", avg_ms);
}

template <std::size_t CHUNK_BITS>
void benchmark_chunked_pmr(const char* chunk_name)
{
  std::println("--- Chunked PMR ({}, {} threads) ---", chunk_name, NUM_THREADS);
  
  auto sqrt_n = static_cast<std::uint64_t>(std::sqrt(static_cast<double>(N))) + 1;
  auto base_primes = sieve_base_primes(sqrt_n);
  
  std::uint64_t segment_size = (N - sqrt_n - 1) / NUM_THREADS;
  std::size_t num_chunks = (segment_size + CHUNK_BITS - 1) / CHUNK_BITS;
  std::size_t buffer_size = num_chunks * (CHUNK_BITS / 8 + 64);
  
  std::vector<std::vector<std::byte>> thread_buffers(NUM_THREADS);
  for (auto& buf : thread_buffers) 
    buf.resize(buffer_size);
  
  auto start = std::chrono::steady_clock::now();
  std::uint64_t total_count = 0;
  
  for (int iter = 0; iter < ITERATIONS; ++iter)
  {
    std::vector<std::jthread> threads;
    std::vector<PaddedCount> counts(NUM_THREADS);
    
    for (int t = 0; t < NUM_THREADS; ++t)
    {
      std::uint64_t low = sqrt_n + 1 + t * segment_size;
      std::uint64_t high = (t == NUM_THREADS - 1) ? N : low + segment_size;
      
      threads.emplace_back([&, t, low, high]() {
        std::pmr::monotonic_buffer_resource pool(
            thread_buffers[t].data(), 
            thread_buffers[t].size(),
            std::pmr::null_memory_resource());
        std::pmr::polymorphic_allocator<std::uint64_t> alloc(&pool);
        
        counts[t].value = sieve_segment_chunked<CHUNK_BITS>(low, high, base_primes, alloc);
      });
    }
    
    threads.clear();
    
    total_count = base_primes.size();
    for (auto& c : counts) 
      total_count += c.value;
  }
  
  auto end = std::chrono::steady_clock::now();
  double avg_ms = std::chrono::duration<double, std::milli>(end - start).count() / ITERATIONS;
  
  std::println("Prime count: {}", total_count);
  std::println("Time: {:.2f} ms\n", avg_ms);
}

int main()
{
  std::println("lam::bitvector v{} ({}) [{}]", 
               lam::bitvec::config::version_string,
               lam::bitvec::config::build_type,
               lam::bitvec::config::git_hash);
  std::println("=== Parallel Sieve Benchmark ===");
  std::println("N = {}, Threads = {}, Iterations = {}", N, NUM_THREADS, ITERATIONS);
  std::println("Cache line size: {} bytes\n", std::hardware_destructive_interference_size);
  
  benchmark_sequential();
  benchmark_parallel_std_allocator();
  benchmark_parallel_pmr_thread_local();
  
  std::println("=== Chunked Allocation Tests ===\n");
  
  benchmark_chunked_std_allocator<CHUNK_32KB>("32KB / L1 data cache");
  benchmark_chunked_pmr<CHUNK_32KB>("32KB / L1 data cache");
  
  benchmark_chunked_std_allocator<CHUNK_64KB>("64KB / total L1 cache");
  benchmark_chunked_pmr<CHUNK_64KB>("64KB / total L1 cache");
  
  benchmark_chunked_std_allocator<CHUNK_256KB>("256KB / L2 cache");
  benchmark_chunked_pmr<CHUNK_256KB>("256KB / L2 cache");
  
  std::println("=== Done ===");
}
