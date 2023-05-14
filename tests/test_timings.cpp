#define BOOST_TEST_MODULE settings_test TestSuites
#define BOOST_TEST_DYN_LINK
#include <stdio.h>

#include <array>
#include <boost/test/unit_test.hpp>
#include <cmath>
#include <limits>
#include <numeric>
#include <timer/collecting_timer.hpp>
#include <timer/frame_timer.hpp>
#include <timer/precise_time.hpp>

namespace utf = boost::unit_test;
namespace tt = boost::test_tools;
using ns = std::chrono::nanoseconds;
using us = std::chrono::microseconds;
using ms = std::chrono::milliseconds;
using s = std::chrono::seconds;
using m = std::chrono::minutes;
using h = std::chrono::hours;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdisabled-macro-expansion"

void test_for_all_times(const PreciseTime &pt, const std::array<int64_t, 6> &times, int line) {
  const auto pt_times = pt.getSeperatedTimeComponents();

  const std::array<std::string, 6> info = {"Failed on nanoseconds.",
                                           "Failed on microseconds.",
                                           "Failed on milliseconds.",
                                           "Failed on seconds.",
                                           "Failed on minutes.",
                                           "Failed on hours."};

  for (size_t i = 0; i < 6; i++) {
    BOOST_TEST_INFO("LINE: " + std::to_string(line) + " " + info[i] +
                    " Array index " + std::to_string(i));
    BOOST_TEST(pt_times[i] == times[i]);
  }
}

void test_for_result(const CollectingTimer::Result timer_result,
                     const CollectingTimer::Result expected_timer_result,
                     int line) {

  BOOST_TEST_INFO("LINE: " + std::to_string(line) + " min_measurement");
  BOOST_TEST(timer_result.min_measurement == expected_timer_result.min_measurement);
  BOOST_TEST_INFO("LINE: " + std::to_string(line) + " max_measurement");
  BOOST_TEST(timer_result.max_measurement == expected_timer_result.max_measurement);
  BOOST_TEST_INFO("LINE: " + std::to_string(line) + " mean");
  BOOST_TEST(timer_result.mean == expected_timer_result.mean);
  BOOST_TEST_INFO("LINE: " + std::to_string(line) + " median");
  BOOST_TEST(timer_result.median == expected_timer_result.median);
  BOOST_TEST_INFO("LINE: " + std::to_string(line) + " standard_derivation");
  BOOST_TEST(timer_result.standard_derivation == expected_timer_result.standard_derivation);
  BOOST_TEST_INFO("LINE: " + std::to_string(line) + " number_measurements");
  BOOST_TEST(timer_result.number_measurements == expected_timer_result.number_measurements);
  BOOST_TEST_INFO("LINE: " + std::to_string(line) + " number_outliners");
  BOOST_TEST(timer_result.number_outliners == expected_timer_result.number_outliners);
  BOOST_TEST_INFO("LINE: " + std::to_string(line) + " is_outliner");
  BOOST_TEST(timer_result.is_outliner.size() ==
             expected_timer_result.is_outliner.size());
  const size_t num_outliners = std::min(
      timer_result.is_outliner.size(), expected_timer_result.is_outliner.size());
  for (size_t i = 0; i < num_outliners; i++) {
    BOOST_TEST_INFO("LINE: " + std::to_string(line) + " outliner Array index " +
                    std::to_string(i));
    BOOST_TEST(timer_result.is_outliner[i] == expected_timer_result.is_outliner[i]);
  }
}

void test_for_Histogram(const CollectingTimer::Histogram timer_histogram,
                        const CollectingTimer::Histogram expected_timer_histogram,
                        int line) {

  BOOST_TEST_INFO("LINE: " + std::to_string(line) + " bucket_size");
  BOOST_TEST(timer_histogram.bucket_size == expected_timer_histogram.bucket_size);

  BOOST_TEST_INFO("LINE: " + std::to_string(line) + " max_num_in_bucket");
  BOOST_TEST(timer_histogram.max_num_in_bucket == expected_timer_histogram.max_num_in_bucket);

  BOOST_TEST_INFO("LINE: " + std::to_string(line) + " buckets size");
  BOOST_TEST(timer_histogram.buckets.size() == expected_timer_histogram.buckets.size());

  const size_t num_buckets = std::min(timer_histogram.buckets.size(),
                                      expected_timer_histogram.buckets.size());

  for (size_t i = 0; i < num_buckets; i++) {
    BOOST_TEST_INFO("LINE: " + std::to_string(line) + " buckets Array index " +
                    std::to_string(i) + " number of entries");
    BOOST_TEST(timer_histogram.buckets[i].num ==
               expected_timer_histogram.buckets[i].num);
  }
}

