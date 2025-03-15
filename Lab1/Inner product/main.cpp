#include "../../general/Tester.h"
#include <cmath>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace Lab1 {

template <typename T> using Matrix = std::vector<std::vector<T>>;
template <typename T> using Vector = std::vector<T>;

template <typename T> struct TestCase {
  Matrix<T> mat;
  Vector<T> vec;
};

template <typename T> class TestCaseGenerator {
public:
  TestCaseGenerator(int rows, int cols) : rows_(rows), cols_(cols) {}

  auto operator()([[maybe_unused]] int seed_) const -> TestCase<T> {
    Matrix<T> mat(rows_, std::vector<T>(cols_, 0));
    Vector<T> vec(cols_, 0);

    for (int i = 0; i < cols_; i++) {
      vec[i] = i;
    }

    for (int i = 0; i < rows_; i++) {
      for (int j = 0; j < cols_; j++) {
        mat[i][j] = i + j;
      }
    }
    return {mat, vec};
  }

private:
  int rows_, cols_;
};

template <typename T> class InnerProduct {
public:
  static auto trivial(const TestCase<T> &input) -> Vector<T> {
    size_t rows = input.mat.size();
    size_t cols = input.mat[0].size();
    Vector<T> res(rows, 0);

    if (cols != input.vec.size()) {
      throw std::invalid_argument(
          "Number of matrix columns must match vector size");
    }

    for (size_t i = 0; i < rows; i++) {
      for (size_t j = 0; j < cols; j++) {
        res[i] += input.mat[i][j] * input.vec[j];
      }
    }
    return res;
  }

  static auto cache_friendly(const TestCase<T> &input) -> Vector<T> {
    size_t rows = input.mat.size();
    size_t cols = input.mat[0].size();
    Vector<T> res(rows, 0);

    if (cols != input.vec.size()) {
      throw std::invalid_argument(
          "Number of matrix columns must match vector size");
    }

    for (size_t j = 0; j < cols; j++) {
      T v = input.vec[j];
      for (size_t i = 0; i < rows; i++) {
        res[i] += input.mat[i][j] * v;
      }
    }
    return res;
  }
};

} // namespace Lab1

int main() {
  int rows = 1000, cols = 1000;
  General::PerformanceTester<Lab1::TestCaseGenerator<double>> pt(
      Lab1::TestCaseGenerator<double>(rows, cols));
  Lab1::InnerProduct<double> ip;

  auto perf_trivial = pt.run(ip.trivial, 100, 0);
  std::cout << "Trivial Method:" << perf_trivial.str() << std::endl;
  auto perf_cache_friendly = pt.run(ip.cache_friendly, 100, 0);
  std::cout << "Cache-Friendly Method:" << perf_cache_friendly.str()
            << std::endl;
  return 0;
}
