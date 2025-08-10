#include <catch2/catch_test_macros.hpp>

#include <timer/precise_time.hpp>

#include <array>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <limits>

using ns = std::chrono::nanoseconds;
using us = std::chrono::microseconds;
using ms = std::chrono::milliseconds;
using s  = std::chrono::seconds;
using m  = std::chrono::minutes;
using h  = std::chrono::hours;


static constexpr size_t NUM_TESTS = 6;
static void test_for_all_times(const PreciseTime& pt,
                               const std::array<int64_t, NUM_TESTS>& times) {
  const auto pt_times = pt.getSeperatedTimeComponents();

  for (size_t i = 0; i < times.size(); i++) {
    REQUIRE(pt_times[i] == times[i]);  // NOLINT cppcoreguidelines-pro-bounds-constant-array-index // trust in the test
  }
}

TEST_CASE("test_precise_time_class_min_max_rollover") {

  constexpr PreciseTime max_pt = PreciseTime::max();
  constexpr PreciseTime min_pt = PreciseTime::min();

  constexpr std::array<int64_t, NUM_TESTS> expected_max_times = {
    999L, 999L, 999L, 59L, 59L, 9223372036854775807L};

  // https://stackoverflow.com/questions/64578042/cant-assign-64-bit-number-to-64-bit-min-in-c
  // hours_min == -9223372036854775808l);
  constexpr std::array<int64_t, NUM_TESTS> expected_min_times = {
    -999L, -999L, -999L, -59L, -59L, std::numeric_limits<std::int64_t>::min()};
  test_for_all_times(max_pt, expected_max_times);
  test_for_all_times(min_pt, expected_min_times);

  // the compiler actually sees that this is bad if constexpr
  // But at the moment no correct rollover for addition and substraction
  // constexpr PreciseTime pos_rollover = max_pt + ONE_NS;

  PreciseTime const pos_rollover = max_pt * 2;
  test_for_all_times(pos_rollover, expected_max_times);

  PreciseTime const neg_rollover = min_pt * 2;
  test_for_all_times(neg_rollover, expected_min_times);

  PreciseTime const pos_rollover_2 = min_pt * -2;
  test_for_all_times(pos_rollover_2, expected_max_times);

  PreciseTime const neg_rollover_2 = max_pt * -2;
  test_for_all_times(neg_rollover_2, expected_min_times);

  PreciseTime const pos_rollover_3 = min_pt * min_pt;
  test_for_all_times(pos_rollover_3, expected_max_times);

  PreciseTime const pos_rollover_4 = max_pt * max_pt;
  test_for_all_times(pos_rollover_4, expected_max_times);

  PreciseTime const neg_rollover_3 = max_pt * min_pt;
  test_for_all_times(neg_rollover_3, expected_min_times);

  PreciseTime const neg_rollover_4 = min_pt * max_pt;
  test_for_all_times(neg_rollover_4, expected_min_times);

  constexpr int NUM_RUNS  = 1000;
  constexpr int INCREMENT = 15;
  for (int i = 0; i < NUM_RUNS; i += INCREMENT) {
    PreciseTime const pos_rollover_5 = PreciseTime(ms(i)) + max_pt + ns(i);
    test_for_all_times(pos_rollover_5, expected_max_times);

    PreciseTime const neg_rollover_5 =
      PreciseTime(s(i)) * -1. + min_pt - ns(i) - ms(i) - h(i);
    test_for_all_times(neg_rollover_5, expected_min_times);
  }

  PreciseTime const pos_rollover_6 = max_pt + max_pt;
  test_for_all_times(pos_rollover_6, expected_max_times);

  PreciseTime const neg_rollover_6 = min_pt + min_pt;
  test_for_all_times(neg_rollover_6, expected_min_times);
}

TEST_CASE("test_precise_time_class_construct") {

  constexpr PreciseTime pt_0 = ns(98788987654321);
  constexpr std::array<int64_t, NUM_TESTS> expected_times_0 = {
    321L, 654L, 987L, 28L, 26L, 27};
  test_for_all_times(pt_0, expected_times_0);

  constexpr PreciseTime pt_1 = ns(-98788987654321);
  constexpr std::array<int64_t, NUM_TESTS> expected_times_1 = {
    -321L, -654L, -987L, -28L, -26L, -27};
  test_for_all_times(pt_1, expected_times_1);

  constexpr PreciseTime pt_2 = ns(321) + us(654) + ms(987) + s(28) + m(26) + h(27);
  test_for_all_times(pt_2, expected_times_0);

  constexpr PreciseTime pt_3 = ns(321) - us(654) + ms(987) - s(28) + m(26) - h(27);
  constexpr std::array<int64_t, NUM_TESTS> expected_times_3 = {
    -679L, -653L, -13L, -27L, -34L, -26};
  test_for_all_times(pt_3, expected_times_3);
}