BOOST_AUTO_TEST_CASE(test_precise_time_class_min_max_rollover) {

  constexpr PreciseTime max_pt = PreciseTime::max();
  constexpr PreciseTime min_pt = PreciseTime::min();

  constexpr std::array<int64_t, 6> expected_max_times = {
      999l, 999l, 999l, 59l, 59l, 9223372036854775807l};

  // https://stackoverflow.com/questions/64578042/cant-assign-64-bit-number-to-64-bit-min-in-c
  // hours_min == -9223372036854775808l);
  constexpr std::array<int64_t, 6> expected_min_times = {
      -999l, -999l, -999l, -59l, -59l, std::numeric_limits<std::int64_t>::min()};
  test_for_all_times(max_pt, expected_max_times, __LINE__);
  test_for_all_times(min_pt, expected_min_times, __LINE__);

  // the compiler actually sees that this is bad if constexpr
  // But at the moment no correct rollover for addition and substraction
  // constexpr PreciseTime pos_rollover = max_pt + ONE_NS;

  PreciseTime pos_rollover = max_pt * 2;
  test_for_all_times(pos_rollover, expected_max_times, __LINE__);

  PreciseTime neg_rollover = min_pt * 2;
  test_for_all_times(neg_rollover, expected_min_times, __LINE__);

  PreciseTime pos_rollover_2 = min_pt * -2;
  test_for_all_times(pos_rollover_2, expected_max_times, __LINE__);

  PreciseTime neg_rollover_2 = max_pt * -2;
  test_for_all_times(neg_rollover_2, expected_min_times, __LINE__);

  PreciseTime pos_rollover_3 = min_pt * min_pt;
  test_for_all_times(pos_rollover_3, expected_max_times, __LINE__);

  PreciseTime pos_rollover_4 = max_pt * max_pt;
  test_for_all_times(pos_rollover_4, expected_max_times, __LINE__);

  PreciseTime neg_rollover_3 = max_pt * min_pt;
  test_for_all_times(neg_rollover_3, expected_min_times, __LINE__);

  PreciseTime neg_rollover_4 = min_pt * max_pt;
  test_for_all_times(neg_rollover_4, expected_min_times, __LINE__);

  for (int i = 0; i < 1000; i += 15) {
    PreciseTime pos_rollover_5 = PreciseTime(ms(i)) + max_pt + ns(i);
    test_for_all_times(pos_rollover_5, expected_max_times, __LINE__);

    PreciseTime neg_rollover_5 =
        PreciseTime(s(i)) * -1. + min_pt - ns(i) - ms(i) - h(i);
    test_for_all_times(neg_rollover_5, expected_min_times, __LINE__);
  }

  PreciseTime pos_rollover_6 = max_pt + max_pt;
  test_for_all_times(pos_rollover_6, expected_max_times, __LINE__);

  PreciseTime neg_rollover_6 = min_pt + min_pt;
  test_for_all_times(neg_rollover_6, expected_min_times, __LINE__);
}

BOOST_AUTO_TEST_CASE(test_precise_time_class_construct) {

  constexpr PreciseTime pt_0 = ns(98788987654321);
  constexpr std::array<int64_t, 6> expected_times_0 = {321l, 654l, 987l, 28l, 26l, 27};
  test_for_all_times(pt_0, expected_times_0, __LINE__);

  constexpr PreciseTime pt_1 = ns(-98788987654321);
  constexpr std::array<int64_t, 6> expected_times_1 = {
      -321l, -654l, -987l, -28l, -26l, -27};
  test_for_all_times(pt_1, expected_times_1, __LINE__);

  constexpr PreciseTime pt_2 = ns(321) + us(654) + ms(987) + s(28) + m(26) + h(27);
  test_for_all_times(pt_2, expected_times_0, __LINE__);

  constexpr PreciseTime pt_3 = ns(321) - us(654) + ms(987) - s(28) + m(26) - h(27);
  constexpr std::array<int64_t, 6> expected_times_3 = {-679l, -653l, -13l, -27l, -34l, -26};
  test_for_all_times(pt_3, expected_times_3, __LINE__);
}

