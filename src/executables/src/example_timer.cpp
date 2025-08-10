#include <chrono>
#include <cmath>
#include <timer/frame_timer.hpp>
#include <vector>

namespace timedFunctions {
static FrameTimer frameTimer;  // NOLINT This is a single file executable

static int f1(int max) {  // NOLINT This is a single file executable
  const auto t = frameTimer.startScopedTimer("innerLoop");

  int abc = 0;
  for (int j = max; j > 0; --j) {
    abc += j;
  }
  return abc;
}

static int f2(int max) {  // NOLINT This is a single file executable
  const auto timer = frameTimer.startScopedTimer("outerLoop");

  int abc = 0;
  for (int j = max; j > 0; --j) {
    abc += f1(j);
  }
  return abc;
}

static int f3(int max) {  // NOLINT This is a single file executable
  const auto timer = frameTimer.startScopedTimer("cos");

  double abc = 0;
  for (int j = max; j > 0; --j) {
    abc += std::cos(static_cast<double>(j));
  }
  return static_cast<int>(abc);
}

static int f4(int max) {  // NOLINT This is a single file executable
  const auto timer = frameTimer.startScopedTimer("sin");

  double abc = 0;
  for (int j = max; j > 0; --j) {
    abc += std::sin(static_cast<double>(j));
  }
  return static_cast<int>(abc);
}

}  // namespace timedFunctions

int main() {
  constexpr bool debug_to_console = false;
  std::vector<int> erg;
  constexpr int NUM_ITERATIONS = 1000;
  for (int max = 1; max < NUM_ITERATIONS; ++max) {
    timedFunctions::frameTimer.frameStart<debug_to_console>();
    erg.push_back(timedFunctions::f2(max));
    erg.push_back(timedFunctions::f3(max));
    erg.push_back(timedFunctions::f4(max));
  }

  timedFunctions::frameTimer.measurementsToFile<std::chrono::microseconds>(
    "/tmp/frames.csv", ';');
  return erg[0];
}
