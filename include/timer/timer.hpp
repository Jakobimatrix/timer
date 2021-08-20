/*
 * \file: timer.hpp
 * \brief: Implements the PreciseTime class useing std::chrono.
 * Implements an easy to use timer class as well as an PreciseTime class.
 * \date: 17.07.2021
 * \author: Jakob Wandel
 * \version: 2.0
 */

#ifndef TIMER_H
#define TIMER_H

#include <math.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace tool {

constexpr double fastModF(double x, double &full) noexcept {
  full = static_cast<double>(static_cast<int64_t>(x));
  return x - full;
}

/*
 * \brief x2y(t x)
 * \param x time in x
 * \return time in y
 */
template <class t> constexpr auto mu2ns(t micro) {
  return static_cast<t>(micro * 1000.);
}

template <class t> constexpr auto ms2ns(t milli) {
  return static_cast<t>(milli * 1000000.);
}

template <class t> constexpr auto s2ns(t sec) {
  return static_cast<t>(sec * 1000000000.);
}

template <class t> constexpr auto ns2us(t ns) {
  return static_cast<t>(ns / 1000.);
}

template <class t> constexpr auto us2ns(t us) {
  return static_cast<t>(us * 1000.);
}

template <class t> constexpr auto ns2ms(t ns) {
  return static_cast<t>(ns / 1000000.);
}

template <class t> constexpr auto ns2s(t ns) {
  return static_cast<t>(ns / 1000000000.);
}

template <class t> constexpr auto ns2m(t ns) {
  return static_cast<t>(ns / (1000000000. * 60.));
}

template <class t> constexpr auto ns2h(t ns) {
  return static_cast<t>(ns / (1000000000. * 3600.));
}

template <class t> constexpr auto m2s(t m) { return static_cast<t>(m * 60.); }

template <class t> constexpr auto h2s(t h) { return static_cast<t>(h * 3600.); }

template <class t> constexpr auto h2m(t h) { return static_cast<t>(h * 60.); }

template <class t> constexpr auto h2ms(t h) {
  return static_cast<t>(h * 3600. * 1000.);
}

template <class t> constexpr auto h2us(t h) {
  return static_cast<t>(h * 3600. * 1000000.);
}

template <class t> constexpr auto h2ns(t h) {
  return static_cast<t>(h * 3600. * 1000000000.);
}

template <class t> constexpr auto s2us(t s) {
  return static_cast<t>(s * 1000000.);
}

template <class t> constexpr auto s2ms(t s) {
  return static_cast<t>(s * 1000.);
}

template <class t> constexpr auto s2m(t s) { return static_cast<t>(s / 60.); }

template <class t> constexpr auto s2h(t s) { return static_cast<t>(s / 3600.); }

template <class t> constexpr auto s2hf(t s) {
  return static_cast<t>(std::floor(s / 3600.));
}

class PreciseTime {
public:
  /// <Construction Area>
  ///
  ///
  constexpr PreciseTime() = default;

  constexpr PreciseTime(const std::chrono::nanoseconds &nano_seconds) noexcept
      : nano_seconds(nano_seconds) {
    sanitize();
  }

  constexpr PreciseTime(const std::chrono::microseconds &micro_seconds) noexcept
      : nano_seconds(std::chrono::nanoseconds(mu2ns(micro_seconds.count()))) {
    sanitize();
  }

  constexpr PreciseTime(const std::chrono::milliseconds &milli_seconds) noexcept
      : nano_seconds(std::chrono::nanoseconds(ms2ns(milli_seconds.count()))) {
    sanitize();
  }

  constexpr PreciseTime(const std::chrono::seconds &seconds) noexcept
      : seconds(seconds) {
    sanitize();
  }

  constexpr PreciseTime(const std::chrono::minutes &minutes) noexcept
      : seconds(std::chrono::seconds(m2s(minutes.count()))) {
    sanitize();
  }

  constexpr PreciseTime(const std::chrono::hours &hours) : hours(hours) {}

  /*
   * Initialization with double values. But I dont want to make PreciseTime
  class a templated one...

  template <class c> // requires std::is_same<c,
                     // std::chrono::nanoseconds>::value // c++20
  constexpr PreciseTime(
      double nanoseconds,
      typename std::enable_if<
          std::is_same<c, std::chrono::nanoseconds>::value>::type * =
          0) noexcept {
    setNanoseconds(nanoseconds);
  }

  template <class c> // requires std::is_same<c,
                     // std::chrono::microseconds>::value // c++20
  constexpr PreciseTime(
      double microseconds,
      typename std::enable_if<
          std::is_same<c, std::chrono::microseconds>::value>::type * =
          0) noexcept {
    setNanoseconds(us2ns(microseconds));
  }

  template <class c> // requires std::is_same<c,
                     // std::chrono::milliseconds>::value // c++20
  constexpr PreciseTime(
      double milliseconds,
      typename std::enable_if<
          std::is_same<c, std::chrono::milliseconds>::value>::type * =
          0) noexcept {
    setNanoseconds(ms2ns(milliseconds));
  }

  template <
      class c> // requires std::is_same<c, std::chrono::seconds>::value // c++20
  constexpr PreciseTime(
      double seconds,
      typename std::enable_if<
          std::is_same<c, std::chrono::seconds>::value>::type * = 0) noexcept {
    setSeconds(seconds);
  }

  template <
      class c> // requires std::is_same<c, std::chrono::minutes>::value // c++20
  constexpr PreciseTime(
      double minutes,
      typename std::enable_if<
          std::is_same<c, std::chrono::minutes>::value>::type * = 0) noexcept {
    setSeconds(m2s(minutes));
  }

  template <
      class c> // requires std::is_same<c, std::chrono::hours>::value // c++20
  constexpr PreciseTime(
      double hours,
      typename std::enable_if<std::is_same<c, std::chrono::hours>::value>::type
          * = 0) noexcept {
    setHours(hours);
  }
  */

  constexpr PreciseTime(const PreciseTime &oth) noexcept = default;
  constexpr PreciseTime(PreciseTime &&oth) noexcept = default;
  PreciseTime &operator=(const PreciseTime &other) noexcept = default;
  PreciseTime &operator=(PreciseTime &&other) noexcept = default;

  /*!
   * \brief max Returns the greatest time the PreciseTime class can hold
   */
  template <int EXPO = 1> static constexpr PreciseTime max() noexcept {
    PreciseTime ps(std::chrono::hours::max());
    ps.seconds = MAX_VALIDE_S;
    ps.nano_seconds = MAX_VALIDE_NS;
    ps.exponent = EXPO;
    return ps;
  }

  /*!
   * \brief min Returns the smallest time the PreciseTime class can hold
   */
  template <int EXPO = 1> static constexpr PreciseTime min() noexcept {
    PreciseTime ps(std::chrono::hours::min());
    ps.seconds = MIN_VALIDE_S;
    ps.nano_seconds = MIN_VALIDE_NS;
    ps.exponent = EXPO;
    return ps;
  }

  /*!
   * \brief zero Returns a PreciseTime of zero
   */
  template <int EXPO = 1> static constexpr PreciseTime zero() noexcept {
    PreciseTime ps;
    ps.exponent = EXPO;
    return ps;
  }

  /*!
   * \brief toDouble() Casts the PrecisionTime to a double value in the unit
   * given with the template
   * \return the time as a double in the given unit.
   */
  template <class c>
  constexpr
      typename std::enable_if<std::is_same<c, std::chrono::nanoseconds>::value,
                              double>::type
      toDouble() const noexcept {
    return h2ns(static_cast<double>(hours.count())) +
           s2ns(static_cast<double>(seconds.count())) +
           static_cast<double>(nano_seconds.count()) + sub_nano_seconds;
  }

  /*!
   * \brief toDouble() Casts the PrecisionTime to a double value in the unit
   * given with the template
   * \return the time as a double in the given unit.
   */
  template <class c>
  constexpr
      typename std::enable_if<std::is_same<c, std::chrono::microseconds>::value,
                              double>::type
      toDouble() const noexcept {
    return h2us(static_cast<double>(hours.count())) +
           s2us(static_cast<double>(seconds.count())) +
           ns2us(static_cast<double>(nano_seconds.count()) + sub_nano_seconds);
  }

