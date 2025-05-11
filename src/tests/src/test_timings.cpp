#include <catch2/catch_test_macros.hpp>

#include <cstdio>
#include <timer/precise_time.hpp>
#include <sstream>

#include <array>
#include <cmath>
#include <limits>
#include <numeric>
#include <timer/collecting_timer.hpp>
#include <timer/frame_timer.hpp>
#include <timer/precise_time.hpp>

using ns = std::chrono::nanoseconds;
using us = std::chrono::microseconds;
using ms = std::chrono::milliseconds;
using s  = std::chrono::seconds;
using m  = std::chrono::minutes;
using h  = std::chrono::hours;


void test_for_result(const CollectingTimer::Result timer_result,
                     const CollectingTimer::Result expected_timer_result,
                     int line) {

  INFO("LINE: " + std::to_string(line) + " min_measurement");
  REQUIRE(timer_result.min_measurement == expected_timer_result.min_measurement);
  INFO("LINE: " + std::to_string(line) + " max_measurement");
  REQUIRE(timer_result.max_measurement == expected_timer_result.max_measurement);
  INFO("LINE: " + std::to_string(line) + " mean");
  REQUIRE(timer_result.mean == expected_timer_result.mean);
  INFO("LINE: " + std::to_string(line) + " median");
  REQUIRE(timer_result.median == expected_timer_result.median);
  INFO("LINE: " + std::to_string(line) + " standard_derivation");
  REQUIRE(timer_result.standard_derivation == expected_timer_result.standard_derivation);
  INFO("LINE: " + std::to_string(line) + " number_measurements");
  REQUIRE(timer_result.number_measurements == expected_timer_result.number_measurements);
  INFO("LINE: " + std::to_string(line) + " number_outliners");
  REQUIRE(timer_result.number_outliners == expected_timer_result.number_outliners);
  INFO("LINE: " + std::to_string(line) + " is_outliner");
  REQUIRE(timer_result.is_outliner.size() == expected_timer_result.is_outliner.size());
  const size_t num_outliners = std::min(
    timer_result.is_outliner.size(), expected_timer_result.is_outliner.size());
  for (size_t i = 0; i < num_outliners; i++) {
    INFO("LINE: " + std::to_string(line) + " outliner Array index " + std::to_string(i));
    REQUIRE(timer_result.is_outliner[i] == expected_timer_result.is_outliner[i]);
  }
}

void test_for_Histogram(const CollectingTimer::Histogram timer_histogram,
                        const CollectingTimer::Histogram expected_timer_histogram,
                        int line) {

  INFO("LINE: " + std::to_string(line) + " bucket_size");
  REQUIRE(timer_histogram.bucket_size == expected_timer_histogram.bucket_size);

  INFO("LINE: " + std::to_string(line) + " max_num_in_bucket");
  REQUIRE(timer_histogram.max_num_in_bucket == expected_timer_histogram.max_num_in_bucket);

  INFO("LINE: " + std::to_string(line) + " buckets size");
  REQUIRE(timer_histogram.buckets.size() == expected_timer_histogram.buckets.size());

  const size_t num_buckets = std::min(timer_histogram.buckets.size(),
                                      expected_timer_histogram.buckets.size());

  for (size_t i = 0; i < num_buckets; i++) {
    INFO("LINE: " + std::to_string(line) + " buckets Array index " +
         std::to_string(i) + " number of entries");
    REQUIRE(timer_histogram.buckets[i].num ==
            expected_timer_histogram.buckets[i].num);
  }
}

