`lam.bitvector`
=========
This is my bitvector library. It was originally written in C for
an undergraduate course, *Programming in C*. It's much different
now.

## Bitvector, a LAM library
Not much more than another bitvector at this time, although it is
appreciably faster than `std::bitset` in my limited micro
benchmarks (hopefully part of the 1% that matters). Here are some
current features of the c++ module `lam.bitvector`:

- **Modules-First**: Built as a native C++23 module (`import lam.bitvector;`).
- **Fully `constexpr`**: Usable in compile-time contexts, including dynamic allocation.
- **Allocator-Aware & Concepts-Driven**: custom concept `bitvector_allocator_c` with full support for `std::pmr` and custom allocators.
- **Memory Safety**: extensive validation via AddressSanitizer (ASan) and UndefinedBehaviorSanitizer (UBSan).
- **Modern IO**: Built-in support for `std::print` and `std::format`.
- **High Performance**: Optimized for cache locality; faster than `std::bitset` in local parallel/chunked benchmarks.
- **Core Guidelines Compliant**: Clean, modern C++ codebase verified by `clang-tidy`.

Currently this software is unlicensed. It is subject to change both in form and in license at any time; I plan to endow an Apache 2.0 with LLVM exceptions license when the time is right. Before then I would like to test it with a few other projects.

## Usage

```cpp
import std;
import lam.bitvector;

int main() 
{
  lam::bitvector<> bv(64);
  bv.set(42);
  bv.toggle(10);
  
  std::println("Bit 42 is: {}", bv[42]);
  std::println("Population count: {}", bv.count());
}
```

## Building and Testing

Requires a C++23 compliant compiler (Clang 16+ recommended for Modules support) and CMake 3.28+.

```bash
# Build (Release)
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Run Tests
ctest --test-dir build

# Run Benchmarks
./build/benchmark
```
### Configuration

You can enable sanitizers for debug builds using CMake options:

| Option | Description | Default |
| :--- | :--- | :--- |
| `LAM_ENABLE_ASAN` | Enable AddressSanitizer (Memory safety) | `OFF` |
| `LAM_ENABLE_UBSAN` | Enable UndefinedBehaviorSanitizer | `OFF` |
| `LAM_ENABLE_TSAN` | Enable ThreadSanitizer (Race detection) | `OFF` |

Example:
```bash
cmake -B build -DLAM_ENABLE_ASAN=ON -DLAM_ENABLE_UBSAN=ON
```

## Performance

Results from Sieve of Eratosthenes (N=100,000,000) on local hardware:

| Implementation | Time (ms) | Speedup |
| :--- | :--- | :--- |
| **Sequential** | ~612 ms | 1.0x |
| **Parallel (8 threads)** | ~144 ms | **4.2x** |
| **Chunked (L1 Cache)** | ~95 ms | **6.4x** |

*Measured with `std::chrono::steady_clock` in Release mode.*
