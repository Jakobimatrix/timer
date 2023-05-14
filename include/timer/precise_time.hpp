/*
 * \file: precise_time.hpp
 * \brief: Implements the PreciseTime class useing std::chrono.
 * \date: 17.07.2021
 * \author: Jakob Wandel
 * \source: https://github.com/Jakobimatrix/timer
 * \version: 2.0
 */

#ifndef PRECISE_TIME_H
#define PRECISE_TIME_H

#include <math.h>

#include <cassert>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>

constexpr double fastModF(double x, double &full) noexcept {
  full = static_cast<double>(static_cast<int64_t>(x));
  return x - full;
}

/*
 * \brief x2y(t x)
 * \param x time in x
 * \return time in y
 */
template <class t>
constexpr auto mu2ns(t micro) {
  return static_cast<t>(micro * 1000.);
}

template <class t>
constexpr auto ms2ns(t milli) {
  return static_cast<t>(milli * 1000000.);
}

template <class t>
constexpr auto s2ns(t sec) {
  return static_cast<t>(sec * 1000000000.);
}

template <class t>
constexpr auto ns2us(t ns) {
  return static_cast<t>(ns / 1000.);
}

template <class t>
constexpr auto us2ns(t us) {
  return static_cast<t>(us * 1000.);
}

template <class t>
constexpr auto ns2ms(t ns) {
  return static_cast<t>(ns / 1000000.);
}

template <class t>
constexpr auto ns2s(t ns) {
  return static_cast<t>(ns / 1000000000.);
}

template <class t>
constexpr auto ns2m(t ns) {
  return static_cast<t>(ns / (1000000000. * 60.));
}

template <class t>
constexpr auto ns2h(t ns) {
  return static_cast<t>(ns / (1000000000. * 3600.));
}

template <class t>
constexpr auto m2s(t m) {
  return static_cast<t>(m * 60.);
}

template <class t>
constexpr auto h2s(t h) {
  return static_cast<t>(h * 3600.);
}

template <class t>
constexpr auto h2m(t h) {
  return static_cast<t>(h * 60.);
}

template <class t>
constexpr auto h2ms(t h) {
  return static_cast<t>(h * 3600. * 1000.);
}

template <class t>
constexpr auto h2us(t h) {
  return static_cast<t>(h * 3600. * 1000000.);
}

template <class t>
constexpr auto h2ns(t h) {
  return static_cast<t>(h * 3600. * 1000000000.);
}

template <class t>
constexpr auto s2us(t s) {
  return static_cast<t>(s * 1000000.);
}

template <class t>
constexpr auto s2ms(t s) {
  return static_cast<t>(s * 1000.);
}

template <class t>
constexpr auto s2m(t s) {
  return static_cast<t>(s / 60.);
}

template <class t>
constexpr auto s2h(t s) {
  return static_cast<t>(s / 3600.);
}

template <class t>
constexpr auto s2hf(t s) {
  return static_cast<t>(std::floor(s / 3600.));
}

template <class T>
constexpr auto timeunit2String() {
  if constexpr (std::is_same<T, std::chrono::nanoseconds>::value) {
    return "ns";
  } else if constexpr (std::is_same<T, std::chrono::microseconds>::value) {
    return "us";
  } else if constexpr (std::is_same<T, std::chrono::milliseconds>::value) {
    return "ms";
  } else if constexpr (std::is_same<T, std::chrono::seconds>::value) {
    return "s";
  } else if constexpr (std::is_same<T, std::chrono::minutes>::value) {
    return "m";
  } else if constexpr (std::is_same<T, std::chrono::hours>::value) {
    return "h";
  } else {
    return "?";
  }
}

class PreciseTime {
 public:
  typedef std::conditional<std::chrono::high_resolution_clock::is_steady,
                           std::chrono::high_resolution_clock,
                           std::chrono::steady_clock>::type PrecisionClock;
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
  template <int EXPO = 1>
  static constexpr PreciseTime max() noexcept {
    PreciseTime ps(std::chrono::hours::max());
    ps.seconds = MAX_VALIDE_S;
    ps.nano_seconds = MAX_VALIDE_NS;
    ps.exponent = EXPO;
    return ps;
  }

  /*!
   * \brief min Returns the smallest time the PreciseTime class can hold
   */
  template <int EXPO = 1>
  static constexpr PreciseTime min() noexcept {
    PreciseTime ps(std::chrono::hours::min());
    ps.seconds = MIN_VALIDE_S;
    ps.nano_seconds = MIN_VALIDE_NS;
    ps.exponent = EXPO;
    return ps;
  }

