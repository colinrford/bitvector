/*
 *  sieve_bench.cpp – Colin Ford
 *    see github.com/colinrford/bitvector
 *    currently unlicensed
 *  runs one sieve at one size, for timing the whole process with hyperfine:
 *    sieve_bench <simple|segmented|atkin> <n> [reps]
 */

import std;
import lam.bitvector;

namespace
{

int usage()
{
  std::println(std::cerr, "usage: sieve_bench <simple|segmented|atkin> <n> [reps]");
  return 2;
}

std::optional<std::uint64_t> parse(std::string_view s)
{
  std::uint64_t value = 0;
  auto [end, ec] = std::from_chars(s.data(), s.data() + s.size(), value);
  if (ec != std::errc{} || end != s.data() + s.size())
    return std::nullopt;
  return value;
}

} // namespace

int main(int argc, char** argv)
{
  if (argc < 3 || argc > 4)
    return usage();

  std::string_view sieve = argv[1];

  if (sieve != "simple" && sieve != "segmented" && sieve != "atkin")
    return usage();

  auto n = parse(argv[2]);
  auto reps = argc == 4 ? parse(argv[3]) : std::optional<std::uint64_t>{1};
  if (!n || !reps || *reps == 0)
    return usage();

  auto run = [&]() -> std::size_t {
    if (sieve == "simple")
      return lam::bitvec::sieve_of_eratosthenes(*n).count();
    if (sieve == "segmented")
      return lam::bitvec::sieve_of_eratosthenes_segmented(*n).count();
    if (sieve == "atkin")
      return lam::bitvec::sieve_of_atkin(*n).count();
    return std::numeric_limits<std::size_t>::max();
  };

  // printing the count keeps the sieve from being optimized away
  std::size_t count = 0;
  for (std::uint64_t i = 0; i < *reps; ++i)
    count = run();
  if (count == std::numeric_limits<std::size_t>::max())
    return usage();

  std::println("{} {} -> {} primes", sieve, *n, count);
  return 0;
}
