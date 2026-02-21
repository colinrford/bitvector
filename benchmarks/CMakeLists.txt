
# Increase constexpr steps significantly for heavy compile-time computations
set(CLANG_CONSTEXPR_STEPS 900000000)

add_executable(eratosthenes ../examples/eratosthenes.cpp)
target_compile_features(eratosthenes PRIVATE cxx_std_23)
target_link_libraries(eratosthenes PRIVATE bitvector)
target_compile_options(eratosthenes PRIVATE
  "-fconstexpr-steps=${CLANG_CONSTEXPR_STEPS}"
)

add_executable(bitvector_benchmark benchmark.cpp)
target_compile_features(bitvector_benchmark PRIVATE cxx_std_23)
target_link_libraries(bitvector_benchmark PRIVATE bitvector)

add_executable(bench_compile_custom bench_compile_custom.cpp)
target_link_libraries(bench_compile_custom PRIVATE bitvector)
target_compile_features(bench_compile_custom PRIVATE cxx_std_23)
target_compile_options(bench_compile_custom PRIVATE "-fconstexpr-steps=900000000")
if(TEST_SIZE)
  target_compile_definitions(bench_compile_custom PRIVATE TEST_SIZE=${TEST_SIZE})
endif()

add_executable(bench_compile_std bench_compile_std.cpp)
target_compile_features(bench_compile_std PRIVATE cxx_std_23)
target_compile_options(bench_compile_std PRIVATE "-fconstexpr-steps=900000000")
if(TEST_SIZE)
  target_compile_definitions(bench_compile_std PRIVATE TEST_SIZE=${TEST_SIZE})
endif()

add_executable(allocator_benchmark allocator_benchmark.cpp)
target_compile_features(allocator_benchmark PRIVATE cxx_std_23)
target_link_libraries(allocator_benchmark PRIVATE bitvector)

add_executable(parallel_sieve_benchmark parallel_sieve_benchmark.cpp)
target_compile_features(parallel_sieve_benchmark PRIVATE cxx_std_23)
#find_package(Threads REQUIRED)
#target_link_libraries(parallel_sieve_benchmark PRIVATE bitvector Threads::Threads)
target_link_libraries(parallel_sieve_benchmark PRIVATE bitvector)