TEST_CASE("test_timer") {
  const std::string timer                = "t";
  std::vector<PreciseTime> timer_0_times = {
    ns(5), ns(1), ns(7), ns(6), ns(8), ns(4), ns(2), ns(20)};
  CollectingTimer t_0(timer_0_times, timer);
  CollectingTimer::Result r_0;
  CollectingTimer::Result r_0_expected;
  t_0.getResult(timer, r_0);
  r_0_expected.min_measurement     = ns(1);
  r_0_expected.max_measurement     = ns(20);
  r_0_expected.mean                = ns(6);
  r_0_expected.median              = ns(5);
  r_0_expected.standard_derivation = ns(5);
  r_0_expected.number_measurements = 8;
  r_0_expected.number_outliners    = 0;
  r_0_expected.is_outliner = {false, false, false, false, false, false, false, false};

  test_for_result(r_0, r_0_expected, __LINE__);

  CollectingTimer::Result r_1;
  r_1.outliner_range = 2.25;
  CollectingTimer::Result r_1_expected;
  t_0.getResult(timer, r_1);
  r_1_expected.min_measurement     = ns(1);
  r_1_expected.max_measurement     = ns(8);
  r_1_expected.mean                = ns(4);
  r_1_expected.median              = ns(5);
  r_1_expected.standard_derivation = ns(2);
  r_1_expected.number_measurements = 8;
  r_1_expected.number_outliners    = 1;
  r_1_expected.is_outliner = {false, false, false, false, false, false, false, true};
  test_for_result(r_1, r_1_expected, __LINE__);

  // construct a triangle
  // put       50
  // put    49,50,51
  // put 48,49,50,51,52
  // ...
  constexpr size_t num_measurements = 50 * 50;
  std::vector<PreciseTime> timer_2_times(num_measurements);
  long int num_input = 0;
  long int range(1);
  ms center(50);
  int ii = 0;
  while (num_input < static_cast<long int>(num_measurements)) {
    std::generate(
      timer_2_times.begin() + num_input,
      timer_2_times.begin() + num_input + range,
      [n = center - (ms(range) - ms(1)) / 2]() mutable { return n++; });
    num_input += range;
    range     += 2;
    ii++;
  }

  REQUIRE(ii == 50);

  CollectingTimer t_2(timer_2_times, timer);
  CollectingTimer::Result r_2;
  CollectingTimer::Result r_2_expected;
  t_2.getResult(timer, r_2);
  r_2_expected.min_measurement     = ms(1);
  r_2_expected.max_measurement     = ms(99);
  r_2_expected.mean                = center;
  r_2_expected.median              = center;
  r_2_expected.standard_derivation = ns(414) + us(412) + ms(20);
  r_2_expected.number_measurements = num_measurements;
  r_2_expected.number_outliners    = 0;
  r_2_expected.is_outliner         = std::vector<char>(num_measurements, false);

  test_for_result(r_2, r_2_expected, __LINE__);

  r_2_expected.h.bucket_size       = ms(5) + us(248) + ns(958);
  r_2_expected.h.max_num_in_bucket = 273;
  r_2_expected.h.buckets.resize(19);

  // The triangle is in 1ms steps, the bucket size is a little more than 5ms
  // The buckets contain the following rows of the triangle:
  // Bucket 1: [1-6] that is the most left part of the triangle:
  //      6
  //     56
  //    456
  //   3456 num = n(n+1)/2 = 21 (n=6)
  //  23456
  // 123456
  // Bucket 2: [7-11] num = 11*12/2 - 21 = 45
  // Bucket 3: [12-16] num = 15*16/2 - 45 - 21 = 70
  // Bucket 4: [17-21] num = 21*22/2 - 70 - 45 - 21 = 95
  // Bucket 5: [22-27] num = 147
  // Bucket 6: [28-32] num = 150
  // Bucket 7: [33-37] num = 175
  // ...

  constexpr std::array<int, 19> num_in_bucket = {
    21, 45, 70, 95, 147, 150, 175, 200, 273, 243, 220, 195, 201, 140, 115, 90, 75, 35, 10};

  for (size_t i = 0; i < 19; i++) {
    r_2_expected.h.buckets[i].num = num_in_bucket[i];
  }

  test_for_Histogram(r_2.h, r_2_expected.h, __LINE__);
}

TEST_CASE("test_FrameTimer") {
  FrameTimer ft;
  std::string t1("F1");
  std::string t2("F2");

  auto f1 = [&ft, &t1](int i) {
    const auto t = ft.startScopedTimer(t1);
    int a        = 0;
    for (int j = i; j > 0; --j) {
      a += j / i;
    }
    return a;
  };

  auto f2 = [&ft, &t2, &f1](int i) {
    const auto t = ft.startScopedTimer(t2);
    int a        = 0;
    for (int j = i; j > 0; --j) {
      a += f1(j);
    }
    return a;
  };

  std::vector<int> erg;
  for (int i = 0; i < 1000; ++i) {
    ft.frameStart<true>();
    erg.push_back(f2(i));
  }

  REQUIRE(erg[0] == 0);
}
