#include <timer/frame_timer.hpp>

FrameTimer ft;
std::string t1("innerLoop");
std::string t2("outerLoop");
std::string t3("cos");
std::string t4("sin");

int f1(int max) {
  const auto t = ft.startScopedTimer(t1);
  int a        = 0;
  for (int j = max; j > 0; --j) {
    a += j;
  }
  return a;
}

int f2(int max) {
  const auto t = ft.startScopedTimer(t2);
  int a        = 0;
  for (int j = max; j > 0; --j) {
    a += f1(j);
  }
  return a;
}

int f3(int max) {
  const auto t = ft.startScopedTimer(t3);
  double a     = 0;
  for (int j = max; j > 0; --j) {
    a += std::cos(static_cast<double>(j));
  }
  return static_cast<int>(a);
}

int f4(int max) {
  const auto t = ft.startScopedTimer(t4);
  double a     = 0;
  for (int j = max; j > 0; --j) {
    a += std::sin(static_cast<double>(j));
  }
  return static_cast<int>(a);
}

int main() {
  constexpr bool debug_to_console = false;
  std::vector<int> erg;
  for (int max = 1; max < 1000; ++max) {
    ft.frameStart<debug_to_console>();
    erg.push_back(f2(max));
    erg.push_back(f3(max));
    erg.push_back(f4(max));
  }

  ft.measurementsToFile<std::chrono::microseconds>("/tmp/frames.csv", ';');
  return erg[0];
}