TEST_CASE("test_precise_time_class_calculus") {  // NOLINT readability-function-cognitive-complexity // Happens inn big tests

  constexpr PreciseTime pt_0 = ns(8788987654321);
  constexpr auto pt_1        = pt_0 * 2;
  constexpr auto pt_2        = pt_0 + pt_0;
  REQUIRE(pt_1 == pt_2);

  constexpr auto pt_3 = pt_0 * 0.5;
  constexpr auto pt_4 = pt_0 / 2;
  REQUIRE(pt_3 == pt_4);

  constexpr auto pt_5 = pt_0 * 0.125;
  constexpr auto pt_6 = pt_0 / 8;
  REQUIRE(pt_5 == pt_6);

  constexpr PreciseTime pt_7 = s(2);
  REQUIRE(pt_7.toDouble<s>() == 2);
  const PreciseTime pt_8 = pt_7 * pt_7;
  REQUIRE(pt_8.toDouble<s>() == 4);
  PreciseTime pt_88 = s(4);
  pt_88.setExponent(2);
  REQUIRE(pt_8 == pt_88);

  REQUIRE(pt_8.getExponent() == 2);
  const auto pt_9 = pt_8.getSqrt();
  REQUIRE(pt_7 == pt_9);

  // NOLINTBEGIN(readability-magic-numbers) // these are random numbers I cant give every one a meaningfull name

  constexpr double REALLY_BIG_NUMBER = 98788987654321;

  double value      = REALLY_BIG_NUMBER;
  PreciseTime pt_10 = ns(static_cast<int64_t>(value));
  REQUIRE(pt_10.toDouble<ns>() == value);
  pt_10 -= us(44);
  value -= us2ns(44);
  REQUIRE(pt_10.toDouble<ns>() == value);
  pt_10 *= 1007;
  value *= 1007;
  REQUIRE(pt_10.toDouble<ns>() == value);
  pt_10 /= 7.;
  value /= 7.;
  REQUIRE(pt_10.toDouble<ns>() == value);
  pt_10 += ns(1);
  value += 1;
  REQUIRE(pt_10.toDouble<ns>() == value);

  // sign test ns
  PreciseTime pt_11                = s(3);
  pt_11                           -= ns(333);
  constexpr PreciseTime pt_11_exp  = ns(2999999667);
  REQUIRE(pt_11 == pt_11_exp);

  // sign test ns
  PreciseTime pt_12                = s(-3);
  pt_12                           += ns(333);
  constexpr PreciseTime pt_12_exp  = ns(-2999999667);
  REQUIRE(pt_12 == pt_12_exp);

  // sign test sec
  PreciseTime pt_13                = h(3);
  pt_13                           -= s(333);
  constexpr PreciseTime pt_13_exp  = h(2) + s(3600 - 333);
  REQUIRE(pt_13 == pt_13_exp);

  // sign test sec
  PreciseTime pt_14                = h(-3);
  pt_14                           += s(333);
  constexpr PreciseTime pt_14_exp  = h(-2) - s(3600 - 333);
  REQUIRE(pt_14 == pt_14_exp);

  constexpr PreciseTime pt_15 =
    PreciseTime(h(-3)) + PreciseTime(s(66)) - PreciseTime(ns(111111111));
  constexpr PreciseTime pt_15_exp = h(-2) - s(3600 - 66) - ns(111111111);
  REQUIRE(pt_15 == pt_15_exp);

  constexpr PreciseTime pt_16 =
    PreciseTime(h(3)) - PreciseTime(s(66)) + PreciseTime(ns(111111111));
  constexpr PreciseTime pt_16_exp = h(2) + s(3600 - 66) + ns(111111111);
  REQUIRE(pt_16 == pt_16_exp);

  constexpr PreciseTime pt_17 =
    PreciseTime(h(3)) - PreciseTime(s(66)) - PreciseTime(ns(111111111));
  constexpr PreciseTime pt_17_exp = h(2) + s(3600 - 66 - 1) + ns(888888889);
  REQUIRE(pt_17 == pt_17_exp);

  constexpr PreciseTime pt_18 =
    PreciseTime(-h(3)) + PreciseTime(s(66)) + PreciseTime(ns(111111111));
  constexpr PreciseTime pt_18_exp = h(-2) - s(3600 - 66 - 1) - ns(888888889);
  REQUIRE(pt_18 == pt_18_exp);

  constexpr PreciseTime pt_19     = PreciseTime(h(3)) - PreciseTime(ns(1));
  constexpr PreciseTime pt_19_exp = h(2) + s(3600 - 1) + ns(999999999);
  REQUIRE(pt_19 == pt_19_exp);

  constexpr PreciseTime pt_20 = PreciseTime(h(-3)) + PreciseTime(ns(1));
  constexpr PreciseTime pt_20_exp =
    PreciseTime(h(-2)) - PreciseTime(s(3600 - 1)) - PreciseTime(ns(999999999));
  REQUIRE(pt_20 == pt_20_exp);

  constexpr PreciseTime pt_21_exp =
    PreciseTime(h(1)) + PreciseTime(s(2)) + PreciseTime(ns(3));

  constexpr PreciseTime pt_22_exp =
    PreciseTime(h(1)) + PreciseTime(s(2)) + PreciseTime(ns(4));

  constexpr PreciseTime pt_23_exp =
    PreciseTime(h(1)) + PreciseTime(s(3)) + PreciseTime(ns(3));

  constexpr PreciseTime pt_24_exp =
    PreciseTime(h(2)) + PreciseTime(s(2)) + PreciseTime(ns(3));

  REQUIRE(pt_21_exp < pt_22_exp);
  REQUIRE(pt_21_exp < pt_23_exp);
  REQUIRE(pt_21_exp < pt_24_exp);

  REQUIRE(pt_22_exp > pt_21_exp);
  REQUIRE(pt_23_exp > pt_21_exp);
  REQUIRE(pt_24_exp > pt_21_exp);


  constexpr PreciseTime pt_25_exp = std::chrono::nanoseconds(5000);
  constexpr PreciseTime pt_26_exp = std::chrono::microseconds(1);

  REQUIRE(pt_25_exp > pt_26_exp);
  REQUIRE(pt_26_exp < pt_25_exp);

  constexpr PreciseTime pt_27_exp = std::chrono::nanoseconds(11);
  constexpr PreciseTime pt_28_exp = std::chrono::milliseconds(1);

  REQUIRE(pt_28_exp > pt_27_exp);
  REQUIRE(pt_27_exp < pt_28_exp);

  constexpr PreciseTime pt_29_exp = std::chrono::nanoseconds(1);
  constexpr PreciseTime pt_30_exp = std::chrono::seconds(1);

  REQUIRE(pt_30_exp > pt_29_exp);
  REQUIRE(pt_29_exp < pt_30_exp);

  // NOLINTEND(readability-magic-numbers)
}

