cmake_minimum_required(VERSION 3.31.6 FATAL_ERROR)

if(CMAKE_VERSION VERSION_GREATER_EQUAL 4.0.3)
  set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")
elseif(CMAKE_VERSION VERSION_GREATER_EQUAL 4.0.0)
  set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "a9e1cf81-9932-4810-974b-6eccaf14e457")
elseif(CMAKE_VERSION VERSION_GREATER_EQUAL 3.31.6)
  set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "0e5b6991-d74f-4b3d-a41c-cf096e0b2508")
else()
  message(FATAL_ERROR "Version lower than 3.31.6 not supported (by me)")
endif()

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED True) #is this needed?
set(CMAKE_CXX_MODULE_STD ON)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

project(lam_bitvector VERSION 0.1.260103 LANGUAGES CXX)

find_package(Git)
if(GIT_FOUND)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
else()
  set(GIT_HASH "unknown")
endif()

if(CMAKE_BUILD_TYPE AND CMAKE_BUILD_TYPE MATCHES "^[Dd][Ee][Bb][Uu][Gg]$")
  set(IS_DEBUG "true")
else()
  set(IS_DEBUG "false")
endif()

configure_file(
  config.cppm.in
  ${CMAKE_CURRENT_BINARY_DIR}/config.cppm
)

# Increase constexpr steps significantly for heavy compile-time computations
set(CLANG_CONSTEXPR_STEPS 900000000)
add_executable(eratosthenes examples/eratosthenes.cpp)
target_compile_features(eratosthenes PRIVATE cxx_std_23)
target_link_libraries(eratosthenes PRIVATE bitvector)
target_compile_options(eratosthenes PRIVATE
  "-fconstexpr-steps=${CLANG_CONSTEXPR_STEPS}"
)
add_executable(benchmark benchmarks/benchmark.cpp)
target_compile_features(benchmark PRIVATE cxx_std_23)
target_link_libraries(benchmark PRIVATE bitvector)

# Sanitizer Options (all OFF by default for performance)
option(LAM_ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(LAM_ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)
option(LAM_ENABLE_TSAN "Enable ThreadSanitizer (cannot combine with ASan)" OFF)

if(LAM_ENABLE_TSAN AND LAM_ENABLE_ASAN)
  message(FATAL_ERROR "Cannot enable both TSan and ASan simultaneously")
endif()

if(LAM_ENABLE_ASAN)
  add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
  add_link_options(-fsanitize=address)
endif()

if(LAM_ENABLE_UBSAN)
  add_compile_options(-fsanitize=undefined -fno-omit-frame-pointer)
  add_link_options(-fsanitize=undefined)
endif()

if(LAM_ENABLE_TSAN)
  add_compile_options(-fsanitize=thread -fno-omit-frame-pointer)
  add_link_options(-fsanitize=thread)
endif()

add_library(bitvector)
add_library(lam::bitvector ALIAS bitvector)
set_target_properties(bitvector PROPERTIES OUTPUT_NAME "lam_bitvector")
target_sources(bitvector
  PUBLIC
  FILE_SET CXX_MODULES
  TYPE CXX_MODULES
  FILES src/cpp/bitvector.cppm
        src/cpp/bitvector-core.cppm
        src/cpp/bitvector-boolean_ops.cppm
        ${CMAKE_CURRENT_BINARY_DIR}/config.cppm
)

target_compile_features(bitvector PUBLIC cxx_std_23)

add_executable(bench_compile_custom benchmarks/bench_compile_custom.cpp)
target_link_libraries(bench_compile_custom PRIVATE bitvector)
target_compile_features(bench_compile_custom PRIVATE cxx_std_23)
target_compile_options(bench_compile_custom PRIVATE "-fconstexpr-steps=900000000")
if(TEST_SIZE)
  target_compile_definitions(bench_compile_custom PRIVATE TEST_SIZE=${TEST_SIZE})
endif()

add_executable(bench_compile_std benchmarks/bench_compile_std.cpp)
target_compile_features(bench_compile_std PRIVATE cxx_std_23)
target_compile_options(bench_compile_std PRIVATE "-fconstexpr-steps=900000000")
if(TEST_SIZE)
  target_compile_definitions(bench_compile_std PRIVATE TEST_SIZE=${TEST_SIZE})
endif()

add_executable(allocator_benchmark benchmarks/allocator_benchmark.cpp)
target_compile_features(allocator_benchmark PRIVATE cxx_std_23)
target_link_libraries(allocator_benchmark PRIVATE bitvector)

add_executable(parallel_sieve_benchmark benchmarks/parallel_sieve_benchmark.cpp)
target_compile_features(parallel_sieve_benchmark PRIVATE cxx_std_23)
target_link_libraries(parallel_sieve_benchmark PRIVATE bitvector)

enable_testing()
add_subdirectory(tests)

