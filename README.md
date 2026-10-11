`lam.bitvector`
=========
This is my bitvector library. It was originally written in C for
an undergraduate course, *Programming in C*. It's much different
now.

## Bitvector, a [LAM](https://www.github.com/colinrford/lam) library
Not much more than another bitvector at this time. Here are some
current features of the c++ module `lam.bitvector`:

- **C++ module**: `import lam.bitvector;`, built on `import std;`.
- **`constexpr` capable**: Usable in compile-time contexts.
- **Formatting**: Built-in support for `std::print` and `std::format`.

Currently this software is unlicensed. It is subject to change both in form and in license at any time; I plan to endow a license when the time is right. Before then I would like to test it with a few other projects.

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

Requires CMake 3.31.6+ and a c++23 compiler that CMake can use to build `import std`, which these days is Clang 18+, GCC 15.2+.

```bash
# Build (Release)
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Run Tests
ctest --test-dir build
```