TEST_CASE("test_precise_time_class_test_to_string") {  // NOLINT readability-function-cognitive-complexity // Happens inn big tests
  constexpr PreciseTime one_ns = std::chrono::nanoseconds(1);
  constexpr PreciseTime one_us = std::chrono::microseconds(1);
  constexpr PreciseTime one_ms = std::chrono::milliseconds(1);
  constexpr PreciseTime one_s  = std::chrono::seconds(1);
  constexpr PreciseTime one_m  = std::chrono::minutes(1);
  constexpr PreciseTime one_h  = std::chrono::hours(1);

  REQUIRE(one_ns.getTimeString(0) == "1ns");
  REQUIRE(one_us.getTimeString(1) == "1.0us");
  REQUIRE(one_ms.getTimeString(2) == "1.00ms");
  REQUIRE(one_s.getTimeString(3) == "1.000s");
  REQUIRE(one_m.getTimeString(4) == "1.0000m");
  REQUIRE(one_h.getTimeString(5) == "1.00000h");

  REQUIRE(one_ns.getMayorTimeString() == "1ns");
  REQUIRE(one_us.getMayorTimeString() == "1us");
  REQUIRE(one_ms.getMayorTimeString() == "1ms");
  REQUIRE(one_s.getMayorTimeString() == "1s");
  REQUIRE(one_m.getMayorTimeString() == "1m");
  REQUIRE(one_h.getMayorTimeString() == "1h");

  REQUIRE(one_ns.toString() ==
          "{h: [0]   m: [0]   s: [0]   ms: [0]   us: [0]   ns: [1]}^1");
  REQUIRE(one_us.toString() ==
          "{h: [0]   m: [0]   s: [0]   ms: [0]   us: [1]   ns: [0]}^1");
  REQUIRE(one_ms.toString() ==
          "{h: [0]   m: [0]   s: [0]   ms: [1]   us: [0]   ns: [0]}^1");
  REQUIRE(one_s.toString() ==
          "{h: [0]   m: [0]   s: [1]   ms: [0]   us: [0]   ns: [0]}^1");
  REQUIRE(one_m.toString() ==
          "{h: [0]   m: [1]   s: [0]   ms: [0]   us: [0]   ns: [0]}^1");
  REQUIRE(one_h.toString() ==
          "{h: [1]   m: [0]   s: [0]   ms: [0]   us: [0]   ns: [0]}^1");

  constexpr PreciseTime usns      = one_us + one_ns;
  constexpr PreciseTime msusns    = one_ms + usns;
  constexpr PreciseTime smsusns   = one_s + msusns;
  constexpr PreciseTime msmsusns  = one_m + smsusns;
  constexpr PreciseTime hmsmsusns = one_h + msmsusns;

  REQUIRE(usns.getTimeString(4) == "1.0010us");
  REQUIRE(msusns.getTimeString(4) == "1.0010ms");
  REQUIRE(smsusns.getTimeString(4) == "1.0010s");
  REQUIRE(msmsusns.getTimeString(4) == "1.0167m");
  REQUIRE(hmsmsusns.getTimeString(4) == "1.0169h");

  REQUIRE(usns.getMayorTimeString() == "1us");
  REQUIRE(msusns.getMayorTimeString() == "1ms");
  REQUIRE(smsusns.getMayorTimeString() == "1s");
  REQUIRE(msmsusns.getMayorTimeString() == "1m");
  REQUIRE(hmsmsusns.getMayorTimeString() == "1h");

  REQUIRE(usns.toString() ==
          "{h: [0]   m: [0]   s: [0]   ms: [0]   us: [1]   ns: [1]}^1");
  REQUIRE(msusns.toString() ==
          "{h: [0]   m: [0]   s: [0]   ms: [1]   us: [1]   ns: [1]}^1");
  REQUIRE(smsusns.toString() ==
          "{h: [0]   m: [0]   s: [1]   ms: [1]   us: [1]   ns: [1]}^1");
  REQUIRE(msmsusns.toString() ==
          "{h: [0]   m: [1]   s: [1]   ms: [1]   us: [1]   ns: [1]}^1");
  REQUIRE(hmsmsusns.toString() ==
          "{h: [1]   m: [1]   s: [1]   ms: [1]   us: [1]   ns: [1]}^1");

  constexpr PreciseTime s_squared            = one_s * one_s;
  constexpr PreciseTime s_cubed              = s_squared * one_s;
  constexpr PreciseTime s_to_the_forth_power = s_cubed * one_s;

  REQUIRE(s_squared.toString() ==
          "{h: [0]   m: [0]   s: [1]   ms: [0]   us: [0]   ns: [0]}^2");
  REQUIRE(s_cubed.toString() ==
          "{h: [0]   m: [0]   s: [1]   ms: [0]   us: [0]   ns: [0]}^3");
  REQUIRE(s_to_the_forth_power.toString() ==
          "{h: [0]   m: [0]   s: [1]   ms: [0]   us: [0]   ns: [0]}^4");
}

TEST_CASE("test_precise_time_class_better_than_double") {

  constexpr PreciseTime max              = PreciseTime::max();
  constexpr PreciseTime max_minus_one_ns = max - ns(1);
  REQUIRE(max != max_minus_one_ns);

  constexpr double max_d_s              = max.toDouble<s>();
  constexpr double max_d_s_minus_one_ns = max_d_s - ns2s(1);
  REQUIRE(max_d_s_minus_one_ns == max_minus_one_ns.toDouble<s>());
  REQUIRE(max_d_s == max_d_s_minus_one_ns);

  constexpr double max_d_ns              = max.toDouble<ns>();
  constexpr double max_d_ns_minus_one_ns = max_d_ns - 1;
  REQUIRE(max_d_ns_minus_one_ns == max_minus_one_ns.toDouble<ns>());
  REQUIRE(max_d_ns == max_d_ns_minus_one_ns);
}