  /*!
   * \brief toDouble() Casts the PrecisionTime to a double value in the unit
   * given with the template
   * \return the time as a double in the given unit.
   */
  template <class c>
  constexpr
      typename std::enable_if<std::is_same<c, std::chrono::milliseconds>::value,
                              double>::type
      toDouble() const noexcept {
    return h2ms(static_cast<double>(hours.count())) +
           s2ms(static_cast<double>(seconds.count())) +
           ns2ms(static_cast<double>(nano_seconds.count()) + sub_nano_seconds);
  }

  /*!
   * \brief toDouble() Casts the PrecisionTime to a double value in the unit
   * given with the template
   * \return the time as a double in the given unit.
   */
  template <class c>
  constexpr
      typename std::enable_if<std::is_same<c, std::chrono::seconds>::value,
                              double>::type
      toDouble() const noexcept {
    return h2s(static_cast<double>(hours.count())) +
           static_cast<double>(seconds.count()) +
           ns2s(static_cast<double>(nano_seconds.count()) + sub_nano_seconds);
  }

  /*!
   * \brief toDouble() Casts the PrecisionTime to a double value in the unit
   * given with the template
   * \return the time as a double in the given unit.
   */
  template <class c>
  constexpr
      typename std::enable_if<std::is_same<c, std::chrono::minutes>::value,
                              double>::type
      toDouble() const noexcept {
    return h2m(static_cast<double>(hours.count())) +
           s2m(static_cast<double>(seconds.count())) +
           ns2m(static_cast<double>(nano_seconds.count()) + sub_nano_seconds);
  }

  /*!
   * \brief toDouble() Casts the PrecisionTime to a double value in the unit
   * given with the template
   * \return the time as a double in the given unit.
   */
  template <class c>
  constexpr typename std::enable_if<std::is_same<c, std::chrono::hours>::value,
                                    double>::type
  toDouble() const noexcept {
    return static_cast<double>(hours.count()) +
           s2h(static_cast<double>(seconds.count())) +
           ns2h(static_cast<double>(nano_seconds.count()) + sub_nano_seconds);
  }

  /*!
   * \brief convert() Converts the PrecisionTime to the type given in the
   * template.
   * \return if preciseTime = 2h,3m,44s,40ms,66us,12ns > convert<nanoseconds>
   * will return (((3600*2h + 60*3m + 44s)*1000 + 40ms)*1000 + 66us)*1000 + 12ns
   * = 7364040066012ns
   */
  template <class c>
  constexpr
      typename std::enable_if<std::is_same<c, std::chrono::nanoseconds>::value,
                              c>::type
      convert() const noexcept {
    return std::chrono::nanoseconds(
        h2ns(hours.count()) + s2ns(seconds.count()) + nano_seconds.count());
  }

  /*!
   * \brief convert() Converts the PrecisionTime to the type given in the
   * template.
   * \return if preciseTime = 2h,3m,44s,40ms,66us,12ns > convert<microseconds>
   * will return ((3600*2h + 60*3m + 44s)*1000 + 40ms)*1000 + 66us =
   * 7364040066us
   */
  template <class c>
  constexpr
      typename std::enable_if<std::is_same<c, std::chrono::microseconds>::value,
                              c>::type
      convert() const noexcept {
    return std::chrono::microseconds(h2us(hours.count()) +
                                     s2us(seconds.count()) +
                                     ns2us(nano_seconds.count()));
  }

  /*!
   * \brief convert() Converts the PrecisionTime to the type given in the
   * template.
   * \return if preciseTime = 2h,3m,44s,40ms,66us,12ns > convert<milliseconds>
   * will return (3600*2h + 60*3m + 44s)*1000 + 40ms= 7364040ms
   */
  template <class c>
  constexpr
      typename std::enable_if<std::is_same<c, std::chrono::milliseconds>::value,
                              c>::type
      convert() const noexcept {
    return std::chrono::milliseconds(h2ms(hours.count()) +
                                     s2ms(seconds.count()) +
                                     ns2ms(nano_seconds.count()));
  }

  /*!
   * \brief convert() Converts the PrecisionTime to the type given in the
   * template.
   * \return if preciseTime = 2h,3m,44s,40ms,66us,12ns > convert<seconds> will
   * return 3600*2h + 60*3m + 44s= 7364s
   */
  template <class c>
  constexpr
      typename std::enable_if<std::is_same<c, std::chrono::seconds>::value,
                              c>::type
      convert() const noexcept {
    return std::chrono::seconds(h2s(hours.count())) + seconds;
  }

  /*!
   * \brief convert() Converts the PrecisionTime to the type given in the
   * template.
   * \return if preciseTime = 2h,3m,44s,40ms,66us,12ns > convert<minutes> will
   * return 60*2h + 3m = 123m
   */
  template <class c>
  constexpr
      typename std::enable_if<std::is_same<c, std::chrono::minutes>::value,
                              c>::type
      convert() const noexcept {
    return std::chrono::minutes(s2m(seconds.count() + h2m(hours.count())));
    ;
  }

  /*!
   * \brief convert() Converts the PrecisionTime to the type given in the
   * template.
   * \return if preciseTime = 2h,3m,44s,40ms,66us,12ns > convert<hours> will
   * return 2h
   */
  template <class c>
  constexpr typename std::enable_if<std::is_same<c, std::chrono::hours>::value,
                                    c>::type
  convert() const noexcept {
    return hours;
  }

  /*!
   * \brief get() Returns the part of the time of which the type was given.
   * \return if preciseTime = 2h,3m,44s,40ms,66us,12ns > get<nanoseconds> will
   * return 12ns
   */
  template <class c>
  constexpr
      typename std::enable_if<std::is_same<c, std::chrono::nanoseconds>::value,
                              c>::type
      get() const noexcept {
    const int64_t ns = nano_seconds.count();
    return std::chrono::nanoseconds(ns - (ns / 1000) * 1000);
  }

  /*!
   * \brief get() Returns the part of the time of which the type was given.
   * \return if preciseTime = 2h,3m,44s,40ms,66us,12ns > get<microseconds> will
   * return 66us
   */
  template <class c>
  constexpr
      typename std::enable_if<std::is_same<c, std::chrono::microseconds>::value,
                              c>::type
      get() const noexcept {
    const int64_t us = nano_seconds.count() / 1000;
    return std::chrono::microseconds(us - (us / 1000) * 1000);
  }

  /*!
   * \brief get() Returns the part of the time of which the type was given.
   * \return if preciseTime = 2h,3m,44s,40ms,66us,12ns > get<milliseconds> will
   * return 40ms
   */
  template <class c>
  constexpr
      typename std::enable_if<std::is_same<c, std::chrono::milliseconds>::value,
                              c>::type
      get() const noexcept {
    const int64_t ms = nano_seconds.count() / 1000000;
    return std::chrono::milliseconds(ms - (ms / 1000) * 1000);
  }

  /*!
   * \brief get() Returns the part of the time of which the type was given.
   * \return if preciseTime = 2h,3m,44s,40ms,66us,12ns > get<seconds> will
   * return 44s
   */
  template <class c>
  constexpr
      typename std::enable_if<std::is_same<c, std::chrono::seconds>::value,
                              c>::type
      get() const noexcept {
    const int64_t s = seconds.count();
    return std::chrono::seconds(s - (s / 60) * 60);
  }

  /*!
   * \brief get() Returns the part of the time of which the type was given.
   * \return if preciseTime = 2h,3m,44s,40ms,66us,12ns > get<minutes> will
   * return 3m
   */
  template <class c>
  constexpr
      typename std::enable_if<std::is_same<c, std::chrono::minutes>::value,
                              c>::type
      get() const noexcept {
    const int64_t m = seconds.count() / 60;
    return std::chrono::minutes(m - (m / 60) * 60);
  }

