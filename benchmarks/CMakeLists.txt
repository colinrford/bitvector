# Benchmarks. bitvector_add_executable (cmake/BitvectorHelpers.cmake) handles the
# lam.bitvector link + C++23; CONSTEXPR_LIMIT bumps the constexpr budget for the
# heavy compile-time benchmarks.

bitvector_add_executable(bitvector_benchmark    SOURCE benchmark.cpp)
bitvector_add_executable(allocator_benchmark    SOURCE allocator_benchmark.cpp)
bitvector_add_executable(parallel_sieve_benchmark SOURCE parallel_sieve_benchmark.cpp)

bitvector_add_executable(bench_compile_custom
  SOURCE          bench_compile_custom.cpp
  CONSTEXPR_LIMIT 900000000)
if(TEST_SIZE)
  target_compile_definitions(bench_compile_custom PRIVATE TEST_SIZE=${TEST_SIZE})
endif()

# bench_compile_std is the std-only baseline — it deliberately does NOT link
# lam.bitvector, so it stays a plain target rather than going through the helper.
add_executable(bench_compile_std bench_compile_std.cpp)
target_compile_features(bench_compile_std PRIVATE cxx_std_23)
lam_target_constexpr_limit(bench_compile_std 900000000)
if(TEST_SIZE)
  target_compile_definitions(bench_compile_std PRIVATE TEST_SIZE=${TEST_SIZE})
endif()
