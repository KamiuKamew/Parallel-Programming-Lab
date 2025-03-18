#include "../../general/Tester.h"
#include "forLoopUnroll.h"
#include <cmath>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace Lab1 {

const int ROWS = 100, COLS = 100;

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
    Matrix<T> mat(rows_, std::vector<T>(cols_, 0)); // Mat[i] represents row i
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

  static auto unrolled_loop(const TestCase<T> &input) -> Vector<T> {
    size_t rows = input.mat.size();
    size_t cols = input.mat[0].size();
    Vector<T> res(rows, 0);

    if (cols != input.vec.size()) {
      throw std::invalid_argument(
          "Number of matrix columns must match vector size");
    }

    for (size_t i = 0; i < rows; i++) {
      for (size_t j = 0; j < cols; j += 4) {
        if (j + 3 < cols) {
          res[i] += input.mat[i][j] * input.vec[j] +
                    input.mat[i][j + 1] * input.vec[j + 1] +
                    input.mat[i][j + 2] * input.vec[j + 2] +
                    input.mat[i][j + 3] * input.vec[j + 3];
        } else {
          for (size_t k = j; k < cols; ++k) {
            res[i] += input.mat[i][k] * input.vec[k];
          }
        }
      }
    }
    return res;
  }

  static auto eliminated_loop(const TestCase<T> &input) -> Vector<T> {
    size_t rows = input.mat.size();
    size_t cols = input.mat[0].size();
    Vector<T> res(rows, 0);

    if (cols != input.vec.size()) {
      throw std::invalid_argument(
          "Number of matrix columns must match vector size");
    }

    FOR(ROWS, [&](int i) {
      FOR(COLS, [&](int j) { res[i] += input.mat[i][j] * input.vec[j]; });
    });
    return res;
  }
};

} // namespace Lab1

int main() {
  int rows = Lab1::ROWS, cols = Lab1::COLS;
  General::PerformanceTester<Lab1::TestCaseGenerator<double>> pt(
      Lab1::TestCaseGenerator<double>(rows, cols));
  Lab1::InnerProduct<double> ip;

  auto perf_trivial = pt.run(ip.trivial, 1000, 0);
  std::cout << "Trivial Method:" << perf_trivial.str() << std::endl;
  auto perf_cache_friendly = pt.run(ip.cache_friendly, 1000, 0);
  std::cout << "Cache-Friendly Method:" << perf_cache_friendly.str()
            << std::endl;
  auto perf_unrolled = pt.run(ip.unrolled_loop, 1000, 0);
  std::cout << "Unrolled Method:" << perf_unrolled.str() << std::endl;
  auto perf_eliminated_loop = pt.run(ip.eliminated_loop, 1000, 0);
  std::cout << "Eliminated Loop Method:" << perf_eliminated_loop.str()
            << std::endl;
  return 0;
}
