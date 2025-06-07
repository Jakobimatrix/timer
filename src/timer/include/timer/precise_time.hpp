/*
 * \file: precise_time.hpp
 * \brief: Implements the PreciseTime class useing std::chrono.
 * \date: 11.05.2025
 * \author: Jakob Wandel
 * \source: https://github.com/Jakobimatrix/timer
 * \version: 3.0
 */

#ifndef PRECISE_TIME_H
#define PRECISE_TIME_H

#include <math.h>

#include <cassert>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <array>

/**
 * @brief Computes the fractional and integral parts of a floating-point number.
 * @param x The input floating-point number.
 * @param full Reference to store the integral part of the number.
 * @return The fractional part of the number.
 */
constexpr double fastModF(double x, double &full) noexcept {
  full = static_cast<double>(static_cast<int64_t>(x));
  return x - full;
}



/**
 * @brief Converts microseconds to nanoseconds.
 * @tparam t The type of the input value.
 * @param micro Time in microseconds.
 * @return Time in nanoseconds.
 */
template <class t>
constexpr auto us2ns(t micro) {
  constexpr t MICRO_TO_NANO = static_cast<t>(1000);
  return micro * MICRO_TO_NANO;
}

/**
 * @brief Converts milliseconds to nanoseconds.
 * @tparam t The type of the input value.
 * @param milli Time in milliseconds.
 * @return Time in nanoseconds.
 */
template <class t>
constexpr auto ms2ns(t milli) {
  constexpr t MILLI_TO_NANO = static_cast<t>(1000000);
  return milli * MILLI_TO_NANO;
}

/**
 * @brief Converts seconds to nanoseconds.
 * @tparam t The type of the input value.
 * @param sec Time in seconds.
 * @return Time in nanoseconds.
 */
template <class t>
constexpr auto s2ns(t sec) {
  constexpr t SEC_TO_NANO = static_cast<t>(1000000000LL);
  return sec * SEC_TO_NANO;
}

/**
 * @brief Converts nanoseconds to microseconds.
 * @tparam t The type of the input value.
 * @param ns Time in nanoseconds.
 * @return Time in microseconds.
 */
template <class t>
constexpr auto ns2us(t ns) {
  constexpr t NANO_TO_MICRO = static_cast<t>(1000);
  return ns / NANO_TO_MICRO;
}

/**
 * @brief Converts nanoseconds to milliseconds.
 * @tparam t The type of the input value.
 * @param ns Time in nanoseconds.
 * @return Time in milliseconds.
 */
template <class t>
constexpr auto ns2ms(t ns) {
  constexpr t NANO_TO_MILLI = static_cast<t>(1000000);
  return ns / NANO_TO_MILLI;
}

/**
 * @brief Converts nanoseconds to seconds.
 * @tparam t The type of the input value.
 * @param ns Time in nanoseconds.
 * @return Time in seconds.
 */
template <class t>
constexpr auto ns2s(t ns) {
  constexpr t NANO_TO_SEC = static_cast<t>(1000000000LL);
  return ns / NANO_TO_SEC;
}

/**
 * @brief Converts nanoseconds to minutes.
 * @tparam t The type of the input value.
 * @param ns Time in nanoseconds.
 * @return Time in minutes.
 */
template <class t>
constexpr auto ns2m(t ns) {
  constexpr t NANO_TO_MIN = static_cast<t>(1000000000LL * 60LL);
  return ns / NANO_TO_MIN;
}

/**
 * @brief Converts nanoseconds to hours.
 * @tparam t The type of the input value.
 * @param ns Time in nanoseconds.
 * @return Time in hours.
 */
template <class t>
constexpr auto ns2h(t ns) {
  constexpr t NANO_TO_HOUR = static_cast<t>(1000000000LL * 3600LL);
  return ns / NANO_TO_HOUR;
}

/**
 * @brief Converts minutes to seconds.
 * @tparam t The type of the input value.
 * @param m Time in minutes.
 * @return Time in seconds.
 */
template <class t>
constexpr auto m2s(t m) {
  constexpr t MIN_TO_SEC = static_cast<t>(60);
  return m * MIN_TO_SEC;
}

/**
 * @brief Converts hours to seconds.
 * @tparam t The type of the input value.
 * @param h Time in hours.
 * @return Time in seconds.
 */
