#include "../../general/Tester.h"
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

namespace Lab1 {

template <typename T> using Vector = std::vector<T>;

template <typename T> class TestCaseGenerator {
public:
  TestCaseGenerator(size_t size) : size_(size) {}

  auto operator()([[maybe_unused]] int seed_) const -> Vector<T> {
    Vector<T> vec(size_, 0);
    for (size_t i = 0; i < size_; ++i) {
      vec[i] = static_cast<T>(i);
    }
    return vec;
  }

private:
  size_t size_;
};

template <typename T> class ArraySum {
public:
  // Trivial algorithm: sum elements one by one
  static auto trivial(const Vector<T> &input) -> T {
    T sum = 0;
    for (const auto &val : input) {
      sum += val;
    }
    return sum;
  }

  // Super-scalar optimization algorithm: two-way chain addition
  static auto two_way_chain(const Vector<T> &input) -> T {
    const size_t size = input.size();
    T sum1 = 0, sum2 = 0;

    // Parallel addition
    size_t i;
    for (i = 0; i + 1 < size; i += 2) {
      sum1 += input[i];
      sum2 += input[i + 1];
    }

    // Handle remaining elements
    if (i < size) {
      sum1 += input[i];
    }

    return sum1 + sum2;
  }

  // Super-scalar optimization algorithm: recursive two-by-two addition
  static auto recursive(const Vector<T> &input) -> T {
    return recursive_sum(input, 0, input.size());
  }

private:
  static auto recursive_sum(const Vector<T> &input, size_t start, size_t end)
      -> T {
    const size_t length = end - start;

    // Base cases
    if (length == 0)
      return 0;
    if (length == 1)
      return input[start];

    // Recursive division
    const size_t mid = start + (length / 2);
    return recursive_sum(input, start, mid) + recursive_sum(input, mid, end);
  }
};

} // namespace Lab1

int main() {
  constexpr size_t ARRAY_SIZE = 10000;
  using ArraySumDouble = Lab1::ArraySum<double>;
  using TestGen = Lab1::TestCaseGenerator<double>;

  General::PerformanceTester<TestGen> tester{TestGen{ARRAY_SIZE}};

  // Create results directory
  std::string output_dir = "results/array_sum";
  std::system("mkdir -p results/array_sum");

  // Use the new runAndSave method for testing
  auto perf_trivial = tester.runAndSave(ArraySumDouble::trivial, "Trivial",
                                        output_dir, 1000, 0);
  std::cout << "Trivial Algorithm: " << perf_trivial.str() << std::endl;

  auto perf_two_way = tester.runAndSave(ArraySumDouble::two_way_chain,
                                        "TwoWayChain", output_dir, 1000, 0);
  std::cout << "Two-Way Chain Algorithm: " << perf_two_way.str() << std::endl;

  auto perf_recursive = tester.runAndSave(ArraySumDouble::recursive,
                                          "Recursive", output_dir, 1000, 0);
  std::cout << "Recursive Algorithm: " << perf_recursive.str() << std::endl;

  std::cout << "Detailed test data saved to " << output_dir << " directory"
            << std::endl;

  return 0;
}
