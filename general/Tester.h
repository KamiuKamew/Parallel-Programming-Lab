#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace General {
struct Performance {
  double duration_mean = 0.0; // microseconds
  double duration_stddev = 0.0;
  double duration_variance = 0.0;
  std::size_t repeat = 0;
  std::vector<long> raw_durations; // 存储每次测试的原始数据

  void addNewResult(long duration_) {
    // 存储原始数据
    raw_durations.push_back(duration_);

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

  // 将性能数据保存到CSV文件中
  void saveToCSV(const std::string &filename,
                 const std::string &algorithm_name) const {
    std::ofstream file(filename, std::ios::app);
    if (!file.is_open()) {
      std::cerr << "无法打开文件: " << filename << std::endl;
      return;
    }

    // 如果文件为空，写入CSV头
    file.seekp(0, std::ios::end);
    if (file.tellp() == 0) {
      file << "Algorithm,TestNumber,Duration(us)" << std::endl;
    }

    // 写入每次测试的数据
    for (size_t i = 0; i < raw_durations.size(); ++i) {
      file << algorithm_name << "," << i + 1 << "," << raw_durations[i]
           << std::endl;
    }

    file.close();
  }

  // 生成统计摘要
  void generateStatistics(const std::string &filename,
                          const std::string &algorithm_name) const {
    std::ofstream file(filename, std::ios::app);
    if (!file.is_open()) {
      std::cerr << "无法打开文件: " << filename << std::endl;
      return;
    }

    // 计算其他统计量
    long min_duration =
        *std::min_element(raw_durations.begin(), raw_durations.end());
    long max_duration =
        *std::max_element(raw_durations.begin(), raw_durations.end());

    // 计算中位数
    std::vector<long> sorted_durations = raw_durations;
    std::sort(sorted_durations.begin(), sorted_durations.end());
    double median;
    if (sorted_durations.size() % 2 == 0) {
      median = (sorted_durations[sorted_durations.size() / 2 - 1] +
                sorted_durations[sorted_durations.size() / 2]) /
               2.0;
    } else {
      median = sorted_durations[sorted_durations.size() / 2];
    }

    // 如果文件为空，写入CSV头
    file.seekp(0, std::ios::end);
    if (file.tellp() == 0) {
      file << "Algorithm,Mean(us),Median(us),StdDev(us),Min(us),Max(us),Samples"
           << std::endl;
    }

    // 写入统计摘要
    file << algorithm_name << "," << std::fixed << std::setprecision(2)
         << duration_mean << "," << median << "," << duration_stddev << ","
         << min_duration << "," << max_duration << "," << repeat << std::endl;

    file.close();
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

  // 增强版run方法，支持保存结果到CSV
  template <typename TestObject>
  auto runAndSave(TestObject &&testObj, const std::string &algorithm_name,
                  const std::string &output_dir, int repeat = 1, int seed_ = 0)
      -> Performance {
    Performance perf = run(std::forward<TestObject>(testObj), repeat, seed_);

    // 确保目录存在
    std::string raw_data_file = output_dir + "/raw_data.csv";
    std::string stats_file = output_dir + "/statistics.csv";

    // 保存原始数据和统计摘要
    perf.saveToCSV(raw_data_file, algorithm_name);
    perf.generateStatistics(stats_file, algorithm_name);

    return perf;
  }

private:
  TestCaseGenerator tcg;
};

} // namespace General