template <class t>
constexpr auto h2s(t h) {
  constexpr t HOUR_TO_SEC = static_cast<t>(3600);
  return h * HOUR_TO_SEC;
}

/**
 * @brief Converts hours to minutes.
 * @tparam t The type of the input value.
 * @param h Time in hours.
 * @return Time in minutes.
 */
template <class t>
constexpr auto h2m(t h) {
  constexpr t HOUR_TO_MIN = static_cast<t>(60);
  return h * HOUR_TO_MIN;
}

/**
 * @brief Converts hours to milliseconds.
 * @tparam t The type of the input value.
 * @param h Time in hours.
 * @return Time in milliseconds.
 */
template <class t>
constexpr auto h2ms(t h) {
  constexpr t HOUR_TO_MILLI = static_cast<t>(3600LL * 1000LL);
  return h * HOUR_TO_MILLI;
}

/**
 * @brief Converts hours to microseconds.
 * @tparam t The type of the input value.
 * @param h Time in hours.
 * @return Time in microseconds.
 */
template <class t>
constexpr auto h2us(t h) {
  constexpr t HOUR_TO_MICRO = static_cast<t>(3600LL * 1000000LL);
  return h * HOUR_TO_MICRO;
}

/**
 * @brief Converts hours to nanoseconds.
 * @tparam t The type of the input value.
 * @param h Time in hours.
 * @return Time in nanoseconds.
 */
template <class t>
constexpr auto h2ns(t h) {
  constexpr t HOUR_TO_NANO = static_cast<t>(3600LL * 1000000000LL);
  return h * HOUR_TO_NANO;
}

/**
 * @brief Converts seconds to microseconds.
 * @tparam t The type of the input value.
 * @param s Time in seconds.
 * @return Time in microseconds.
 */
template <class t>
constexpr auto s2us(t s) {
  constexpr t SEC_TO_MICRO = static_cast<t>(1000000);
  return s * SEC_TO_MICRO;
}

/**
 * @brief Converts seconds to milliseconds.
 * @tparam t The type of the input value.
 * @param s Time in seconds.
 * @return Time in milliseconds.
 */
template <class t>
constexpr auto s2ms(t s) {
  constexpr t SEC_TO_MILLI = static_cast<t>(1000);
  return s * SEC_TO_MILLI;
}

/**
 * @brief Converts seconds to minutes.
 * @tparam t The type of the input value.
 * @param s Time in seconds.
 * @return Time in minutes.
 */
template <class t>
constexpr auto s2m(t s) {
  constexpr t SEC_TO_MIN = static_cast<t>(60);
  return s / SEC_TO_MIN;
}

/**
 * @brief Converts seconds to hours.
 * @tparam t The type of the input value.
 * @param s Time in seconds.
 * @return Time in hours.
 */
template <class t>
constexpr auto s2h(t s) {
  constexpr t SEC_TO_HOUR = static_cast<t>(3600);
  return s / SEC_TO_HOUR;
}

/**
 * @brief Converts seconds to hours (floored).
 * @tparam t The type of the input value.
 * @param s Time in seconds.
 * @return Time in hours (floored).
 */
template <class t>
constexpr auto s2hf(t s) {
  constexpr t SEC_TO_HOUR = static_cast<t>(3600);
  return std::floor(s / SEC_TO_HOUR);
}