BOOST_AUTO_TEST_CASE(test_precise_time_class_calculus) {

  constexpr PreciseTime pt_0 = ns(8788987654321);
  constexpr auto pt_1 = pt_0 * 2;
  constexpr auto pt_2 = pt_0 + pt_0;
  BOOST_TEST(pt_1 == pt_2);

  constexpr auto pt_3 = pt_0 * 0.5;
  constexpr auto pt_4 = pt_0 / 2;
  BOOST_TEST(pt_3 == pt_4);

  constexpr auto pt_5 = pt_0 * 0.125;
  constexpr auto pt_6 = pt_0 / 8;
  BOOST_TEST(pt_5 == pt_6);

  constexpr PreciseTime pt_7 = s(2);
  BOOST_TEST(pt_7.toDouble<s>() == 2);
  const PreciseTime pt_8 = pt_7 * pt_7;
  BOOST_TEST(pt_8.toDouble<s>() == 4);
  PreciseTime pt_88 = s(4);
  pt_88.setExponent(2);
  BOOST_TEST(pt_8 == pt_88);

  BOOST_TEST(pt_8.getExponent() == 2);
  const auto pt_9 = pt_8.getSqrt();
  BOOST_TEST(pt_7 == pt_9);

  double value = 98788987654321;
  PreciseTime pt_10 = ns(static_cast<int64_t>(value));
  BOOST_TEST(pt_10.toDouble<ns>() == value);
  pt_10 -= us(44);
  value -= us2ns(44);
  BOOST_TEST(pt_10.toDouble<ns>() == value);
  pt_10 *= 1007;
  value *= 1007;
  BOOST_TEST(pt_10.toDouble<ns>() == value);
  pt_10 /= 7.;
  value /= 7.;
  BOOST_TEST(pt_10.toDouble<ns>() == value);
  pt_10 += ns(1);
  value += 1;
  BOOST_TEST(pt_10.toDouble<ns>() == value);

  // sign test ns
  PreciseTime pt_11 = s(3);
  pt_11 -= ns(333);
  constexpr PreciseTime pt_11_exp = ns(2999999667);
  BOOST_TEST(pt_11 == pt_11_exp);

  // sign test ns
  PreciseTime pt_12 = s(-3);
  pt_12 += ns(333);
  constexpr PreciseTime pt_12_exp = ns(-2999999667);
  BOOST_TEST(pt_12 == pt_12_exp);

  // sign test sec
  PreciseTime pt_13 = h(3);
  pt_13 -= s(333);
  constexpr PreciseTime pt_13_exp = h(2) + s(3600 - 333);
  BOOST_TEST(pt_13 == pt_13_exp);

  // sign test sec
  PreciseTime pt_14 = h(-3);
  pt_14 += s(333);
  constexpr PreciseTime pt_14_exp = h(-2) - s(3600 - 333);
  BOOST_TEST(pt_14 == pt_14_exp);

  constexpr PreciseTime pt_15 =
      PreciseTime(h(-3)) + PreciseTime(s(66)) - PreciseTime(ns(111111111));
  constexpr PreciseTime pt_15_exp = h(-2) - s(3600 - 66) - ns(111111111);
  BOOST_TEST(pt_15 == pt_15_exp);

  constexpr PreciseTime pt_16 =
      PreciseTime(h(3)) - PreciseTime(s(66)) + PreciseTime(ns(111111111));
  constexpr PreciseTime pt_16_exp = h(2) + s(3600 - 66) + ns(111111111);
  BOOST_TEST(pt_16 == pt_16_exp);

  constexpr PreciseTime pt_17 =
      PreciseTime(h(3)) - PreciseTime(s(66)) - PreciseTime(ns(111111111));
  constexpr PreciseTime pt_17_exp = h(2) + s(3600 - 66 - 1) + ns(888888889);
  BOOST_TEST(pt_17 == pt_17_exp);

  constexpr PreciseTime pt_18 =
      PreciseTime(-h(3)) + PreciseTime(s(66)) + PreciseTime(ns(111111111));
  constexpr PreciseTime pt_18_exp = h(-2) - s(3600 - 66 - 1) - ns(888888889);
  BOOST_TEST(pt_18 == pt_18_exp);

  constexpr PreciseTime pt_19 = PreciseTime(h(3)) - PreciseTime(ns(1));
  constexpr PreciseTime pt_19_exp = h(2) + s(3600 - 1) + ns(999999999);
  BOOST_TEST(pt_19 == pt_19_exp);

  constexpr PreciseTime pt_20 = PreciseTime(h(-3)) + PreciseTime(ns(1));
  constexpr PreciseTime pt_20_exp =
      PreciseTime(h(-2)) - PreciseTime(s(3600 - 1)) - PreciseTime(ns(999999999));
  BOOST_TEST(pt_20 == pt_20_exp);

  constexpr PreciseTime pt_21_exp =
      PreciseTime(h(1)) + PreciseTime(s(2)) + PreciseTime(ns(3));

  constexpr PreciseTime pt_22_exp =
      PreciseTime(h(1)) + PreciseTime(s(2)) + PreciseTime(ns(4));

  constexpr PreciseTime pt_23_exp =
      PreciseTime(h(1)) + PreciseTime(s(3)) + PreciseTime(ns(3));

  constexpr PreciseTime pt_24_exp =
      PreciseTime(h(2)) + PreciseTime(s(2)) + PreciseTime(ns(3));

  BOOST_TEST(pt_21_exp < pt_22_exp);
  BOOST_TEST(pt_21_exp < pt_23_exp);
  BOOST_TEST(pt_21_exp < pt_24_exp);

  BOOST_TEST(pt_22_exp > pt_21_exp);
  BOOST_TEST(pt_23_exp > pt_21_exp);
  BOOST_TEST(pt_24_exp > pt_21_exp);


  constexpr PreciseTime pt_25_exp = std::chrono::nanoseconds(5000);
  constexpr PreciseTime pt_26_exp = std::chrono::microseconds(1);

  BOOST_TEST(pt_25_exp > pt_26_exp);
  BOOST_TEST(pt_26_exp < pt_25_exp);

  constexpr PreciseTime pt_27_exp = std::chrono::nanoseconds(11);
  constexpr PreciseTime pt_28_exp = std::chrono::milliseconds(1);

  BOOST_TEST(pt_28_exp > pt_27_exp);
  BOOST_TEST(pt_27_exp < pt_28_exp);

  constexpr PreciseTime pt_29_exp = std::chrono::nanoseconds(1);
  constexpr PreciseTime pt_30_exp = std::chrono::seconds(1);

  BOOST_TEST(pt_30_exp > pt_29_exp);
  BOOST_TEST(pt_29_exp < pt_30_exp);
}