  /*!
   * \brief zero Returns a PreciseTime of zero
   */
  template <int EXPO = 1>
  static constexpr PreciseTime zero() noexcept {
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
  constexpr typename std::enable_if<std::is_same<c, std::chrono::nanoseconds>::value, double>::type toDouble() const
      noexcept {
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
  constexpr typename std::enable_if<std::is_same<c, std::chrono::microseconds>::value, double>::type toDouble() const
      noexcept {
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
  constexpr typename std::enable_if<std::is_same<c, std::chrono::milliseconds>::value, double>::type toDouble() const
      noexcept {
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
  constexpr typename std::enable_if<std::is_same<c, std::chrono::seconds>::value, double>::type toDouble() const
      noexcept {
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
  constexpr typename std::enable_if<std::is_same<c, std::chrono::minutes>::value, double>::type toDouble() const
      noexcept {
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
  constexpr typename std::enable_if<std::is_same<c, std::chrono::hours>::value, double>::type toDouble() const
      noexcept {
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
  constexpr typename std::enable_if<std::is_same<c, std::chrono::nanoseconds>::value, c>::type convert() const
      noexcept {
    return std::chrono::nanoseconds(h2ns(hours.count()) + s2ns(seconds.count()) +
                                    nano_seconds.count());
  }

  /*!
   * \brief convert() Converts the PrecisionTime to the type given in the
   * template.
   * \return if preciseTime = 2h,3m,44s,40ms,66us,12ns > convert<microseconds>
   * will return ((3600*2h + 60*3m + 44s)*1000 + 40ms)*1000 + 66us =
   * 7364040066us
   */
  template <class c>
  constexpr typename std::enable_if<std::is_same<c, std::chrono::microseconds>::value, c>::type convert() const
      noexcept {
    return std::chrono::microseconds(h2us(hours.count()) + s2us(seconds.count()) +
                                     ns2us(nano_seconds.count()));
  }

  /*!
   * \brief convert() Converts the PrecisionTime to the type given in the
   * template.
   * \return if preciseTime = 2h,3m,44s,40ms,66us,12ns > convert<milliseconds>
   * will return (3600*2h + 60*3m + 44s)*1000 + 40ms= 7364040ms
   */
  template <class c>
  constexpr typename std::enable_if<std::is_same<c, std::chrono::milliseconds>::value, c>::type convert() const
      noexcept {
    return std::chrono::milliseconds(h2ms(hours.count()) + s2ms(seconds.count()) +
                                     ns2ms(nano_seconds.count()));
  }

  /*!
   * \brief convert() Converts the PrecisionTime to the type given in the
   * template.
   * \return if preciseTime = 2h,3m,44s,40ms,66us,12ns > convert<seconds> will
   * return 3600*2h + 60*3m + 44s= 7364s
   */
  template <class c>
  constexpr typename std::enable_if<std::is_same<c, std::chrono::seconds>::value, c>::type convert() const
      noexcept {
    return std::chrono::seconds(h2s(hours.count())) + seconds;
  }

  /*!
   * \brief convert() Converts the PrecisionTime to the type given in the
   * template.
   * \return if preciseTime = 2h,3m,44s,40ms,66us,12ns > convert<minutes> will
   * return 60*2h + 3m = 123m
   */
  template <class c>
  constexpr typename std::enable_if<std::is_same<c, std::chrono::minutes>::value, c>::type convert() const
      noexcept {
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
  constexpr typename std::enable_if<std::is_same<c, std::chrono::hours>::value, c>::type convert() const
      noexcept {
    return hours;
  }

  /*!
   * \brief get() Returns the part of the time of which the type was given.
   * \return if preciseTime = 2h,3m,44s,40ms,66us,12ns > get<nanoseconds> will
   * return 12ns
   */
  template <class c>
  constexpr typename std::enable_if<std::is_same<c, std::chrono::nanoseconds>::value, c>::type get() const
      noexcept {
    const int64_t ns = nano_seconds.count();
    return std::chrono::nanoseconds(ns - (ns / 1000) * 1000);
  }

  /*!
   * \brief get() Returns the part of the time of which the type was given.
   * \return if preciseTime = 2h,3m,44s,40ms,66us,12ns > get<microseconds> will
   * return 66us
   */
  template <class c>
  constexpr typename std::enable_if<std::is_same<c, std::chrono::microseconds>::value, c>::type get() const
      noexcept {
    const int64_t us = nano_seconds.count() / 1000;
    return std::chrono::microseconds(us - (us / 1000) * 1000);
  }

  /*!
   * \brief get() Returns the part of the time of which the type was given.
   * \return if preciseTime = 2h,3m,44s,40ms,66us,12ns > get<milliseconds> will
   * return 40ms
   */
  template <class c>
  constexpr typename std::enable_if<std::is_same<c, std::chrono::milliseconds>::value, c>::type get() const
      noexcept {
    const int64_t ms = nano_seconds.count() / 1000000;
    return std::chrono::milliseconds(ms - (ms / 1000) * 1000);
  }

  /*!
   * \brief get() Returns the part of the time of which the type was given.
   * \return if preciseTime = 2h,3m,44s,40ms,66us,12ns > get<seconds> will
   * return 44s
   */
  template <class c>
  constexpr typename std::enable_if<std::is_same<c, std::chrono::seconds>::value, c>::type get() const
      noexcept {
    const int64_t s = seconds.count();
    return std::chrono::seconds(s - (s / 60) * 60);
  }

  /*!
   * \brief get() Returns the part of the time of which the type was given.
   * \return if preciseTime = 2h,3m,44s,40ms,66us,12ns > get<minutes> will
   * return 3m
   */
  template <class c>
  constexpr typename std::enable_if<std::is_same<c, std::chrono::minutes>::value, c>::type get() const
      noexcept {
    const int64_t m = seconds.count() / 60;
    return std::chrono::minutes(m - (m / 60) * 60);
  }

  /*!
   * \brief get Returns the part of the time of which the type was given.
   * \return if preciseTime = 2h,3m,44s,40ms,66us,12ns > get<hours> will
   * return 2h
   */
  template <class c>
  constexpr typename std::enable_if<std::is_same<c, std::chrono::hours>::value, c>::type get() const
      noexcept {
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
      nano_seconds =
          std::chrono::nanoseconds(nano_seconds.count() - s2ns(carry_seconds_l));
      // add them to the seconds
      seconds += std::chrono::seconds(carry_seconds_l);
    }
  }

  constexpr void sanitizeSubNs() noexcept {
    const double ns = static_cast<double>(static_cast<int64_t>(sub_nano_seconds));
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
    constexpr double max_h = static_cast<double>(std::chrono::hours::max().count());
    constexpr double min_h = static_cast<double>(std::chrono::hours::min().count());

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

    const double hours_d =
        static_cast<double>(hours.count()) + static_cast<double>(pt.hours.count());
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

    const double hours_d =
        static_cast<double>(hours.count()) - static_cast<double>(pt.hours.count());
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
        static_cast<double>(nano_seconds.count()) * multi + sub_nano_seconds * multi;

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
    (*this) *= (1.0 / div);  // this is not as precise
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
    /*
    bool HE = hours == pt.hours;
    bool SE = seconds == pt.seconds;
    bool NSE = nano_seconds == pt.nano_seconds;

    if (HE && SE && NSE) {
      return false;
    }
    if (HE && SE) {
      return nano_seconds < pt.nano_seconds;
    }
    if (HE) {
      return seconds < pt.seconds;
    }
    return hours < pt.hours;
    */
    return hours == pt.hours && seconds == pt.seconds &&
               nano_seconds != pt.nano_seconds && nano_seconds < pt.nano_seconds ||
           hours == pt.hours && seconds != pt.seconds && seconds < pt.seconds ||
           hours != pt.hours && hours < pt.hours;
  }

  constexpr bool operator>(const PreciseTime &pt) const noexcept {
    return !(*this < pt) && *this != pt;
  }

  constexpr bool operator<=(const PreciseTime &pt) const noexcept {
    return !(*this > pt);
  }

  constexpr bool operator>=(const PreciseTime &pt) const noexcept {
    return !(*this < pt);
  }
  ///
  ///
  /// </Comparisons>

  /*!
   * \brief Implements a clean print for the current time:
   * {h: [xxxx]   m: [xx]   s: [xxx]   ms: [xxx]   us: [xxx]  ns:
   * [xxx]}^exponent
   */
  friend std::ostream &operator<<(std::ostream &os, const PreciseTime &pt) noexcept {
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
    stream << std::fixed << std::setprecision(num_decimal_places) << time_d << unit;

    return stream.str();
  }

  static constexpr std::chrono::seconds MAX_VALIDE_S = std::chrono::seconds(3599);
  static constexpr std::chrono::seconds MIN_VALIDE_S = std::chrono::seconds(-3599);
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

#endif