  /*!
   * \brief get Returns the part of the time of which the type was given.
   * \return if preciseTime = 2h,3m,44s,40ms,66us,12ns > get<hours> will
   * return 2h
   */
  template <class c>
  constexpr typename std::enable_if<std::is_same<c, std::chrono::hours>::value,
                                    c>::type
  get() const noexcept {
    return hours;
  }

  constexpr void setNanoseconds(double ns) noexcept {
    constexpr double max_int64_t =
        static_cast<double>(std::numeric_limits<int64_t>::max());
    constexpr double min_int64_t =
        static_cast<double>(std::numeric_limits<int64_t>::min());

    if (ns > max_int64_t || ns < min_int64_t) {
      setSeconds(ns2s(ns));
      return;
    }

    const int64_t ns_ = static_cast<int64_t>(ns);
    nano_seconds = std::chrono::nanoseconds(ns_);
    sub_nano_seconds += ns - static_cast<double>(ns_);
    sanitizeNS();
    sanitizeS();
  }

  constexpr void setSeconds(double s) noexcept {
    constexpr double max_int64_t =
        static_cast<double>(std::numeric_limits<int64_t>::max());
    constexpr double min_int64_t =
        static_cast<double>(std::numeric_limits<int64_t>::min());

    if (s > max_int64_t || s < min_int64_t) {
      setHours(s2h(s));
      return;
    }
    const int64_t sec_l = static_cast<int64_t>(s);
    seconds = std::chrono::seconds(sec_l);
    sanitizeS();
    setNanoseconds(s2ns(s - static_cast<double>(sec_l)));
  }

  constexpr void setHours(double h) noexcept {
    constexpr double max_int64_t =
        static_cast<double>(std::numeric_limits<int64_t>::max());
    constexpr double min_int64_t =
        static_cast<double>(std::numeric_limits<int64_t>::min());

    if (h > max_int64_t) {
      const auto exp = exponent;
      *this = PreciseTime::max();
      exponent = exp;
      return;
    } else if (h < min_int64_t) {
      const auto exp = exponent;
      *this = PreciseTime::min();
      exponent = exp;
      return;
    }

    const int64_t hours_l = static_cast<int64_t>(h);
    hours = std::chrono::hours(hours_l);
    setNanoseconds(h2ns(h - static_cast<double>(hours_l)));
  }

  ///
  ///
  /// </Construction Area>

  /*!
   * \brief getExponent returns the exponent of the PreciseTime.
   * \return The exponent.
   */
  constexpr int getExponent() const noexcept { return exponent; }

  constexpr void setExponent(int exponent) noexcept {
    this->exponent = exponent;
  }

private:
  /*!
   * \brief sanitizeNS If the internal counter for the nano seconds get over
   * 999999999 we adjust the nanoseconds by adding to the internal seconds.
   */
  constexpr void sanitizeNS() noexcept {
    if (nano_seconds > MAX_VALIDE_NS || nano_seconds < MIN_VALIDE_NS) {
      double seconds_from_nanoseconds = ns2s(nano_seconds.count());
      double carry_seconds = 0;
      fastModF(seconds_from_nanoseconds, carry_seconds);
      const int64_t carry_seconds_l = static_cast<int64_t>(carry_seconds);

      // substract the nanoseconds which made up a second
      nano_seconds = std::chrono::nanoseconds(nano_seconds.count() -
                                              s2ns(carry_seconds_l));
      // add them to the seconds
      seconds += std::chrono::seconds(carry_seconds_l);
    }
  }

  constexpr void sanitizeSubNs() noexcept {
    const double ns =
        static_cast<double>(static_cast<int64_t>(sub_nano_seconds));
    nano_seconds += std::chrono::nanoseconds(static_cast<int64_t>(ns));
    sub_nano_seconds -= ns;
  }

  /*!
   * \brief sanitizeS If the internal counter for the seconds 3599
   * we adjust the seconds by adding to the internal hours.
   */
  constexpr void sanitizeS() noexcept {
    /// Sanitize seconds
    if (seconds > MAX_VALIDE_S || seconds < MIN_VALIDE_S) {
      double hours_from_seconds = s2h(seconds.count());
      double carry_hours = 0;
      fastModF(hours_from_seconds, carry_hours);

      const int64_t carry_hours_l = static_cast<int64_t>(carry_hours);
      // substract the seconds which made up a hour

      const bool was_positive = isPositive();
      const bool expect_higher = carry_hours_l > 0;

      seconds = std::chrono::seconds(seconds.count() - h2s(carry_hours_l));
      // add them to the hours
      hours += std::chrono::hours(carry_hours_l);
      overflowProtection(was_positive, expect_higher);
    }
  }

  /*!
   * \brief sanitizeSign Sometimes when substracting we end up with different
   * signs: {10s} - {10ns} will lead to a internal value of {+10s and -10ns}. We
   * have to adjust this to {9s and 999999990ns}.
   * This assumes all values (hours, seconds, nanoseconds to be in a sane range
   */
  constexpr void sanitizeSign() noexcept {

    auto sameSign = [](int64_t a, int64_t b) {
      if (a == 0 || b == 0) {
        return true;
      }
      const bool s_a = a > 0 ? true : false;
      const bool s_b = b > 0 ? true : false;
      return s_a == s_b;
    };

    // fix s if it has a different sign to h
    if (!sameSign(seconds.count(), hours.count())) {
      if (hours.count() < 0) {
        hours += std::chrono::hours(1);
        seconds = seconds - std::chrono::seconds(3600);
      } else {
        hours -= std::chrono::hours(1);
        seconds = std::chrono::seconds(3600) + seconds;
      }
    }

    // fix ns if it now has different sign to s
    if (!sameSign(nano_seconds.count(), seconds.count())) {
      if (seconds.count() < 0) {
        seconds += std::chrono::seconds(1);
        nano_seconds = nano_seconds - std::chrono::nanoseconds(1000000000);
      } else {
        seconds -= std::chrono::seconds(1);
        nano_seconds = std::chrono::nanoseconds(1000000000) + nano_seconds;
      }
    } else if (!sameSign(nano_seconds.count(), hours.count())) {
      if (hours.count() < 0) {
        hours += std::chrono::hours(1);
        seconds = std::chrono::seconds(-3600 + 1);
        nano_seconds = nano_seconds - std::chrono::nanoseconds(1000000000);
      } else {
        hours -= std::chrono::hours(1);
        seconds = std::chrono::seconds(3600 - 1);
        nano_seconds = std::chrono::nanoseconds(1000000000) + nano_seconds;
      }
    }
  }

  /*!
   * \brief sanitize Combines all sanitize functions.
   */
  constexpr void sanitize() noexcept {
    sanitizeSubNs();
    sanitizeNS();
    sanitizeS();
    sanitizeSign();
  }

  /*!
   * \brief OverflowProtection checks if a operation (+-* /) did cause an over
   * or underflow. If it happened THIS is set to max() or min();
   * \param was_positive_before_operation Set to true if this was positive
   * before the operation.
   * \param expected_higher Set to True if you expect this (time) to be higher
   * than before the operation.
   */
  constexpr void overflowProtection(bool was_positive_before_operation,
                                    bool expected_higher) noexcept {
    if (was_positive_before_operation) {
      if (expected_higher) {
        if (!isPositive()) {
          *this = max();
          has_rolled_over = true;
        }
      }
    } else {
      if (!expected_higher) {
        if (isPositive()) {
          *this = min();
          has_rolled_over = true;
        }
      }
    }
  }

  constexpr bool overflowProtection(double hours_d) noexcept {
    const auto type = overflowProtectionType(hours_d);

    if (type == 1) {
      *this = max();
      has_rolled_over = true;
      return true;
    }
    if (type == -1) {
      *this = min();
      has_rolled_over = true;
      return true;
    }
    return false;
  }

  constexpr int overflowProtectionType(double hours_d) noexcept {
    constexpr double max_h =
        static_cast<double>(std::chrono::hours::max().count());
    constexpr double min_h =
        static_cast<double>(std::chrono::hours::min().count());

    if (hours_d > max_h) {
      return 1;
    }
    if (hours_d < min_h) {
      return -1;
    }
    return 0;
  }

public:
  /// <Arithmetics>
  ///
  ///