BOOST_AUTO_TEST_CASE(test_precise_time_class_test_to_string) {
  constexpr PreciseTime ns = std::chrono::nanoseconds(1);
  constexpr PreciseTime us = std::chrono::microseconds(1);
  constexpr PreciseTime ms = std::chrono::milliseconds(1);
  constexpr PreciseTime s = std::chrono::seconds(1);
  constexpr PreciseTime m = std::chrono::minutes(1);
  constexpr PreciseTime h = std::chrono::hours(1);

  BOOST_TEST(ns.getTimeString(0).c_str() == "1ns");
  BOOST_TEST(us.getTimeString(1).c_str() == "1.0us");
  BOOST_TEST(ms.getTimeString(2).c_str() == "1.00ms");
  BOOST_TEST(s.getTimeString(3).c_str() == "1.000s");
  BOOST_TEST(m.getTimeString(4).c_str() == "1.0000m");
  BOOST_TEST(h.getTimeString(5).c_str() == "1.00000h");

  BOOST_TEST(ns.getMayorTimeString() == "1ns");
  BOOST_TEST(us.getMayorTimeString() == "1us");
  BOOST_TEST(ms.getMayorTimeString() == "1ms");
  BOOST_TEST(s.getMayorTimeString() == "1s");
  BOOST_TEST(m.getMayorTimeString() == "1m");
  BOOST_TEST(h.getMayorTimeString() == "1h");

  BOOST_TEST(ns.toString() ==
             "{h: [0]   m: [0]   s: [0]   ms: [0]   us: [0]   ns: [1]}^1");
  BOOST_TEST(us.toString() ==
             "{h: [0]   m: [0]   s: [0]   ms: [0]   us: [1]   ns: [0]}^1");
  BOOST_TEST(ms.toString() ==
             "{h: [0]   m: [0]   s: [0]   ms: [1]   us: [0]   ns: [0]}^1");
  BOOST_TEST(s.toString() ==
             "{h: [0]   m: [0]   s: [1]   ms: [0]   us: [0]   ns: [0]}^1");
  BOOST_TEST(m.toString() ==
             "{h: [0]   m: [1]   s: [0]   ms: [0]   us: [0]   ns: [0]}^1");
  BOOST_TEST(h.toString() ==
             "{h: [1]   m: [0]   s: [0]   ms: [0]   us: [0]   ns: [0]}^1");

  constexpr PreciseTime usns = us + ns;
  constexpr PreciseTime msusns = ms + usns;
  constexpr PreciseTime smsusns = s + msusns;
  constexpr PreciseTime msmsusns = m + smsusns;
  constexpr PreciseTime hmsmsusns = h + msmsusns;

  BOOST_TEST(usns.getTimeString(4).c_str() == "1.0010us");
  BOOST_TEST(msusns.getTimeString(4).c_str() == "1.0010ms");
  BOOST_TEST(smsusns.getTimeString(4).c_str() == "1.0010s");
  BOOST_TEST(msmsusns.getTimeString(4).c_str() == "1.0167m");
  BOOST_TEST(hmsmsusns.getTimeString(4).c_str() == "1.0169h");

  BOOST_TEST(usns.getMayorTimeString().c_str() == "1us");
  BOOST_TEST(msusns.getMayorTimeString().c_str() == "1ms");
  BOOST_TEST(smsusns.getMayorTimeString().c_str() == "1s");
  BOOST_TEST(msmsusns.getMayorTimeString().c_str() == "1m");
  BOOST_TEST(hmsmsusns.getMayorTimeString().c_str() == "1h");

  BOOST_TEST(usns.toString() ==
             "{h: [0]   m: [0]   s: [0]   ms: [0]   us: [1]   ns: [1]}^1");
  BOOST_TEST(msusns.toString() ==
             "{h: [0]   m: [0]   s: [0]   ms: [1]   us: [1]   ns: [1]}^1");
  BOOST_TEST(smsusns.toString() ==
             "{h: [0]   m: [0]   s: [1]   ms: [1]   us: [1]   ns: [1]}^1");
  BOOST_TEST(msmsusns.toString() ==
             "{h: [0]   m: [1]   s: [1]   ms: [1]   us: [1]   ns: [1]}^1");
  BOOST_TEST(hmsmsusns.toString() ==
             "{h: [1]   m: [1]   s: [1]   ms: [1]   us: [1]   ns: [1]}^1");

  constexpr PreciseTime ss = s * s;
  constexpr PreciseTime sss = ss * s;
  constexpr PreciseTime ssss = sss * s;

  BOOST_TEST(ss.toString() ==
             "{h: [0]   m: [0]   s: [1]   ms: [0]   us: [0]   ns: [0]}^2");
  BOOST_TEST(sss.toString() ==
             "{h: [0]   m: [0]   s: [1]   ms: [0]   us: [0]   ns: [0]}^3");
  BOOST_TEST(ssss.toString() ==
             "{h: [0]   m: [0]   s: [1]   ms: [0]   us: [0]   ns: [0]}^4");
}

