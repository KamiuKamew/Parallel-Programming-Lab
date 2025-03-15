#include <chrono>
#include <cmath>
#include <string>
#include <utility>

namespace General {
struct Performance {
  double duration_mean = 0.0; // microseconds
  double duration_stddev = 0.0;
  double duration_variance = 0.0;
  std::size_t repeat = 0;

  void addNewResult(long duration_) {
    double duration = static_cast<double>(duration_);
    if (repeat == 0) {
      duration_mean = duration;
      duration_variance = 0.0;
    } else {
      double delta = duration - duration_mean;
      duration_mean += delta / (repeat + 1);
      duration_variance += delta * (duration - duration_mean);
    }
    duration_stddev =
        repeat > 1 ? std::sqrt(duration_variance / (repeat - 1)) : 0.0;
    repeat++;
  }

  auto str() const {
    return "Performance: mean=" + std::to_string(duration_mean) +
           "us, stddev=" + std::to_string(duration_stddev) +
           "us, repeat=" + std::to_string(repeat);
  }
};

template <typename TestCaseGenerator> class PerformanceTester {
public:
  PerformanceTester<TestCaseGenerator>(TestCaseGenerator &&tcg_)
      : tcg(std::move(tcg_)) {}

  template <typename TestObject>
  auto run(TestObject &&testObj, int repeat = 1, int seed_ = 0) -> Performance {
    Performance perf;
    for (int i = 0; i < repeat; i++) {
      auto input = tcg(seed_);
      auto start = std::chrono::high_resolution_clock::now();
      auto res = testObj(input);
      auto end = std::chrono::high_resolution_clock::now();
      auto duration =
          std::chrono::duration_cast<std::chrono::microseconds>(end - start)
              .count();
      perf.addNewResult(duration);
    }
    return perf;
  }

private:
  TestCaseGenerator tcg;
};

} // namespace General