  constexpr bool isPositive() noexcept {
    if (hours.count() != 0) {
      return 0 < hours.count();
    }
    if (seconds.count() != 0) {
      return 0 < seconds.count();
    }
    if (nano_seconds.count() != 0) {
      return 0 < nano_seconds.count();
    }
    return sub_nano_seconds >= 0.;
  }

  constexpr bool isPositive(const PreciseTime &pt) noexcept {
    if (pt.hours.count() != 0) {
      return 0 < pt.hours.count();
    }
    if (pt.seconds.count() != 0) {
      return 0 < pt.seconds.count();
    }
    if (pt.nano_seconds.count() != 0) {
      return 0 < pt.nano_seconds.count();
    }
    return pt.sub_nano_seconds >= 0.;
  }

  constexpr void operator+=(const PreciseTime &pt) noexcept {
    assert(pt.exponent == exponent &&
           "You can not add different units like s + s^2");
    if (has_rolled_over) {
      return;
    }
    if (pt.has_rolled_over) {
      *this = pt;
      return;
    }

    const double hours_d = static_cast<double>(hours.count()) +
                           static_cast<double>(pt.hours.count());
    if (overflowProtection(hours_d)) {
      return;
    }

    const bool was_positive = isPositive();
    const bool expect_higher = isPositive(pt);

    sub_nano_seconds += pt.sub_nano_seconds;
    nano_seconds += pt.nano_seconds;
    seconds += pt.seconds;
    hours += pt.hours;

    sanitize();
    overflowProtection(was_positive, expect_higher);
  }

  constexpr PreciseTime operator+(const PreciseTime &pt) const noexcept {
    PreciseTime ret(*this);
    ret += pt;
    return ret;
  }

  constexpr void operator-=(const PreciseTime &pt) noexcept {
    assert(pt.exponent == exponent &&
           "You can not substartc different units like s - s^2");
    if (has_rolled_over) {
      return;
    }
    if (pt.has_rolled_over) {
      *this = pt;
      return;
    }

    const double hours_d = static_cast<double>(hours.count()) -
                           static_cast<double>(pt.hours.count());
    if (overflowProtection(hours_d)) {
      return;
    }

    const bool was_positive = isPositive();
    const bool expect_higher = !isPositive(pt);

    sub_nano_seconds -= pt.sub_nano_seconds;
    nano_seconds -= pt.nano_seconds;
    seconds -= pt.seconds;
    hours -= pt.hours;

    sanitize();
    overflowProtection(was_positive, expect_higher);
  }

  constexpr PreciseTime operator-(const PreciseTime &pt) const noexcept {
    PreciseTime ret(*this);
    ret -= pt;
    return ret;
  }

  constexpr void operator*=(const double multi) noexcept {
    if (has_rolled_over) {
      return;
    }
    const double hours_ = static_cast<double>(hours.count()) * multi;

    if (overflowProtection(hours_)) {
      return;
    }

    const double seconds_ = static_cast<double>(seconds.count()) * multi;

    const double nanoseconds_ =
        static_cast<double>(nano_seconds.count()) * multi +
        sub_nano_seconds * multi;

    PreciseTime ns;
    ns.setNanoseconds(nanoseconds_);

    PreciseTime s;
    s.setSeconds(seconds_);

    PreciseTime h;
    h.setHours(hours_);

    const auto exp = exponent;
    *this = ns + s + h;
    exponent = exp;
  }

  constexpr PreciseTime operator*(const double multi) const noexcept {
    PreciseTime ret(*this);
    ret *= multi;
    return ret;
  }

  constexpr PreciseTime operator*(const PreciseTime &pt) const noexcept {
    if (has_rolled_over) {
      return *this;
    }
    if (pt.has_rolled_over) {
      return pt;
    }

    const double resulting_s =
        pt.toDouble<std::chrono::seconds>() * toDouble<std::chrono::seconds>();

    const double hours_d = s2h(resulting_s);
    PreciseTime ret;
    const auto type = ret.overflowProtectionType(hours_d);
    if (type == 1) {
      ret = max();
      ret.has_rolled_over = true;
    } else if (type == -1) {
      ret = min();
      ret.has_rolled_over = true;
    }

    const double ns = s2ns(resulting_s);
    ret.setNanoseconds(ns);
    ret.exponent = exponent + pt.exponent;
    return ret;
  }

  constexpr PreciseTime operator/(const double div) const noexcept {
    PreciseTime ret(*this);
    ret *= (1. / div);
    return ret;
  }

  constexpr void operator/=(const double div) {
    (*this) *= (1.0 / div); // this is not as precise
    return;
  }

  constexpr PreciseTime operator/(const PreciseTime &pt) const noexcept {
    if (has_rolled_over) {
      return *this;
    }
    if (pt.has_rolled_over) {
      return pt;
    }

    const double resulting_s =
        pt.toDouble<std::chrono::seconds>() / toDouble<std::chrono::seconds>();

    const double hours_d = s2h(resulting_s);
    PreciseTime ret;
    const auto type = ret.overflowProtectionType(hours_d);
    if (type == 1) {
      ret = max();
      ret.has_rolled_over = true;
    } else if (type == -1) {
      ret = min();
      ret.has_rolled_over = true;
    }

    const double ns = s2ns(resulting_s);
    ret.setNanoseconds(ns);
    ret.exponent = exponent - pt.exponent;
    return ret;
  }

  /*!
   * \brief Returns an array of 6 int64_t values containing the time components
   * itemized into:
   * {"nanoseconds", "microseconds", "milliseconds", "seconds", "minutes",
   * "hours"};
   * You can use the get<td::chrono::duration>() methode if you dont need all
   * components.
   */
  constexpr std::array<int64_t, 6> getSeperatedTimeComponents() const noexcept {
    return {get<std::chrono::nanoseconds>().count(),
            get<std::chrono::microseconds>().count(),
            get<std::chrono::milliseconds>().count(),
            get<std::chrono::seconds>().count(),
            get<std::chrono::minutes>().count(),
            get<std::chrono::hours>().count()};
  }

  constexpr bool hasRolledOver() { return has_rolled_over; }

  static PreciseTime sqrt(PreciseTime &pt) noexcept {
    assert(pt.exponent % 2 == 0 &&
           "squareroot of Precise time with odd exponent not supported.");

    const double ns = s2ns(std::sqrt(pt.toDouble<std::chrono::seconds>()));
    const int exp = pt.exponent;
    pt = zero();
    pt.setNanoseconds(ns);
    pt.sanitize();

    pt.exponent = exp / 2;

    return pt;
  }

  void sqrt() noexcept { PreciseTime::sqrt(*this); }

  PreciseTime getSqrt() const noexcept {
    PreciseTime ret = *this;
    PreciseTime::sqrt(ret);
    return ret;
  }

  ///
  ///
  /// </Arithmetics>

  /// <Comparisons>
  ///
  ///
  constexpr bool operator==(const PreciseTime &pt) const noexcept {
    return pt.exponent == exponent && pt.nano_seconds == nano_seconds &&
           pt.seconds == seconds && pt.hours == hours;
  }

  constexpr bool operator!=(const PreciseTime &pt) const noexcept {
    return !(*this == pt);
  }

  constexpr bool operator<(const PreciseTime &pt) const noexcept {
    if (hours == pt.hours) {
      if (seconds == pt.seconds) {
        if (nano_seconds == pt.nano_seconds) {
          return false;
        } else if (nano_seconds < pt.nano_seconds) {
          return true;
        }
      } else if (seconds < pt.seconds) {
        return true;
      }
    } else if (hours < pt.hours) {
      return true;
    }
    return false;
  }

  constexpr bool operator>(const PreciseTime &pt) const noexcept {
    if (*this < pt) {
      return false;
    }
    if (*this == pt) {
      return false;
    }
    return true;
  }

  constexpr bool operator<=(const PreciseTime &pt) const noexcept {
    if (*this > pt) {
      return false;
    }
    return true;
  }

