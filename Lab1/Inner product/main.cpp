#include "../../general/Tester.h"
#include "forLoopUnroll.h"
#include <cmath>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>
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

  std::string output_dir = "results/inner_product";
  std::system("mkdir -p results/inner_product");

  auto perf_trivial = pt.runAndSave(ip.trivial, "Trivial", output_dir, 1000, 0);
  std::cout << "Trivial Algorithm: " << perf_trivial.str() << std::endl;

  auto perf_cache_friendly =
      pt.runAndSave(ip.cache_friendly, "CacheFriendly", output_dir, 1000, 0);
  std::cout << "Cache-Friendly Algorithm: " << perf_cache_friendly.str()
            << std::endl;

  auto perf_unrolled =
      pt.runAndSave(ip.unrolled_loop, "UnrolledLoop", output_dir, 1000, 0);
  std::cout << "Unrolled Loop Algorithm: " << perf_unrolled.str() << std::endl;

  auto perf_eliminated_loop =
      pt.runAndSave(ip.eliminated_loop, "EliminatedLoop", output_dir, 1000, 0);
  std::cout << "Eliminated Loop Algorithm: " << perf_eliminated_loop.str()
            << std::endl;

  std::cout << "Detailed test data saved to " << output_dir << " directory"
            << std::endl;

  return 0;
}