/**
 * @brief Converts a time unit to its string representation.
 * @tparam T The chrono duration type (e.g., nanoseconds, microseconds).
 * @return The string representation of the time unit.
 */
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
  /**
   * @brief Default constructor for PreciseTime.
   */
  constexpr PreciseTime() = default;

  /**
   * @brief Constructs a PreciseTime object from nanoseconds.
   * @param ns Time in nanoseconds.
   */
  constexpr PreciseTime(const std::chrono::nanoseconds &ns) noexcept
      : nano_seconds(ns) {
    sanitize();
  }

  /**
   * @brief Constructs a PreciseTime object from microseconds.
   * @param us Time in microseconds.
   */
  constexpr PreciseTime(const std::chrono::microseconds &us) noexcept
      : nano_seconds(std::chrono::nanoseconds(us2ns(us.count()))) {
    sanitize();
  }

  /**
   * @brief Constructs a PreciseTime object from milliseconds.
   * @param ms Time in milliseconds.
   */
  constexpr PreciseTime(const std::chrono::milliseconds &ms) noexcept
      : nano_seconds(std::chrono::nanoseconds(ms2ns(ms.count()))) {
    sanitize();
  }

  /**
   * @brief Constructs a PreciseTime object from seconds.
   * @param s Time in seconds.
   */
  constexpr PreciseTime(const std::chrono::seconds &s) noexcept
      : seconds(s) {
    sanitize();
  }

  /**
   * @brief Constructs a PreciseTime object from minutes.
   * @param m Time in minutes.
   */
  constexpr PreciseTime(const std::chrono::minutes &m) noexcept
      : seconds(std::chrono::seconds(m2s(m.count()))) {
    sanitize();
  }

  /**
   * @brief Constructs a PreciseTime object from hours.
   * @param h Time in hours.
   */
  constexpr PreciseTime(const std::chrono::hours &h) : hours(h) {}

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

  constexpr PreciseTime(const PreciseTime &other) noexcept = default;

  /**
   * @brief Move constructor for PreciseTime.
   * @param other The other PreciseTime object to move.
   */
  constexpr PreciseTime(PreciseTime &&other) noexcept = default;

  /**
   * @brief Copy assignment operator for PreciseTime.
   * @param other The other PreciseTime object to copy.
   * @return Reference to the current object.
   */
  PreciseTime &operator=(const PreciseTime &other) noexcept = default;

  /**
   * @brief Move assignment operator for PreciseTime.
   * @param other The other PreciseTime object to move.
   * @return Reference to the current object.
   */
  PreciseTime &operator=(PreciseTime &&other) noexcept = default;

  /**
   * @brief Returns the greatest time the PreciseTime class can hold.
   * @tparam EXPO The exponent of the time unit.
   * @return The maximum PreciseTime object.
   */
  template <int EXPO = 1>
  static constexpr PreciseTime max() noexcept {
    PreciseTime ps(std::chrono::hours::max());
    ps.seconds = MAX_VALIDE_S;
    ps.nano_seconds = MAX_VALIDE_NS;
    ps.exponent = EXPO;
    return ps;
  }

  /**
   * @brief Returns the smallest time the PreciseTime class can hold.
   * @tparam EXPO The exponent of the time unit.
   * @return The minimum PreciseTime object.
   */
  template <int EXPO = 1>
  static constexpr PreciseTime min() noexcept {
    PreciseTime ps(std::chrono::hours::min());
    ps.seconds = MIN_VALIDE_S;
    ps.nano_seconds = MIN_VALIDE_NS;
    ps.exponent = EXPO;
    return ps;
  }

  /**
   * @brief Returns a PreciseTime object representing zero.
   * @tparam EXPO The exponent of the time unit.
   * @return A PreciseTime object with zero value.
   */
  template <int EXPO = 1>
  static constexpr PreciseTime zero() noexcept {
    PreciseTime ps;
    ps.exponent = EXPO;
    return ps;
  }

  /**
   * @brief Converts the PreciseTime to a double value in the specified unit.
   * @tparam c The chrono duration type std::chrono::nanoseconds.
   * @return The time as a double in the specified unit.
   */
  template <class c>
  constexpr typename std::enable_if<std::is_same<c, std::chrono::nanoseconds>::value, double>::type toDouble() const
      noexcept {
    return h2ns(static_cast<double>(hours.count())) +
           s2ns(static_cast<double>(seconds.count())) +
           static_cast<double>(nano_seconds.count()) + sub_nano_seconds;
  }

  /**
   * @brief Converts the PreciseTime to a double value in the specified unit.
   * @tparam c The chrono duration type std::chrono::microseconds.
   * @return The time as a double in the specified unit.
   */
  template <class c>
  constexpr typename std::enable_if<std::is_same<c, std::chrono::microseconds>::value, double>::type toDouble() const
      noexcept {
    return h2us(static_cast<double>(hours.count())) +
           s2us(static_cast<double>(seconds.count())) +
           ns2us(static_cast<double>(nano_seconds.count()) + sub_nano_seconds);
  }

  /**
   * @brief Converts the PreciseTime to a double value in the specified unit.
   * @tparam c The chrono duration type std::chrono::milliseconds.
   * @return The time as a double in the specified unit.
   */
  template <class c>
  constexpr typename std::enable_if<std::is_same<c, std::chrono::milliseconds>::value, double>::type toDouble() const
      noexcept {
    return h2ms(static_cast<double>(hours.count())) +
           s2ms(static_cast<double>(seconds.count())) +
           ns2ms(static_cast<double>(nano_seconds.count()) + sub_nano_seconds);
  }

  /**
   * @brief Converts the PreciseTime to a double value in the specified unit.
   * @tparam c The chrono duration type std::chrono::seconds>.
   * @return The time as a double in the specified unit.
   */
  template <class c>
  constexpr typename std::enable_if<std::is_same<c, std::chrono::seconds>::value, double>::type toDouble() const
      noexcept {
    return h2s(static_cast<double>(hours.count())) +
           static_cast<double>(seconds.count()) +
           ns2s(static_cast<double>(nano_seconds.count()) + sub_nano_seconds);
  }

  /**
   * @brief Converts the PreciseTime to a double value in the specified unit.
   * @tparam c The chrono duration type std::chrono::minutes.
   * @return The time as a double in the specified unit.
   */
  template <class c>
  constexpr typename std::enable_if<std::is_same<c, std::chrono::minutes>::value, double>::type toDouble() const
      noexcept {
    return h2m(static_cast<double>(hours.count())) +
           s2m(static_cast<double>(seconds.count())) +
           ns2m(static_cast<double>(nano_seconds.count()) + sub_nano_seconds);
  }

  /**
   * @brief Converts the PreciseTime to a double value in the specified unit.
   * @tparam c The chrono duration type std::chrono::hours.
   * @return The time as a double in the specified unit.
   */
  template <class c>
  constexpr typename std::enable_if<std::is_same<c, std::chrono::hours>::value, double>::type toDouble() const
      noexcept {
    return static_cast<double>(hours.count()) +
           s2h(static_cast<double>(seconds.count())) +
           ns2h(static_cast<double>(nano_seconds.count()) + sub_nano_seconds);
  }

  /**
   * @brief Converts the PreciseTime to the specified chrono duration type.
   * @tparam c The chrono duration type std::chrono::nanoseconds.
   * @return The time as the specified chrono duration type.
   */
  template <class c>
  constexpr typename std::enable_if<std::is_same<c, std::chrono::nanoseconds>::value, c>::type convert() const
      noexcept {
    return std::chrono::nanoseconds(h2ns(hours.count()) + s2ns(seconds.count()) +
                                    nano_seconds.count());
  }

  /**
   * @brief Converts the PreciseTime to the specified chrono duration type.
   * @tparam c The chrono duration type std::chrono::microseconds.
   * @return The time as the specified chrono duration type.
   */
  template <class c>
  constexpr typename std::enable_if<std::is_same<c, std::chrono::microseconds>::value, c>::type convert() const
      noexcept {
    return std::chrono::microseconds(h2us(hours.count()) + s2us(seconds.count()) +
                                     ns2us(nano_seconds.count()));
  }

  /**
   * @brief Converts the PreciseTime to the specified chrono duration type.
   * @tparam c The chrono duration type std::chrono::milliseconds.
   * @return The time as the specified chrono duration type.
   */
  template <class c>
  constexpr typename std::enable_if<std::is_same<c, std::chrono::milliseconds>::value, c>::type convert() const
      noexcept {
    return std::chrono::milliseconds(h2ms(hours.count()) + s2ms(seconds.count()) +
                                     ns2ms(nano_seconds.count()));
  }

  /**
   * @brief Converts the PreciseTime to the specified chrono duration type.
   * @tparam c The chrono duration type std::chrono::seconds.
   * @return The time as the specified chrono duration type.
   */
  template <class c>
  constexpr typename std::enable_if<std::is_same<c, std::chrono::seconds>::value, c>::type convert() const
      noexcept {
    return std::chrono::seconds(h2s(hours.count())) + seconds;
  }

  /**
   * @brief Converts the PreciseTime to the specified chrono duration type.
   * @tparam c The chrono duration type std::chrono::minutes.
   * @return The time as the specified chrono duration type.
   */
  template <class c>
  constexpr typename std::enable_if<std::is_same<c, std::chrono::minutes>::value, c>::type convert() const
      noexcept {
    return std::chrono::minutes(s2m(seconds.count() + h2m(hours.count())));
    ;
  }

  /**
   * @brief Converts the PreciseTime to the specified chrono duration type.
   * @tparam c The chrono duration type std::chrono::hours.
   * @return The time as the specified chrono duration type.
   */
  template <class c>
  constexpr typename std::enable_if<std::is_same<c, std::chrono::hours>::value, c>::type convert() const
      noexcept {
    return hours;
  }

  /**
   * @brief Returns the part of the time corresponding to the specified chrono duration type.
   * @tparam c The chrono duration type std::chrono::nanoseconds.
   * @return The part of the time as the specified chrono duration type.
   */
  template <class c>
  constexpr typename std::enable_if<std::is_same<c, std::chrono::nanoseconds>::value, c>::type get() const
      noexcept {
    const int64_t ns = nano_seconds.count();
    return std::chrono::nanoseconds(ns - (ns / 1000) * 1000);
  }

  /**
   * @brief Returns the part of the time corresponding to the specified chrono duration type.
   * @tparam c The chrono duration type std::chrono::microseconds.
   * @return The part of the time as the specified chrono duration type.
   */
  template <class c>
  constexpr typename std::enable_if<std::is_same<c, std::chrono::microseconds>::value, c>::type get() const
      noexcept {
    const int64_t us = nano_seconds.count() / 1000;
    return std::chrono::microseconds(us - (us / 1000) * 1000);
  }

  /**
   * @brief Returns the part of the time corresponding to the specified chrono duration type.
   * @tparam c The chrono duration type std::chrono::milliseconds.
   * @return The part of the time as the specified chrono duration type.
   */
  template <class c>
  constexpr typename std::enable_if<std::is_same<c, std::chrono::milliseconds>::value, c>::type get() const
      noexcept {
    const int64_t ms = nano_seconds.count() / 1000000;
    return std::chrono::milliseconds(ms - (ms / 1000) * 1000);
  }

  /**
   * @brief Returns the part of the time corresponding to the specified chrono duration type.
   * @tparam c The chrono duration type std::chrono::seconds.
   * @return The part of the time as the specified chrono duration type.
   */
  template <class c>
  constexpr typename std::enable_if<std::is_same<c, std::chrono::seconds>::value, c>::type get() const
      noexcept {
    const int64_t s = seconds.count();
    return std::chrono::seconds(s - (s / 60) * 60);
  }

  /**
   * @brief Returns the part of the time corresponding to the specified chrono duration type.
   * @tparam c The chrono duration type std::chrono::minutes.
   * @return The part of the time as the specified chrono duration type.
   */
  template <class c>
  constexpr typename std::enable_if<std::is_same<c, std::chrono::minutes>::value, c>::type get() const
      noexcept {
    const int64_t m = seconds.count() / 60;
    return std::chrono::minutes(m - (m / 60) * 60);
  }

  /**
   * @brief Returns the part of the time corresponding to the specified chrono duration type.
   * @tparam c The chrono duration type std::chrono::hours.
   * @return The part of the time as the specified chrono duration type.
   */
  template <class c>
  constexpr typename std::enable_if<std::is_same<c, std::chrono::hours>::value, c>::type get() const
      noexcept {
    return hours;
  }

  /**
   * @brief Sets the time in nanoseconds.
   * @param ns Time in nanoseconds.
   */
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

  /**
   * @brief Sets the time in seconds.
   * @param s Time in seconds.
   */
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

  /**
   * @brief Sets the time in hours.
   * @param h Time in hours.
   */
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

  /**
   * @brief Returns the exponent of the PreciseTime.
   * @return The exponent.
   */
  constexpr int getExponent() const noexcept { return exponent; }

  /**
   * @brief Sets the exponent of the PreciseTime.
   * @param exp The exponent to set.
   */
  constexpr void setExponent(int exp) noexcept {
    this->exponent = exp;
  }

 private:
  /**
   * @brief If the internal counter for the nano seconds get over
   * 999999999 we adjust the nanoseconds by adding to the internal seconds.
   */
  constexpr void sanitizeNS() noexcept {
    if (nano_seconds > MAX_VALIDE_NS || nano_seconds < MIN_VALIDE_NS) {
      double seconds_from_nanoseconds = static_cast<double>(ns2s(nano_seconds.count()));
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

  /**
   * @brief Adjusts the sub-nanoseconds to ensure they are within valid range.
   */
  constexpr void sanitizeSubNs() noexcept {
    const double ns = static_cast<double>(static_cast<int64_t>(sub_nano_seconds));
    nano_seconds += std::chrono::nanoseconds(static_cast<int64_t>(ns));
    sub_nano_seconds -= ns;
  }

  /**
   * @brief If the internal counter for the seconds 3599
   * we adjust the seconds by adding to the internal hours.
   */
  constexpr void sanitizeS() noexcept {
    /// Sanitize seconds
    if (seconds > MAX_VALIDE_S || seconds < MIN_VALIDE_S) {
      double hours_from_seconds = static_cast<double>(s2h(seconds.count()));
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

  /**
   * @brief Sometimes when substracting we end up with different
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

  /**
   * @brief Combines all sanitize functions.
   */
  constexpr void sanitize() noexcept {
    sanitizeSubNs();
    sanitizeNS();
    sanitizeS();
    sanitizeSign();
  }

  /**
   * @brief Checks if a operation (+-* /) did cause an over
   * or underflow. If it happened THIS is set to max() or min();
   * @param was_positive_before_operation Set to true if this was positive
   * before the operation.
   * @param expected_higher Set to True if you expect this (time) to be higher
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

  /**
   * @brief Checks for overflow or underflow based on the given hours value.
   * @param hours_d The hours value to check.
   * @return True if overflow or underflow occurred, false otherwise.
   */
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

  /**
   * @brief Determines the type of overflow or underflow based on the given hours value.
   * @param hours_d The hours value to check.
   * @return 1 for overflow, -1 for underflow, 0 for no overflow or underflow.
   */
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
  /**
   * @brief Checks if the PreciseTime is positive.
   * @return True if positive, false otherwise.
   */
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

  /**
   * @brief Checks if the given PreciseTime is positive.
   * @param pt The PreciseTime object to check.
   * @return True if positive, false otherwise.
   */
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

  /**
   * @brief Adds another PreciseTime to the current object.
   * @param pt The other PreciseTime object to add.
   */
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

  /**
   * @brief Adds another PreciseTime to the current object.
   * @param pt The other PreciseTime object to add.
   * @return The resulting PreciseTime object.
   */
  constexpr PreciseTime operator+(const PreciseTime &pt) const noexcept {
    PreciseTime ret(*this);
    ret += pt;
    return ret;
  }

  /**
   * @brief Subtracts another PreciseTime from the current object.
   * @param pt The other PreciseTime object to subtract.
   */
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

  /**
   * @brief Subtracts another PreciseTime from the current object.
   * @param pt The other PreciseTime object to subtract.
   * @return The resulting PreciseTime object.
   */
  constexpr PreciseTime operator-(const PreciseTime &pt) const noexcept {
    PreciseTime ret(*this);
    ret -= pt;
    return ret;
  }

  /**
   * @brief Multiplies the current PreciseTime by a scalar.
   * @param multi The scalar multiplier.
   */
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

  /**
   * @brief Multiplies the current PreciseTime by a scalar.
   * @param multi The scalar multiplier.
   * @return The resulting PreciseTime object.
   */
  constexpr PreciseTime operator*(const double multi) const noexcept {
    PreciseTime ret(*this);
    ret *= multi;
    return ret;
  }

  /**
   * @brief Multiplies the current PreciseTime by another PreciseTime.
   * @param pt The other PreciseTime object to multiply.
   * @return The resulting PreciseTime object.
   */
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

  /**
   * @brief Divides the current PreciseTime by a scalar.
   * @param div The scalar divisor.
   * @return The resulting PreciseTime object.
   */
  constexpr PreciseTime operator/(const double div) const noexcept {
    PreciseTime ret(*this);
    ret *= (1. / div);
    return ret;
  }

  /**
   * @brief Divides the current PreciseTime by a scalar.
   * @param div The scalar divisor.
   */
  constexpr void operator/=(const double div) {
    (*this) *= (1.0 / div);  // this is not as precise
    return;
  }

  /**
   * @brief Divides the current PreciseTime by another PreciseTime.
   * @param pt The other PreciseTime object to divide.
   * @return The resulting PreciseTime object.
   */
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

  /**
   * @brief Returns an array of time components separated into units.
   * @return An array containing time components: nanoseconds, microseconds, milliseconds, seconds, minutes, hours.
   */
  constexpr std::array<int64_t, 6> getSeperatedTimeComponents() const noexcept {
    return {get<std::chrono::nanoseconds>().count(),
            get<std::chrono::microseconds>().count(),
            get<std::chrono::milliseconds>().count(),
            get<std::chrono::seconds>().count(),
            get<std::chrono::minutes>().count(),
            get<std::chrono::hours>().count()};
  }

  /**
   * @brief Checks if the PreciseTime has rolled over due to overflow or underflow.
   * @return True if rolled over, false otherwise.
   */
  constexpr bool hasRolledOver() { return has_rolled_over; }

  /**
   * @brief Computes the square root of the given PreciseTime.
   * @param pt The PreciseTime object to compute the square root of.
   * @return The square root of the given PreciseTime.
   */
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

  /**
   * @brief Computes the square root of the current PreciseTime.
   */
  void sqrt() noexcept { PreciseTime::sqrt(*this); }

  /**
   * @brief Returns a new PreciseTime object representing the square root of the current object.
   * @return The square root of the current PreciseTime.
   */
  PreciseTime getSqrt() const noexcept {
    PreciseTime ret = *this;
    PreciseTime::sqrt(ret);
    return ret;
  }

  constexpr bool operator==(const PreciseTime &pt) const noexcept {
    return pt.exponent == exponent && pt.nano_seconds == nano_seconds &&
           pt.seconds == seconds && pt.hours == hours;
  }

  constexpr bool operator!=(const PreciseTime &pt) const noexcept {
    return !(*this == pt);
    return (hours == pt.hours && seconds == pt.seconds &&
            (nano_seconds != pt.nano_seconds) && (nano_seconds < pt.nano_seconds)) ||
           (hours == pt.hours && seconds != pt.seconds && seconds < pt.seconds) ||
           ((hours != pt.hours) && (hours < pt.hours));
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
    return (hours == pt.hours && seconds == pt.seconds &&
            nano_seconds != pt.nano_seconds && nano_seconds < pt.nano_seconds) ||
           (hours == pt.hours && seconds != pt.seconds && seconds < pt.seconds) ||
           (hours != pt.hours && hours < pt.hours);
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


  /**
   * @brief Prints the PreciseTime in a clean format.
   * {h: [xxxx]   m: [xx]   s: [xxx]   ms: [xxx]   us: [xxx]  ns:
   * [xxx]}^exponent
   * @param os The output stream.
   * @param pt The PreciseTime object to print.
   * @return The output stream.
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

  /**
   * @brief Converts the PreciseTime to a string representation.
   * @return The string representation of the PreciseTime.
   */
  std::string toString() const noexcept {
    std::stringstream ss;
    ss << *this;
    return ss.str();
  }

  /**
   * @brief Returns the most significant time unit of the PreciseTime.
   * @return A PreciseTime object representing the most significant time unit.
   */
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

  /**
   * @brief Returns the most significant time unit as a string.
   * @return A string representing the most significant time unit.
   */
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


  /**
   * @brief Returns the time in its highest format as a string. E.g.:
   * 44s,40ms,66us,12ns.getTimeString(4) --> "44.04s"
   * 66us,12ns.getTimeString(4) --> "66.02us"
   * @param precision The number of decimal places to include.
   * @return A string representing the time in its highest format.
   */
  std::string getTimeString(int precision) const noexcept {
    double time_d = toDouble<std::chrono::nanoseconds>();
    std::stringstream stream;

    std::string unit = "ns";

    if (get<std::chrono::hours>().count() > 0) {
      time_d = ns2h(time_d);
      unit = "h";
    } else if (get<std::chrono::minutes>().count() > 0) {
      time_d = ns2m(time_d);
      unit = "m";
    } else if (get<std::chrono::seconds>().count() > 0) {
      time_d = ns2s(time_d);
      unit = "s";
    } else if (get<std::chrono::milliseconds>().count() > 0) {
      time_d = ns2ms(time_d);
      unit = "ms";
    } else if (get<std::chrono::microseconds>().count() > 0) {
      time_d = ns2us(time_d);
      unit = "us";
    }

    stream << std::fixed << std::setprecision(precision) << time_d << unit;

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