  constexpr bool operator>=(const PreciseTime &pt) const noexcept {
    if (*this < pt) {
      return false;
    }
    return true;
  }
  ///
  ///
  /// </Comparisons>

  /*!
   * \brief Implements a clean print for the current time:
   * {h: [xxxx]   m: [xx]   s: [xxx]   ms: [xxx]   us: [xxx]  ns:
   * [xxx]}^exponent
   */
  friend std::ostream &operator<<(std::ostream &os,
                                  const PreciseTime &pt) noexcept {
    auto blanks = [](int64_t num) {
      const int64_t i = std::abs(num);
      if (i < 10) {
        return "  ";
      } else if (i < 100) {
        return " ";
      }
      return "";
    };

    const int64_t hours_ = pt.get<std::chrono::hours>().count();
    const int64_t minutes_ = pt.get<std::chrono::minutes>().count();
    const int64_t seconds_ = pt.get<std::chrono::seconds>().count();
    const int64_t ms = pt.get<std::chrono::milliseconds>().count();
    const int64_t us = pt.get<std::chrono::microseconds>().count();
    const int64_t ns = pt.get<std::chrono::nanoseconds>().count();
    const std::string exp = std::to_string(pt.exponent);

    // clang-format off
    os << "{h: ["  << hours_   << "] " << blanks(hours_)
       << "m: ["  << minutes_ << "] " << blanks(minutes_)
       << "s: ["  << seconds_ << "] " << blanks(seconds_)
       << "ms: [" << ms << "] " << blanks(ms)
       << "us: [" << us << "] " << blanks(us)
       << "ns: [" << ns  << "]}^"<< exp;
    // clang-format on
    if (pt.has_rolled_over) {
      os << "\n+-----------------------------+\n"
            "| Over- or Underflow detected |\n"
            "+-----------------------------+";
    }
    return os;
  }

  constexpr PreciseTime getMayorTime() const noexcept {
    if (get<std::chrono::hours>().count() > 0) {
      return PreciseTime(get<std::chrono::hours>());
    }
    if (get<std::chrono::minutes>().count() > 0) {
      return PreciseTime(get<std::chrono::minutes>());
    }
    if (get<std::chrono::seconds>().count() > 0) {
      return PreciseTime(get<std::chrono::seconds>());
    }
    if (get<std::chrono::milliseconds>().count() > 0) {
      return PreciseTime(get<std::chrono::milliseconds>());
    }
    if (get<std::chrono::microseconds>().count() > 0) {
      return PreciseTime(get<std::chrono::microseconds>());
    }
    if (get<std::chrono::nanoseconds>().count() > 0) {
      return PreciseTime(get<std::chrono::nanoseconds>());
    }
    return zero();
  }

  std::string getMayorTimeString() const noexcept {
    if (get<std::chrono::hours>().count() > 0) {
      return std::to_string(get<std::chrono::hours>().count()) + "h";
    }
    if (get<std::chrono::minutes>().count() > 0) {
      return std::to_string(get<std::chrono::minutes>().count()) + "m";
    }
    if (get<std::chrono::seconds>().count() > 0) {
      return std::to_string(get<std::chrono::seconds>().count()) + "s";
    }
    if (get<std::chrono::milliseconds>().count() > 0) {
      return std::to_string(get<std::chrono::milliseconds>().count()) + "ms";
    }
    if (get<std::chrono::microseconds>().count() > 0) {
      return std::to_string(get<std::chrono::microseconds>().count()) + "us";
    }
    if (get<std::chrono::nanoseconds>().count() > 0) {
      return std::to_string(get<std::chrono::nanoseconds>().count()) + "ns";
    }
    return std::to_string(sub_nano_seconds) + "ns";
  }

  /*!
   * \brief Returns the time in its highst format. E.g.:
   * 44s,40ms,66us,12ns.getTimeString(4) --> "44.04s"
   * 66us,12ns.getTimeString(4) --> "66.02us"
   * \param precision The amount of numbers after the most significant power of
   * ten.
   * \return String of time in highst format.
   */
  std::string getTimeString(int precision) const noexcept {
    double time_d = toDouble<std::chrono::nanoseconds>();
    std::stringstream stream;

    std::string unit = "ns";

    if (get<std::chrono::hours>().count() > 0) {
      time_d = ns2h(time_d);
      unit = "h";
    }
    if (get<std::chrono::minutes>().count() > 0) {
      time_d = ns2m(time_d);
      unit = "m";
    }
    if (get<std::chrono::seconds>().count() > 0) {
      time_d = ns2s(time_d);
      unit = "s";
    }
    if (get<std::chrono::milliseconds>().count() > 0) {
      time_d = ns2ms(time_d);
      unit = "ms";
    }
    if (get<std::chrono::microseconds>().count() > 0) {
      time_d = ns2us(time_d);
      unit = "us";
    }

    const int power = static_cast<int>(log10(std::abs(time_d)));
    const int num_decimal_places = std::max(0, precision - power);
    stream << std::fixed << std::setprecision(num_decimal_places) << time_d
           << unit;

    return stream.str();
  }

  static constexpr std::chrono::seconds MAX_VALIDE_S =
      std::chrono::seconds(3599);
  static constexpr std::chrono::seconds MIN_VALIDE_S =
      std::chrono::seconds(-3599);
  static constexpr std::chrono::nanoseconds MAX_VALIDE_NS =
      std::chrono::nanoseconds(999999999);
  static constexpr std::chrono::nanoseconds MIN_VALIDE_NS =
      std::chrono::nanoseconds(-999999999);

  double sub_nano_seconds = 0.;

  // internal value to save the nano seconds
  std::chrono::nanoseconds nano_seconds = std::chrono::nanoseconds::zero();

  // internal value to save the seconds
  std::chrono::seconds seconds = std::chrono::seconds::zero();

  // internal value to save the hours
  std::chrono::hours hours = std::chrono::hours::zero();

  // internal value to save the unit: s, s^2, s^3...
  int exponent = 1;

  // keep track if we have an overor underflow.
  bool has_rolled_over = false;
};

class Timer {
public:
  Timer() = default;
  Timer(const std::vector<PreciseTime> &given_measurements,
        const std::string label) {
    measurements[label] = given_measurements;
  }

  /*!
   * \brief start starts a new measurement.
   * \param s The name under which the measurement/timer shall be saved.
   */
  void start(const std::string &s = "") noexcept {
    const Timer::precisionClock::time_point start = precisionClock::now();
    begin_measurements[s] = start;
  }

  /*!
   * \brief Stops a started measurement, computes the time since start()
   * and saves it.
   * \param s The name under which the measurement/timer shall be saved.
   */
  void stop(const std::string &s = "") noexcept {
    const Timer::precisionClock::time_point stop = precisionClock::now();
    const begin_measurements_it start_ = begin_measurements.find(s);
    if (start_ == begin_measurements.end()) {
      // TODO debugMsg: The timer with name s was never started
      return;
    }

    const std::chrono::nanoseconds duration =
        std::chrono::duration_cast<std::chrono::nanoseconds>(stop -
                                                             start_->second);
    measurements[s].emplace_back(duration);
  }

  /*!
   * \brief Struckt to hold information about a Histogram.
   */
  struct Histogram {

    /*!
     * \brief Struckt to store information about how much measurements fall into
     * a Range = Bucket.
     */
    struct Bucket {
      PreciseTime begin;
      PreciseTime end;
      int num = 0;

      /*!
       * \brief Calculates the time between in the center of the bucket/Range.
       * \return a PreciseTime: The center of the bucket.
       */
      PreciseTime getBucketCenter() const noexcept {
        return (end - begin) / 2. + begin;
      }
    };

    std::vector<Bucket> buckets;
    PreciseTime bucket_size;
    int max_num_in_bucket = 0;

    /*!
     * \brief Calculates the bucket size according to Scott's Rule.
     * \param num_values Number of measurements
     * \param std_dev The standard deviation of the measurements.
     * \return The computed bucket size.
     */
    PreciseTime scottsRuleBucketSize(size_t num_values,
                                     const PreciseTime &std_dev) const
        noexcept {
      const double num_good_values = static_cast<double>(num_values);
      return std_dev * 3.49 * 1. / std::cbrt(num_good_values);
    }

