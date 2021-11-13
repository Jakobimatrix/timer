#include <timer/frame_timer.hpp>

FrameTimer ft;
std::string t1("innerLoop");
std::string t2("outer");
std::string t3("sqrt");

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
    a += std::sqrt(j);
  }
  return a;
}

int main() {

  std::vector<int> erg;
  for (int i = 1; i < 1000; ++i) {
    ft.frameStart<false>();
    erg.push_back(f2(i));
    erg.push_back(f3(i));
  }

  ft.measurementsToFile<std::chrono::microseconds>("/tmp/frames.csv", ';');
  return erg[0];
}
