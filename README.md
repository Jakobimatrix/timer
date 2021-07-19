# timer
A simple timer class easy to use. useing std::chrono

Please use clang-tidy if you want to contribute: [easy installation](https://github.com/Jakobimatrix/initRepro)

### Features  PreciseTime class:
 * adding/substracting with considering the unit: 1[s] + 1[s] -> ok; 1[s] + 1[s^2] -> error
 * division/multiplication with
 * a) int/double/etc
 * b) PreciseTime class while keeping track of the unit: 1[s] * 1[s] = 1[s^2]
 * calculateing the square root of a PreciseTime keeping track of the unit: sqrt(4[s^2]) = 2[s].
 * Comparisons: {==, !=, <=, >=, <, >}
 * Nice print output: std::cout << my_precise_time << std::endl;
 * Max Time: h: [9223372036854775807] m: [59]  s: [59]  ms: [999] us: [999] ns: [999]
 * Min Time: h: [-9223372036854775808] m: [-59]  s: [-59]  ms: [-999] us: [-999] ns:[-999]
 * Constexpr construction and arithmetic (appart from sqrt)

### Features Timer class:
 * Timer: Useing the PreciseTime class to track time.
 * No need to register new timers.
 * As many timers as you like.
 * On demand output of max, min, mean, standard deviation for all timers.
 * Print Histogram fo measurements into console
 * Write multiple histograms on top of each other for better comparison.
 * Write measurements to file for further investigation in favourite table calculation or MATLAB
 * Print histogram to file for further investigation in your favourite table calculation (choose X-Y-Plot) or MATLAB.
 
### Features SimpleTimer class:
 * Start/Reset/getTimeTime without saving measurements. No overheat.
 
## Todos
 - [x] Better conversations between ns, ms ,s
 - [x] add commentary
 - [x] add an example usage
 - [x] have a nice way to print measurements to file
 - [x] compute Frequency Distribution Table
 - [ ] Tests
 - [ ] A dynamic PreciseTime where the user can specify the needed resolution and the max time span to optimise calculation
 
## Build the project (Linux)
* If you use CMake to build your project place this repro inside 'path' and use **add_subdirectory(path/
timer)** and **target_link_libraries(your_lib timer_lib)**
* Or just include the header

## Example usage:


```c++
#include "timer.hpp"
#include <string>
#include <cmath>
#include <random>
#include <unistd.h>

// from https://github.com/google/benchmark/blob/main/include/benchmark/benchmark.h
#define BENCHMARK_ALWAYS_INLINE __attribute__((always_inline))

template <class Tp>
inline BENCHMARK_ALWAYS_INLINE void DoNotOptimize(Tp const& value) {
  asm volatile("" : : "r,m"(value) : "memory");
}

template <class Tp>
inline BENCHMARK_ALWAYS_INLINE void DoNotOptimize(Tp& value) {
#if defined(__clang__)
  asm volatile("" : "+r,m"(value) : : "memory");
#else
  asm volatile("" : "+m,r"(value) : : "memory");
#endif
}

constexpr double fastModF(double x, double& full) noexcept {
    full = static_cast<double>(static_cast<long long>(x));
    return x - full;
}


int main() {
  constexpr tool::PreciseTime max_pt = tool::PreciseTime::max();
  constexpr tool::PreciseTime min_pt = tool::PreciseTime::min();
  std::cout << "max: " << max_pt << "\n"
            << "min: " << min_pt << "\n";

  // construction
  tool::PreciseTime my_time1(std::chrono::nanoseconds(987654321));
  std::cout << "1. " << my_time1 << "\n";
  tool::PreciseTime my_time2(std::chrono::milliseconds(42));
  std::cout << "2. " << my_time2 << "\n";
  constexpr tool::PreciseTime my_time3 =
      tool::PreciseTime(std::chrono::nanoseconds(22)) +
      tool::PreciseTime(std::chrono::microseconds(450)) +
      tool::PreciseTime(std::chrono::milliseconds(12));
  std::cout << "3. " << my_time3 << "\n";

  // calculations: add/substract times
  my_time1 = my_time1 + my_time1;
  my_time1 -= my_time2;

  // calculations: multiply/divide by factor
  my_time2 *= 1.5;
  my_time2 = my_time2 / 3.3;

  // calculations: s*s = s^2
  tool::PreciseTime timeSquared = my_time1 * my_time2;
  // This throws a runtime error if build in debug mode.
  // calculations: s^2 + s: can't mix units
  // tool::PreciseTime corruptedTime = timeSquared + my_time1;

  // Takeing the square root only works with usints s^n where n is even.
  tool::PreciseTime normalTime = tool::PreciseTime::sqrt(timeSquared);
  std::cout << "4. " << normalTime << "\n";

  // overflow protection
  constexpr auto my_time4 = max_pt * 5;  // my_time4 is still max_pt
  std::cout << "5. " << my_time4 << "\n";

  // constexpr arithmetic
  constexpr auto zero_t = max_pt - my_time4;
  std::cout << "6. " << zero_t << "\n";

  // coversations
  typedef std::chrono::microseconds exampleType;

  // This gets the time in exampleType ! this might result in resolution loss
  // if the time is 22ns and 450us and 12ms, This returns exact 12450us
  constexpr exampleType us = my_time3.convert<exampleType>();
  std::cout << "7. " << us.count() << "\n";

  // if the time is 22ns and 450us and 12ms, This returns exact 450us
  constexpr exampleType part_us = my_time3.get<exampleType>();
  std::cout << "8. " << part_us.count() << "\n";

  // if the time is 22ns and 450us and 12ms, This returns exact* 12450.022
  // double precision exact
  constexpr double floatingPoint = my_time3.toDouble<exampleType>();
  std::cout << "9. " << std::fixed << floatingPoint << "\n\n";

  // Usage of timer class
  tool::Timer timer;

  std::random_device rd;
  std::default_random_engine generator(rd());
  const double mean = 1.;
  const double deviation = 0.15;
  std::normal_distribution<double> distribution(mean, deviation);
  
  const std::string timer_cos("std::cos");
  const std::string timer_sleep("sleep"); 

  for (int i = 0; i < 100; i++) {
    const double sleep = distribution(generator);
    timer.start(timer_sleep);
    usleep(sleep * 100);
    timer.stop(timer_sleep);
    timer.start(timer_cos);
    std::cos(static_cast<double>(i) / 100.);
    timer.stop(timer_cos);
  }
  std::cout << timer << "\n";
  
  constexpr int num_tests = 1000000;
  
  std::uniform_real_distribution<double> distr(-1000000000000000000., 100000000000000000000.);

  const std::string timer_std_modf("std::modf");
  const std::string timer_my_mod_f("my::modf");

  for(int i = 0; i < num_tests; i++){
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
  
  timer.histogramToFile<std::chrono::nanoseconds>("/tmp/histo.csv", ';');
  timer.measurementsToFile<std::chrono::nanoseconds>("/tmp/measure.csv", ';');
  
  // print to console
  std::vector<tool::Timer::Result>results(2);
  timer.getResult(timer_std_modf, results[0]);
  timer.getResult(timer_my_mod_f, results[1]);
  
  std::cout << results;
  
  return 0;
}
```

### Output:
![PreciseTime sample output](https://raw.githubusercontent.com/Jakobimatrix/timer/master/example_output/precise_time_sample_output.png)

![timer example 1](https://raw.githubusercontent.com/Jakobimatrix/timer/master/example_output/timer_example_1.png)

![timer example 2](https://raw.githubusercontent.com/Jakobimatrix/timer/master/example_output/timer_example_2.png)