    /*!
     * \brief Initiates the inner vector of Buckets to be ready to input values.
     * \param bucket_size_ The size of one Bucket in the histogram.
     * \param min The minimal value/Time the histogram shall display.
     * \param max The maximal value/Time the histogram shall display.
     * \return The computed bucket size.
     */
    void initBuckets(const PreciseTime &bucket_size_, const PreciseTime &min,
                     const PreciseTime &max) {
      constexpr PreciseTime SMALLEST_BUCKET_SIZE(std::chrono::nanoseconds(1));
      bucket_size = bucket_size_;
      if (bucket_size < SMALLEST_BUCKET_SIZE) {
        bucket_size = SMALLEST_BUCKET_SIZE;
      }
      assert(max > min && "Max time lower than min time.");
      const PreciseTime range = max - min;
      const size_t num_buckets = static_cast<size_t>(
          ceil(range.toDouble<std::chrono::nanoseconds>() /
               bucket_size.toDouble<std::chrono::nanoseconds>()));

      buckets.reserve(num_buckets);

      PreciseTime iterator = min;
      while (iterator < max) {
        const auto end = iterator + bucket_size;
        Histogram::Bucket b{iterator, end, 0};
        buckets.push_back(std::move(b));
        iterator = end;
      }
    }

    /*!
     * \brief Returns a number of white spaces in a speciffic color. Works only
     * in Linux shell. \param color A number for different color output (e.g.
     * the position in an array). \param num_tiles The number of spaces to be
     * colored. \return A string with num_tiles spaces with the command to color
     * these spaces.
     */
    static std::string colorCmdBgTile(size_t color, size_t num_tiles) {
      constexpr size_t NUM_COLORS = 10;
      if (color >= NUM_COLORS) {
        const size_t div = color / NUM_COLORS;
        color = color - NUM_COLORS * div;
      }
      const std::string empty_tiles(num_tiles, ' ');
      const std::string end("\033[0m");
      std::string color_s;
      // https://misc.flogisoft.com/bash/tip_colors_and_formatting
      switch (color) {
      case 0:
        color_s = std::string("\033[48:5:82:0m"); // light green
        break;
      case 1:
        color_s = std::string("\033[48:5:208:0m"); // orange
        break;
      case 2:
        color_s = std::string("\033[48:5:256:0m"); // white
        break;
      case 3:
        color_s = std::string("\033[48:5:9:0m"); // red
        break;
      case 4:
        color_s = std::string("\033[48:5:249:0m"); // grey
        break;
      case 5:
        color_s = std::string("\033[48:5:11:0m"); // yellow
        break;
      case 6:
        color_s = std::string("\033[48:5:225:0m"); // pink
        break;
      case 7:
        color_s = std::string("\033[48:5:90:0m"); // dark violet
        break;
      case 8:
        color_s = std::string("\033[48:5:45:0m"); // light blue
        break;
      case 9:
        color_s = std::string("\033[48:5:2:0m"); // darker green
        break;
      }
      return color_s + empty_tiles + end;
    }
  };

  /*!
   * \brief Struct to bundel the results of N measurements.
   */
  struct Result {

    /*!
     * \brief Set the range for the outliner detection: Every measurement
     * smaller than mean - dev*n_times_deviation and greater than mean -
     * dev*n_times_deviation is considdered to be an outliner. n_times_deviation
     * the range for the outliner detection. Default is 3.5 which is more
     * than 99.9% of all measurements if the measurements follow a normal
     * distribution.
     */
    void setOutlinerRange(double n_times_deviation) noexcept {
      outliner_range = n_times_deviation;
    }

    /*!
     * \brief Helper function to calculate the maximal size of character in the
     * cmd plot, depending on num_char_terminal_width
     */
    size_t calcPlotSize() const noexcept {
      // clang-format off
      // <   BucketSizeInfo  ><PLOT>  <-- maximal r.num_char_terminal_width characters.
      // [213.0ns - 214.0ns] |######
      // clang-format on
      constexpr size_t BucketSizeInfo = 22;
      return std::max(50ul, num_char_terminal_width - BucketSizeInfo);
    }

    /*!
     * \brief Sets the width of the terminal for the drawing of the histogram.
     * Default is 80 characters.
     */
    void setCharWidthOfTerminal(size_t terminal_width) noexcept {
      num_char_terminal_width = terminal_width;
    }

    /*!
     * \brief Nice output stream for one Results statistical values. Expects
     * Results to be complete Initiated.
     */
    void streamOutBaseStatistics(std::ostream &os, const Result &r) const {
      os << "###Result of <" << r.timer_name << ">###"
         << "\n"
         << "E{X}: \t" << r.mean << "\n"
         << "Median{X}: \t" << r.median << "\n"
         << "Max{X}: " << r.max_measurement << "\n"
         << "Min{X}: " << r.min_measurement << "\n"
         << "D{X}: \t" << r.standard_derivation << "\n"
         << "N measurments: \t" << r.number_measurements << "\n"
         << "N outliners.: \t" << r.number_outliners << "\n";
    }

    /*!
     * \brief Nice output stream for one Results histogram. Expects Results to
     * be complete Initiated.
     */
    void streamOutHistogram(std::ostream &os, const Result &r) const {

      const double smallest_unit =
          r.h.max_num_in_bucket / static_cast<double>(r.calcPlotSize());
      int num_buckets_skipped = 0;
      PreciseTime begin_skip;
      PreciseTime end_skip;
      auto streamBucketsSkipped = [&os, &num_buckets_skipped, &begin_skip,
                                   &end_skip]() {
        os << "[" << begin_skip.getTimeString(3) << " - "
           << end_skip.getTimeString(3) << "] |"
           << "skipped " << num_buckets_skipped << " insignificant bars"
           << "\n";
      };

      for (const auto &bucket : r.h.buckets) {
        const size_t num_units =
            static_cast<size_t>(bucket.num / smallest_unit);
        if (num_units == 0) {
          if (num_buckets_skipped == 0) {
            begin_skip = bucket.begin;
          }
          num_buckets_skipped++;
          end_skip = bucket.end;
          continue;
        }
        if (num_buckets_skipped > 0) {
          streamBucketsSkipped();
          num_buckets_skipped = 0;
        }

        os << "[" << bucket.begin.getTimeString(3) << " - "
           << bucket.end.getTimeString(3) << "] |"
           << std::string(num_units, '#') << "\n";
      }
      if (num_buckets_skipped > 0) {
        streamBucketsSkipped();
        num_buckets_skipped = 0;
      }
    }

    /*!
     * \brief Implements a clean print for the statistics. Expects Results to be
     * complete Initiated.
     */
    friend std::ostream &operator<<(std::ostream &os, const Result &r) {
      r.streamOutBaseStatistics(os, r);
      r.streamOutHistogram(os, r);
      return os;
    }

