#include <utility>

template <int N> struct ForLoopUnroll {
  template <typename Func, typename... Args>
  static void execute(Func &&func, Args &&...args) {
    ForLoopUnroll<N - 1>::execute(std::forward<Func>(func),
                                  std::forward<Args>(args)...);
    func(N - 1, std::forward<Args>(args)...);
  }
};

template <> struct ForLoopUnroll<0> {
  template <typename Func, typename... Args>
  static void execute(Func &&, Args &&...) {}
};

/**
 * @brief unrolled for loop macro.
 *
 * @param N the number of iterations.
 * @param FUNC the function to be executed in each iteration. The first argument
 * of the function is the iteration index, and the rest of the arguments are
 * passed as is.
 * @param... the arguments to be passed to the function.
 */
#define FOR(N, FUNC, ...)                                                      \
  ForLoopUnroll<N>::execute(FUNC __VA_OPT__(, ) __VA_ARGS__)