BOOST_AUTO_TEST_CASE(test_precise_time_class_better_than_double) {

  constexpr PreciseTime max = PreciseTime::max();
  constexpr PreciseTime max_minus_one_ns = max - ns(1);
  BOOST_TEST(max != max_minus_one_ns);

  constexpr double max_d_s = max.toDouble<s>();
  constexpr double max_d_s_minus_one_ns = max_d_s - ns2s(1);
  BOOST_TEST(max_d_s_minus_one_ns == max_minus_one_ns.toDouble<s>());
  BOOST_TEST(max_d_s == max_d_s_minus_one_ns);

  constexpr double max_d_ns = max.toDouble<ns>();
  constexpr double max_d_ns_minus_one_ns = max_d_ns - 1;
  BOOST_TEST(max_d_ns_minus_one_ns == max_minus_one_ns.toDouble<ns>());
  BOOST_TEST(max_d_ns == max_d_ns_minus_one_ns);
}

BOOST_AUTO_TEST_CASE(test_timer) {

  const std::string timer = "t";
  std::vector<PreciseTime> timer_0_times = {
      ns(5), ns(1), ns(7), ns(6), ns(8), ns(4), ns(2), ns(20)};
  CollectingTimer t_0(timer_0_times, timer);
  CollectingTimer::Result r_0;
  CollectingTimer::Result r_0_expected;
  t_0.getResult(timer, r_0);
  r_0_expected.min_measurement = ns(1);
  r_0_expected.max_measurement = ns(20);
  r_0_expected.mean = ns(6);
  r_0_expected.median = ns(5);
  r_0_expected.standard_derivation = ns(5);
  r_0_expected.number_measurements = 8;
  r_0_expected.number_outliners = 0;
  r_0_expected.is_outliner = {false, false, false, false, false, false, false, false};

  test_for_result(r_0, r_0_expected, __LINE__);

  CollectingTimer::Result r_1;
  r_1.outliner_range = 2.25;
  CollectingTimer::Result r_1_expected;
  t_0.getResult(timer, r_1);
  r_1_expected.min_measurement = ns(1);
  r_1_expected.max_measurement = ns(8);
  r_1_expected.mean = ns(4);
  r_1_expected.median = ns(5);
  r_1_expected.standard_derivation = ns(2);
  r_1_expected.number_measurements = 8;
  r_1_expected.number_outliners = 1;
  r_1_expected.is_outliner = {false, false, false, false, false, false, false, true};
  test_for_result(r_1, r_1_expected, __LINE__);

  // construct a triangle
  // put       50
  // put    49,50,51
  // put 48,49,50,51,52
  // ...
  constexpr size_t num_measurements = 50 * 50;
  std::vector<PreciseTime> timer_2_times(num_measurements);
  size_t num_input = 0;
  size_t range(1);
  ms center(50);
  int ii = 0;
  while (num_input < num_measurements) {
    std::generate(
        timer_2_times.begin() + num_input,
        timer_2_times.begin() + num_input + range,
        [n = center - (ms(range) - ms(1)) / 2]() mutable { return n++; });
    num_input += range;
    range += 2;
    ii++;
  }

  BOOST_TEST(ii = 50);

  CollectingTimer t_2(timer_2_times, timer);
  CollectingTimer::Result r_2;
  CollectingTimer::Result r_2_expected;
  t_2.getResult(timer, r_2);
  r_2_expected.min_measurement = ms(1);
  r_2_expected.max_measurement = ms(99);
  r_2_expected.mean = center;
  r_2_expected.median = center;
  r_2_expected.standard_derivation = ns(414) + us(412) + ms(20);
  r_2_expected.number_measurements = num_measurements;
  r_2_expected.number_outliners = 0;
  r_2_expected.is_outliner = std::vector<char>(num_measurements, false);

  test_for_result(r_2, r_2_expected, __LINE__);

  r_2_expected.h.bucket_size = ms(5) + us(248) + ns(958);
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

FrameTimer ft;
std::string t1("F1");
std::string t2("F2");

int f1(int i) {
  const auto t = ft.startScopedTimer(t1);
  int a = 0;
  for (int j = i; j > 0; --j) {
    a += j / i;
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

BOOST_AUTO_TEST_CASE(test_FrameTimer) {
  std::vector<int> erg;
  for (int i = 0; i < 1000; ++i) {
    ft.frameStart<true>();
    erg.push_back(f2(i));
  }

  BOOST_TEST(erg[0] == 0);
}

#pragma clang diagnostic pop