    /*!
     * \brief Implements a clean print for an vector of statistics. Expects
     * Results to be complete Initiated.
     */
    friend std::ostream &operator<<(std::ostream &os,
                                    const std::vector<Result> &rs) {
      if (rs.size() == 0) {
        return os;
      }

      if (rs.size() == 1) {
        os << rs[0];
        return os;
      }

      // find the minimal bucket size and histogram start/end
      PreciseTime min_bucket_size = PreciseTime::max();
      PreciseTime histogram_start = PreciseTime::max();
      PreciseTime histogram_end = PreciseTime::zero();
      std::vector<size_t> indexes_used;
      for (size_t i = 0; i < rs.size(); ++i) {
        const Result &r = rs[i];
        if (r.h.buckets.size() == 0) {
          continue;
        }
        indexes_used.push_back(i);
        min_bucket_size = std::min(min_bucket_size, r.h.bucket_size);
        histogram_start = std::min(histogram_start, r.h.buckets[0].begin);
        histogram_end = std::max(histogram_end, r.h.buckets.back().end);
      }

      int max_num_in_bucket = 0;
      size_t max_name_length = 0;
      for (size_t i : indexes_used) {
        const auto &result = rs[i];
        result.streamOutBaseStatistics(os, result);
        max_num_in_bucket =
            std::max(max_num_in_bucket, result.h.max_num_in_bucket);
        max_name_length = std::max(max_name_length, result.timer_name.size());
      }

      os << "\nLegend:\n";
      constexpr size_t MIN_LEGEND_COLOR_LENGTH = 10;
      size_t legend_color_length = max_name_length + MIN_LEGEND_COLOR_LENGTH;
      for (size_t i : indexes_used) {
        const auto &result = rs[i];
        const size_t length = legend_color_length - result.timer_name.size();
        os << result.timer_name << Histogram::colorCmdBgTile(i, length) << "\n";
      }
      os << "\n";

      // stores for each histogram at which bucket we are
      std::vector<size_t> current_bucket(rs.size());
      // contains number_inside_bucket and histogram_index == color
      std::vector<std::pair<size_t, size_t>> number_color;
      number_color.resize(indexes_used.size());

      Histogram h;
      h.initBuckets(min_bucket_size, histogram_start, histogram_end);

      const double smallest_unit =
          max_num_in_bucket / static_cast<double>(rs[0].calcPlotSize());
      int num_buckets_skipped = 0;
      PreciseTime begin_skip;
      PreciseTime end_skip;
      auto streamBucketsSkipped = [&os, &num_buckets_skipped, &begin_skip,
                                   &end_skip]() {
        os << "[" << begin_skip.getTimeString(3) << " - "
           << end_skip.getTimeString(3) << "] |"
           << "skipped " << num_buckets_skipped << " insignificant bars"
           << "\n";
      };

      for (const auto &bucket_comb_histo : h.buckets) {

        for (size_t i : indexes_used) {
          const auto &current_buckets = rs[i].h.buckets;
          if (current_bucket[i] >= current_buckets.size()) {
            number_color[i] =
                std::pair<size_t, size_t>(0, i); // not drawn anymore
            continue; // already all values reorganized in h.buckets and printed
          }
          size_t current_bucket_nr = current_bucket[i];
          // Todo interpolation would be more precise than "fitting the center
          // into the new histogram bucket". But than again, one should use a
          // graphical tool for a precise histogram anyway.
          const auto &current_bucket_ref = current_buckets[current_bucket_nr];
          const auto bucket_comb_histo_center =
              bucket_comb_histo.getBucketCenter();

          if (bucket_comb_histo_center < current_bucket_ref.begin) {
            number_color[i] = std::pair<size_t, size_t>(0, i); // not drawn
            continue; // this histogram starts later
          }

          if (bucket_comb_histo_center > current_bucket_ref.end) {
            // we have to increase the current bucket
            current_bucket_nr = ++current_bucket[i];
            if (current_bucket[i] == current_buckets.size()) {
              number_color[i] =
                  std::pair<size_t, size_t>(0, i); // not drawn anymore
              continue; // we are at the end, no more buckets in
                        // current_buckets;
            }
          }
          // the current_bucket and bucket_comb_histo are aligned, we save
          // number and index.
          number_color[i] = std::pair<size_t, size_t>(
              current_buckets[current_bucket_nr].num, i);
        }

        sort(number_color.begin(), number_color.end());
        size_t current_cursor_position = 0;
        std::string data("");
        for (const auto &nc : number_color) {
          const size_t num_units =
              static_cast<size_t>(nc.first / smallest_unit);
          if (current_cursor_position == num_units) {
            continue; // skip this, we cannot print two colors onto each other
          }
          const size_t cursor_diff = num_units - current_cursor_position;
          data += Histogram::colorCmdBgTile(nc.second, cursor_diff);
          current_cursor_position += cursor_diff;
        }

        if (current_cursor_position == 0) {
          if (num_buckets_skipped == 0) {
            begin_skip = bucket_comb_histo.begin;
          }
          num_buckets_skipped++;
          end_skip = bucket_comb_histo.end;
          continue;
        }
        if (num_buckets_skipped > 0) {
          streamBucketsSkipped();
          num_buckets_skipped = 0;
        }

        os << "[" << bucket_comb_histo.begin.getTimeString(3) << " - "
           << bucket_comb_histo.end.getTimeString(3) << "] |" << data << "\n";
      }
      if (num_buckets_skipped > 0) {
        streamBucketsSkipped();
        num_buckets_skipped = 0;
      }
      return os;
    }

    std::string timer_name;
    PreciseTime min_measurement = PreciseTime::max();
    PreciseTime max_measurement = PreciseTime::min();
    PreciseTime median;
    PreciseTime mean;
    PreciseTime standard_derivation = PreciseTime::max();
    size_t number_measurements = 0;
    size_t number_outliners = 0;
    double outliner_range = 3.5;
    size_t num_char_terminal_width = 80;
    std::vector<char> is_outliner;
    Histogram h;
  };

  /*!
   * \brief getResult Calculates for the given timer the statics.
   * ! Statistics only make much sense if you have more than 1000 measurements!
   * \param name The name of the timer the statistics shall be calculated for.
   * \param result Will contain the statistical data
   * \param sort_measurements To calculate the median, the measurements must be
   * sorted. If you want to keep the measurements in the order they where
   * recorded, you have to set this to false.
   * \return false if the name of the
   * given timer doesn't exist or has less than 3 measurements.
   */
  bool getResult(const std::string &name, Result &result,
                 bool sort_measurements = true) noexcept {

    const auto timer = measurements.find(name);
    if (timer == measurements.end()) {
      return false;
    }

    result.number_measurements = timer->second.size();
    if (result.number_measurements < 3) {
      return false;
    }

    result.is_outliner = std::vector<char>(result.number_measurements, false);

    if (sort_measurements) {
      result.median = findMedian(timer->second);
    } else {
      result.median = findMedianCopy(timer->second);
    }

    auto setMean = [&timer, &result]() {
      PreciseTime sum;
      for (size_t i = 0; i < result.number_measurements; ++i) {
        const auto &measurement = timer->second[i];
        if (!result.is_outliner[i]) {
          sum += measurement;
        }
      }
      result.mean =
          sum / (result.number_measurements - result.number_outliners);
    };

    auto setMinMax = [&timer, &result]() {
      result.max_measurement = PreciseTime::min();
      result.min_measurement = PreciseTime::max();
      for (size_t i = 0; i < result.number_measurements; ++i) {
        const auto &measurement = timer->second[i];
        if (!result.is_outliner[i]) {
          if (measurement > result.max_measurement) {
            result.max_measurement = measurement;
          }
          if (measurement < result.min_measurement) {
            result.min_measurement = measurement;
          }
        }
      }
    };

    auto setDeviation = [&timer, &result]() {
      // variance
      PreciseTime var_sum;
      var_sum.setExponent(2);
      for (size_t i = 0; i < result.number_measurements; ++i) {
        const auto &measurement = timer->second[i];
        if (!result.is_outliner[i]) {
          const PreciseTime diff_pt = measurement - result.mean;
          var_sum += diff_pt * diff_pt;
        }
      }
      PreciseTime variance =
          var_sum / (result.number_measurements - result.number_outliners - 1);
      variance.sqrt();
      result.standard_derivation = variance;
    };

    auto setOutliners = [&timer, &result]() {
      const auto dev_range = result.standard_derivation * result.outliner_range;
      const auto outliner_distance_top = result.mean + dev_range;
      const auto outliner_distance_bot = result.mean - dev_range;
      size_t num_outliners = 0;
      for (size_t i = 0; i < result.number_measurements; ++i) {
        const auto &measurement = timer->second[i];
        const bool is_otliner = (measurement < outliner_distance_bot ||
                                 outliner_distance_top < measurement);
        result.is_outliner[i] = is_otliner;
        num_outliners += is_otliner ? 1 : 0;
      }
      result.number_outliners = num_outliners;
    };

    auto setHistogram = [&timer, &result]() {
      const size_t number_values =
          result.number_measurements - result.number_outliners;
      const auto bucket_size = result.h.scottsRuleBucketSize(
          number_values, result.standard_derivation);
      result.h.initBuckets(bucket_size, result.min_measurement,
                           result.max_measurement);

      for (size_t i = 0; i < result.number_measurements; ++i) {
        const auto &measurement = timer->second[i];
        if (!result.is_outliner[i]) {
          for (auto &bucket : result.h.buckets) {
            if (bucket.begin <= measurement && measurement <= bucket.end) {
              bucket.num++;
              break;
            }
          }
        }
      }

      for (const auto &bucket : result.h.buckets) {
        result.h.max_num_in_bucket =
            std::max(result.h.max_num_in_bucket, bucket.num);
      }
    };

    result.timer_name = name;

    // default deviation is maximal -> all values are inside outliner_range *
    // deviation, no outliners
    setMean();
    // now we can calculate the deviation estimation
    setDeviation();

    // detect outliners with deviation
    if (result.standard_derivation > PreciseTime(std::chrono::nanoseconds(1))) {
      setOutliners();
      // estimate a better mean without outliners.
      setMean();
      // estimate the deviation without outliners
      setDeviation();
    }

    setMinMax();

    setHistogram();

    return true;
  }

