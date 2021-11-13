
#include <timer/frame_timer.hpp>

FrameTimer ft;
std::string t1("innerLoop");
std::string t2("outerLoop");
std::string t3("cos");
std::string t4("sin");

int f1(int i) {
  const auto t = ft.startScopedTimer(t1);
  int a = 0;
  for (int j = i; j > 0; --j) {
    a += j;
  }
  return a;
}

int f2(int i) {
  const auto t = ft.startScopedTimer(t2);
  int a = 0;
  for (int j = i; j > 0; --j) {
    a += f1(j);
  }
  return a;
}

int f3(int i) {
  const auto t = ft.startScopedTimer(t3);
  int a = 0;
  for (int j = i; j > 0; --j) {
    a += std::cos(j);
  }
  return a;
}

int f4(int i) {
  const auto t = ft.startScopedTimer(t4);
  int a = 0;
  for (int j = i; j > 0; --j) {
    a += std::sin(j);
  }
  return a;
}

int main() {

  std::vector<int> erg;
  for (int i = 1; i < 1000; ++i) {
    ft.frameStart<false>();
    erg.push_back(f2(i));
    erg.push_back(f3(i));
    erg.push_back(f4(i));
  }

  ft.measurementsToFile<std::chrono::microseconds>("/tmp/frames.csv", ';');
  return erg[0];
}

/*
#include <cmath>
#include <random>
#include <string>
#include <timer/collecting_timer.hpp>
#include <unistd.h>

// from
// https://github.com/google/benchmark/blob/main/include/benchmark/benchmark.h
#define BENCHMARK_ALWAYS_INLINE __attribute__((always_inline))

template <class Tp>
inline BENCHMARK_ALWAYS_INLINE void DoNotOptimize(Tp const &value) {
  asm volatile("" : : "r,m"(value) : "memory");
}

template <class Tp>
inline BENCHMARK_ALWAYS_INLINE void DoNotOptimize(Tp &value) {
#if defined(__clang__)
  asm volatile("" : "+r,m"(value) : : "memory");
#else
  asm volatile("" : "+m,r"(value) : : "memory");
#endif
}

int main() {
  CollectingTimer timer;

  std::random_device rd;
  std::default_random_engine generator(rd());

  std::uniform_real_distribution<double> distr(-1000000000000000000.,
                                               100000000000000000000.);

  constexpr int num_tests = 1000000;
  const std::string timer_std_modf("std::modf");
  const std::string timer_my_mod_f("my::modf");

  for (int i = 0; i < num_tests; i++) {
    const double d = distr(rd);
    timer.start(timer_std_modf);
    double full = 0.;
    const double decimals = std::modf(d, &full);
    timer.stop(timer_std_modf);
    DoNotOptimize(full);
    DoNotOptimize(decimals);

    timer.start(timer_my_mod_f);
    double full_mine = 0.;
    const double decimals_mine = fastModF(d, full_mine);
    timer.stop(timer_my_mod_f);
    DoNotOptimize(full_mine);
    DoNotOptimize(decimals_mine);
  }

  // save in file
  timer.histogramToFile<std::chrono::nanoseconds>("/tmp/histo.csv", ';');
  timer.measurementsToFile<std::chrono::nanoseconds>("/tmp/measure.csv", ';');

  // print to console
  std::vector<CollectingTimer::Result> results(2);
  timer.getResult(timer_std_modf, results[0]);
  timer.getResult(timer_my_mod_f, results[1]);

  std::cout << results;

  return 0;
}

*/