  /*!
   * \brief Calculates for all saved measurments/timers the statistics and
   * prints them.
   */
  friend std::ostream &operator<<(std::ostream &os, Timer &t) {
    for (const auto &timer : t.measurements) {
      Result r;
      t.getResult(timer.first, r);
      os << "Timer: " << timer.first << std::endl << r << "\n";
    }
    return os;
  }

  /*!
   * \brief Writes all measurements from all timers into the given file
   * (appends) for further analysis with Excel or Matlab. \template a
   * std::chrono duration in which the time (as double values) should be
   * printed. \param file_name The name of the file to write into. If its a
   * path, the path must exist. \param seperator A character to seperate the
   * input fields. \return true if writeing was successfull.
   */
  template <class T>
  bool measurementsToFile(const std::string &file_name, char seperator) {

    const size_t num_timers = measurements.size();
    if (num_timers == 0) {
      return false;
    }

    std::ofstream file;
    file.open(file_name.c_str(), std::ios_base::app);
    if (file.bad()) {
      return false;
    }

    auto inputIntoFile = [&file](std::string &input_line) {
      input_line.pop_back();
      input_line += "\n";
      std::copy(input_line.begin(), input_line.end(),
                std::ostream_iterator<char>(file));
      input_line = "";
    };

    size_t max_num_measurements = 0;
    std::string input_line = "";
    for (const auto &timer : measurements) {
      input_line += timer.first + seperator;
      const size_t num_measurements = timer.second.size();
      max_num_measurements = std::max(num_measurements, max_num_measurements);
    }

    inputIntoFile(input_line);

    for (size_t m = 0; m < max_num_measurements; m++) {
      for (const auto &timer : measurements) {
        if (timer.second.size() > m) {
          const double val = timer.second[m].toDouble<T>();
          input_line += std::to_string(val) + seperator;
        } else {
          input_line += seperator;
        }
      }
      inputIntoFile(input_line);
    }

    return file.bad();
  }

  /*!
   * \brief Writes the histogramms of all measurements from all timers into the
   * given file (appends) for further analysis with Excel or Matlab. \template a
   * std::chrono duration in which the time (as double values) should be
   * printed. \param file_name The name of the file to write into. If its a
   * path, the path must exist. \param seperator A character to seperate the
   * input fields. \return true if writeing was successfull.
   */
  template <class T>
  bool histogramToFile(const std::string &file_name, char seperator) {

    const size_t num_timers = measurements.size();
    if (num_timers == 0) {
      return false;
    }

    std::vector<Result> results(num_timers);
    size_t i = 0;
    for (const auto &timer : measurements) {
      getResult(timer.first, results[i++]);
    }

    std::ofstream file;
    file.open(file_name.c_str(), std::ios_base::app);
    if (file.bad()) {
      return false;
    }

    std::string input_line = "";
    auto inputIntoFile = [&file, &input_line]() {
      input_line.pop_back();
      input_line += "\n";
      std::copy(input_line.begin(), input_line.end(),
                std::ostream_iterator<char>(file));
      input_line = "";
    };

    size_t max_num_buckets = 0;
    for (const auto &result : results) {
      input_line += result.timer_name + " bucket" + seperator +
                    result.timer_name + " count" + seperator;
      const size_t num_buckets = result.h.buckets.size();
      max_num_buckets = std::max(max_num_buckets, num_buckets);
    }
    inputIntoFile();

    for (size_t b = 0; b < max_num_buckets; b++) {
      for (const auto &result : results) {
        if (result.h.buckets.size() > b) {
          const Histogram::Bucket &bucket = result.h.buckets[b];
          const double val = bucket.getBucketCenter().toDouble<T>();
          const double normed_value =
              static_cast<double>(bucket.num) /
              static_cast<double>(result.number_measurements -
                                  result.number_outliners);
          input_line += std::to_string(val) + seperator +
                        std::to_string(normed_value) + seperator;
        } else {
          input_line += ' ' + seperator + ' ' + seperator;
        }
      }
      inputIntoFile();
    }

    return file.bad();
  }

private:
  PreciseTime findMedian(std::vector<PreciseTime> &measurements) noexcept {
    // https://www.geeksforgeeks.org/finding-median-of-unsorted-array-in-linear-time-using-c-stl/
    const size_t n = measurements.size();

    // If size of the arr[] is even
    if (n % 2 == 0) {

      // Applying nth_element
      // on n/2th index
      std::nth_element(measurements.begin(), measurements.begin() + n / 2,
                       measurements.end());

      // Applying nth_element
      // on (n-1)/2 th index
      std::nth_element(measurements.begin(), measurements.begin() + (n - 1) / 2,
                       measurements.end());

      // Find the average of value at
      // index N/2 and (N-1)/2
      return (measurements[(n - 1) / 2] + measurements[n / 2]) / 2.0;
    }

    // If size of the arr[] is odd
    else {

      // Applying nth_element
      // on n/2
      nth_element(measurements.begin(), measurements.begin() + n / 2,
                  measurements.end());

      // Value at index (N/2)th
      // is the median
      return measurements[n / 2];
    }
  }

  PreciseTime findMedianCopy(std::vector<PreciseTime> measurements) {
    return findMedian(measurements);
  }

  typedef std::conditional<std::chrono::high_resolution_clock::is_steady,
                           std::chrono::high_resolution_clock,
                           std::chrono::steady_clock>::type precisionClock;

  std::map<std::string, Timer::precisionClock::time_point> begin_measurements;
  typedef std::map<std::string, Timer::precisionClock::time_point>::iterator
      begin_measurements_it;
  std::map<std::string, std::vector<PreciseTime>> measurements;
};

/*!
 * \brief A Single timer without statistic support.
 */
class SingleTimer {
public:
  /*!
   * \brief Start one Simple Timer. No Statistics will be generated.
   */
  void start() {
    started = true;
    start_time = precisionClock::now();
  }

  /*!
   * \brief Reset the Timer.
   */
  void reset() { started = false; }

  /*!
   * \brief Return true if start() was called and reset() was not.
   */
  bool hasStarted() const { return started; }

  /*!
   * \brief Returns the time since start() was called. Timer continues to run.
   * \return The passed time in given template format: std::chrono:: time
   * duration
   */
  template <class T> T getPassedTime() const {
    const SingleTimer::precisionClock::time_point stop_time =
        precisionClock::now();
    if (!started) {
      return std::chrono::duration_cast<T>(stop_time - stop_time);
    }
    return std::chrono::duration_cast<T>(stop_time - start_time);
  }

private:
  typedef std::conditional<std::chrono::high_resolution_clock::is_steady,
                           std::chrono::high_resolution_clock,
                           std::chrono::steady_clock>::type precisionClock;

  SingleTimer::precisionClock::time_point start_time;
  bool started = false;
};

} // namespace tool

#endif
