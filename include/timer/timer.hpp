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

#include <cassert>
#include <chrono>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace tool {

constexpr double fastModF(double x, double &full) noexcept {
  full = static_cast<double>(static_cast<long long>(x));
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

class PreciseTime {
 public:
  /// <Construction Area>
  ///
  ///
  constexpr PreciseTime() = default;

  constexpr explicit PreciseTime(const std::chrono::nanoseconds &nano_seconds) noexcept
      : nano_seconds(nano_seconds) {
    sanitize();
  }

  constexpr explicit PreciseTime(const std::chrono::microseconds &micro_seconds) noexcept
      : nano_seconds(std::chrono::nanoseconds(mu2ns(micro_seconds.count()))) {
    sanitize();
  }

  constexpr explicit PreciseTime(const std::chrono::milliseconds &milli_seconds) noexcept
      : nano_seconds(std::chrono::nanoseconds(ms2ns(milli_seconds.count()))) {
    sanitize();
  }

  constexpr explicit PreciseTime(const std::chrono::seconds &seconds) noexcept
      : seconds(seconds) {
    sanitize();
  }

  constexpr explicit PreciseTime(const std::chrono::minutes &minutes) noexcept
      : seconds(std::chrono::seconds(m2s(minutes.count()))) {
    sanitize();
  }

  constexpr explicit PreciseTime(const std::chrono::hours &hours)
      : hours(hours) {}

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
    const long ns = nano_seconds.count();
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
    const long us = nano_seconds.count() / 1000;
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
    const long ms = nano_seconds.count() / 1000000;
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
    const long s = seconds.count();
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
    const long m = seconds.count() / 60;
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
      const long carry_seconds_l = static_cast<long>(carry_seconds);

      // substract the nanoseconds which made up a second
      nano_seconds =
          std::chrono::nanoseconds(nano_seconds.count() - s2ns(carry_seconds_l));
      // add them to the seconds
      seconds += std::chrono::seconds(carry_seconds_l);
    }
  }


  constexpr void sanitizeSubNs() noexcept {
    const double ns = static_cast<double>(static_cast<int>(sub_nano_seconds));
    nano_seconds += std::chrono::nanoseconds(static_cast<int>(ns));
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
      const long carry_hours_l = static_cast<long>(carry_hours);

      // substract the seconds which made up a hour
      seconds = std::chrono::seconds(seconds.count() - h2s(carry_hours_l));
      // add them to the seconds
      hours += std::chrono::hours(carry_hours_l);
    }
  }

  /*!
   * \brief sanitizeSign Sometimes when substracting we end up with different
   * signs: {10s} - {10ns} will lead to a internal value of {+10s and -10ns}. We
   * have to adjust this to {9s and 999999990ns}.
   */
  constexpr void sanitizeSign() noexcept {
    auto pos = [](long probe) -> bool { return probe >= 0; };
    auto neg = [](long probe) -> bool { return probe <= 0; };
    auto sign = [this, &neg, &pos](long probe) {
      if (hours.count() < 0) {
        return neg(probe);
      } else if (hours.count() > 0) {
        return pos(probe);
      } else {
        if (seconds.count() < 0) {
          return neg(probe);
        } else if (seconds.count() > 0) {
          return pos(probe);
        } else {
          return true;  // we dont need to worry about different signs
        }
      }
    };


    // fix ns if necessary
    if (!sign(nano_seconds.count())) {
      long fixed_ns = s2ns(seconds.count()) + nano_seconds.count();
      nano_seconds = std::chrono::nanoseconds(fixed_ns);
      seconds = std::chrono::seconds(0);
      sanitize();
      return;
    }

    // fix s if necessary
    if (!sign(seconds.count())) {
      long fixed_s = h2s(hours.count()) + seconds.count();
      seconds = std::chrono::seconds(fixed_s);
      hours = std::chrono::hours(0);
      sanitize();
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
   * \brief sanitize the sub nanosecond which keep track of time smaller than ns.
   */
  constexpr void setSubNanoseconds(double ns) noexcept {
    const int full_ns = static_cast<long>(ns);
    const double full_nanoseconds = static_cast<double>(full_ns);
    sub_nano_seconds += ns - full_nanoseconds;
  }

 public:
  /// <Arithmetics>
  ///
  ///

  /*!
   * \brief OverflowProtection checks if a given time in hour presentation would over or underflow.
   * If an over or underflow operation would occur, it sets THIS to max() or min();
   * \return true if the operation does not over or underflow, false otherwise.
   */
  constexpr bool overflowProtection(double hours) noexcept {
    constexpr auto max_time = max().toDouble<std::chrono::hours>();
    constexpr auto min_time = min().toDouble<std::chrono::hours>();
    // overflow /underflow protection protection
    if (hours > max_time) {
      *this = max();
      return false;
    } else if (hours < min_time) {
      *this = min();
      return false;
    }
    return true;
  }

  constexpr void operator+=(const PreciseTime &pt) noexcept {
    assert(pt.exponent == exponent &&
           "You can not add different units like s + s^2");
    sub_nano_seconds += pt.sub_nano_seconds;
    nano_seconds += pt.nano_seconds;
    seconds += pt.seconds;
    hours += pt.hours;

    const double hours_d = toDouble<std::chrono::hours>();
    if (!overflowProtection(hours_d)) {
      return;
    }

    sanitize();
  }

  constexpr PreciseTime operator+(const PreciseTime &pt) const noexcept {
    PreciseTime ret(*this);
    ret += pt;
    return ret;
  }

  constexpr void operator-=(const PreciseTime &pt) noexcept {
    assert(pt.exponent == exponent &&
           "You can not substartc different units like s - s^2");
    sub_nano_seconds -= pt.sub_nano_seconds;
    nano_seconds -= pt.nano_seconds;
    seconds -= pt.seconds;
    hours -= pt.hours;

    const double hours_d = toDouble<std::chrono::hours>();
    if (!overflowProtection(hours_d)) {
      return;
    }

    sanitize();
  }

  constexpr PreciseTime operator-(const PreciseTime &pt) const noexcept {
    PreciseTime ret(*this);
    ret -= pt;
    return ret;
  }

  constexpr void operator*=(const double multi) noexcept {
    // overflow protection
    const double d_hours = toDouble<std::chrono::hours>() * multi;
    if (!overflowProtection(d_hours)) {
      return;
    }
    const double hours_ = hours.count() * multi;
    double remaining_hours = 0;
    const double seconds_rest = h2s(fastModF(hours_, remaining_hours));

    const double seconds_ = seconds.count() * multi + seconds_rest;
    double remaining_seconds = 0;
    const double nanoseconds_rest = s2ns(fastModF(seconds_, remaining_seconds));

    const double nanoseconds_ = nano_seconds.count() * multi + nanoseconds_rest;
    const double full_nanoseconds = static_cast<double>(static_cast<int>(nanoseconds_));
    const double subnanoseconds_rest = nanoseconds_ - full_nanoseconds;

    sub_nano_seconds = sub_nano_seconds * multi + subnanoseconds_rest;
    nano_seconds = std::chrono::nanoseconds(static_cast<long>(nanoseconds_));
    seconds = std::chrono::seconds(static_cast<long>(remaining_seconds));
    hours = std::chrono::hours(static_cast<long>(remaining_hours));

    sanitize();
  }

  constexpr PreciseTime operator*(const double multi) const noexcept {
    PreciseTime ret(*this);
    ret *= multi;
    return ret;
  }


  constexpr PreciseTime operator*(const PreciseTime &pt) const noexcept {
    const double resulting_s =
        pt.toDouble<std::chrono::seconds>() * toDouble<std::chrono::seconds>();

    const double ns = s2ns(resulting_s);

    PreciseTime ret(std::chrono::nanoseconds(static_cast<long>(ns)));
    ret.setSubNanoseconds(ns);
    ret.exponent = pt.exponent + exponent;
    return ret;
  }

  constexpr PreciseTime operator/(const double div) const noexcept {
    return *this * (1. / div);
  }

  constexpr void operator/=(const double div) { *this *= (1. / div); }

  constexpr PreciseTime operator/(const PreciseTime &pt) const noexcept {
    const double resulting_s =
        pt.toDouble<std::chrono::seconds>() / toDouble<std::chrono::seconds>();

    const double ns = s2ns(resulting_s);
    PreciseTime ret(std::chrono::nanoseconds(static_cast<long>(ns)));
    ret.setSubNanoseconds(ns);
    ret.exponent = exponent - pt.exponent;
    return ret;
  }


  static PreciseTime sqrt(PreciseTime &pt) noexcept {
    assert(pt.exponent % 2 == 0 &&
           "squareroot of Precise time with odd exponent not supported.");

    const double ns = s2ns(std::sqrt(pt.toDouble<std::chrono::seconds>()));
    const int exp = pt.exponent;
    pt = zero();
    pt.exponent = exp;
    pt.setSubNanoseconds(ns);
    pt.nano_seconds = std::chrono::nanoseconds(static_cast<long>(ns));
    pt.sanitize();

    pt.exponent /= 2;

    return pt;
  }

  void sqrt() noexcept { PreciseTime::sqrt(*this); }

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
  friend std::ostream &operator<<(std::ostream &os, const PreciseTime &pt) noexcept {
    auto blanks = [](long num) {
      const long i = std::abs(num);
      if (i < 10) {
        return "  ";
      } else if (i < 100) {
        return " ";
      }
      return "";
    };

    const long hours_ = pt.get<std::chrono::hours>().count();
    const long minutes_ = pt.get<std::chrono::minutes>().count();
    const long seconds_ = pt.get<std::chrono::seconds>().count();
    const long ms = pt.get<std::chrono::milliseconds>().count();
    const long us = pt.get<std::chrono::microseconds>().count();
    const long ns = pt.get<std::chrono::nanoseconds>().count();
    const std::string exp = std::to_string(pt.exponent);

    // clang-format off
    os << "{h: ["  << hours_   << "] " << blanks(hours_)
       << "m: ["  << minutes_ << "] " << blanks(minutes_)
       << "s: ["  << seconds_ << "] " << blanks(seconds_)
       << "ms: [" << ms << "] " << blanks(ms)
       << "us: [" << us << "] " << blanks(us)
       << "ns: [" << ns  << "]}^"<< exp;
    // clang-format on
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
};

class Timer {
 public:
  /*!
   * \brief start starts a new measurement.
   * \param s The name under which the measurement/timer shall be saved.
   */
  void start(const std::string &s = "") noexcept {
    const Timer::precisionClock::time_point start = precisionClock::now();
    begin_measurements[s] = start;
  }

  /*!
   * \brief stop stops a started measurement, computes the time since start()
   * and saves it. \param s The name under which the measurement/timer shall be
   * saved.
   */
  void stop(const std::string &s = "") noexcept {
    const Timer::precisionClock::time_point stop = precisionClock::now();
    const begin_measurements_it start_ = begin_measurements.find(s);
    if (start_ == begin_measurements.end()) {
      // TODO debugMsg: The timer with name s was never started
      return;
    }

    const std::chrono::nanoseconds duration =
        std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start_->second);
    measurements[s].emplace_back(duration);
  }

  struct Histogram {
    struct Bucket {
      PreciseTime begin;
      PreciseTime end;
      int num;
      PreciseTime getBucketCenter() const noexcept {
        return (end - begin) / 2.;
      }
    };
    std::vector<Bucket> buckets;
    PreciseTime bucket_size;
    int max_num_in_bucket = 0;

    PreciseTime scottsRuleBucketSize(size_t num_values, const PreciseTime &std_dev) const noexcept {
      const double num_good_values = static_cast<double>(num_values);
      return std_dev * 3.49 * 1. / std::cbrt(num_good_values);
    }

    void initBuckets(const PreciseTime &bucket_size_,
                     const PreciseTime &min,
                     const PreciseTime &max) {
      constexpr PreciseTime SMALLEST_BUCKET_SIZE(std::chrono::nanoseconds(1));
      bucket_size = bucket_size_;
      if (bucket_size < SMALLEST_BUCKET_SIZE) {
        bucket_size = SMALLEST_BUCKET_SIZE;
      }
      assert(max > min && "Max time lower than min time.");
      const PreciseTime range = max - min;
      const size_t num_buckets =
          static_cast<size_t>(range.toDouble<std::chrono::nanoseconds>() /
                              bucket_size.toDouble<std::chrono::nanoseconds>());

      buckets.reserve(num_buckets);

      PreciseTime iterator = min;
      while (iterator < max) {
        const auto end = iterator + bucket_size;
        Histogram::Bucket b{iterator, end, 0};
        buckets.push_back(std::move(b));
        iterator = end;
      }
    }

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
          color_s = std::string("\033[48:5:82:0m");  // light green
          break;
        case 1:
          color_s = std::string("\033[48:5:208:0m");  // orange
          break;
        case 2:
          color_s = std::string("\033[48:5:256:0m");  // white
          break;
        case 3:
          color_s = std::string("\033[48:5:9:0m");  // red
          break;
        case 4:
          color_s = std::string("\033[48:5:249:0m");  // grey
          break;
        case 5:
          color_s = std::string("\033[48:5:11:0m");  // yellow
          break;
        case 6:
          color_s = std::string("\033[48:5:225:0m");  // pink
          break;
        case 7:
          color_s = std::string("\033[48:5:90:0m");  // dark violet
          break;
        case 8:
          color_s = std::string("\033[48:5:45:0m");  // light blue
          break;
        case 9:
          color_s = std::string("\033[48:5:2:0m");  // darker green
          break;
      }
      return color_s + empty_tiles + end;
    }
  };

  // struct to bundel the statistics for one timers measurments
  struct Result {

    void setOutlinerRange(double n_times_deviation) noexcept {
      outliner_range = n_times_deviation;
    }

    size_t calcPlotSize() const noexcept {
      // clang-format off
      // <   BucketSizeInfo  ><PLOT>  <-- maximal r.num_char_terminal_width characters.
      // [213.0ns - 214.0ns] |######
      // clang-format on
      constexpr size_t BucketSizeInfo = 22;
      return std::max(50ul, num_char_terminal_width - BucketSizeInfo);
    }

    void streamOutBaseStatistics(std::ostream &os, const Result &r) const {
      os << "###Result of <" << r.timer_name << ">###"
         << "\n"
         << "E{X}: \t" << r.mean << "\n"
         << "Max{X}: " << r.max_measurement << "\n"
         << "Min{X}: " << r.min_measurement << "\n"
         << "D{X}: \t" << r.standard_derivation << "\n"
         << "N measurments: \t" << r.number_measurements << "\n"
         << "N outliners.: \t" << r.number_outliners << "\n";
    }

    void streamOutHistogram(std::ostream &os, const Result &r) const {

      const double smallest_unit =
          r.h.max_num_in_bucket / static_cast<double>(r.calcPlotSize());
      int num_buckets_skipped = 0;
      PreciseTime begin_skip;
      PreciseTime end_skip;
      auto streamBucketsSkipped = [&os, &num_buckets_skipped, &begin_skip, &end_skip]() {
        os << "[" << begin_skip.getTimeString(3) << " - "
           << end_skip.getTimeString(3) << "] |"
           << "skipped " << num_buckets_skipped << " insignificant bars"
           << "\n";
      };

      for (const auto &bucket : r.h.buckets) {
        const size_t num_units = static_cast<size_t>(bucket.num / smallest_unit);
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
     * \brief Implements a clean print for the statistics.
     */
    friend std::ostream &operator<<(std::ostream &os, const Result &r) {
      r.streamOutBaseStatistics(os, r);
      r.streamOutHistogram(os, r);
      return os;
    }

    /*!
     * \brief Implements a clean print for the statistics.
     */
    friend std::ostream &operator<<(std::ostream &os, const std::vector<Result> &rs) {
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
        max_num_in_bucket = std::max(max_num_in_bucket, result.h.max_num_in_bucket);
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
      auto streamBucketsSkipped = [&os, &num_buckets_skipped, &begin_skip, &end_skip]() {
        os << "[" << begin_skip.getTimeString(3) << " - "
           << end_skip.getTimeString(3) << "] |"
           << "skipped " << num_buckets_skipped << " insignificant bars"
           << "\n";
      };

      for (const auto &bucket_comb_histo : h.buckets) {

        for (size_t i : indexes_used) {
          const auto &current_buckets = rs[i].h.buckets;
          if (current_bucket[i] >= current_buckets.size()) {
            number_color[i] = std::pair<size_t, size_t>(0, i);  // not drawn anymore
            continue;  // already all values reorganized in h.buckets and printed
          }
          size_t current_bucket_nr = current_bucket[i];
          // Todo interpolation would be more precise than "fitting the center into the new histogram bucket".
          // But than again, one should use a graphical tool for a precise histogram anyway.
          const auto bucket_center = current_buckets[current_bucket_nr].getBucketCenter();

          if (bucket_center > bucket_comb_histo.end) {
            number_color[i] = std::pair<size_t, size_t>(0, i);  // not drawn
            continue;  // this histogram starts later
          }

          if (bucket_center < bucket_comb_histo.begin) {
            // we have to increase the current bucket
            current_bucket[i]++;
            if (current_bucket[i] == current_buckets.size()) {
              number_color[i] = std::pair<size_t, size_t>(0, i);  // not drawn anymore
              continue;  // we are at the end, no more buckets in current_buckets;
            }
          }
          // the current_bucket and bucket_comb_histo are aligned, we save number and index.
          number_color[i] =
              std::pair<size_t, size_t>(current_buckets[current_bucket_nr].num, i);
        }

        sort(number_color.begin(), number_color.end());
        size_t current_cursor_position = 0;
        std::string data("");
        for (const auto &nc : number_color) {
          const size_t num_units = static_cast<size_t>(nc.first / smallest_unit);
          if (current_cursor_position == num_units) {
            continue;  // skip this, we cannot print two colors onto each other
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
   * \return false if the name of the given timer doesn't exist or has less than 3 measurements.
   */
  bool getResult(const std::string &name, Result &result) noexcept {

    const auto timer = measurements.find(name);
    if (timer == measurements.end()) {
      return false;
    }

    result.number_measurements = timer->second.size();
    if (result.number_measurements < 3) {
      return false;
    }

    result.is_outliner = std::vector<char>(result.number_measurements, false);

    auto setMean = [&timer, &result]() {
      PreciseTime sum;
      for (size_t i = 0; i < result.number_measurements; ++i) {
        const auto &measurement = timer->second[i];
        if (!result.is_outliner[i]) {
          sum += measurement;
        }
      }
      result.mean = sum / (result.number_measurements - result.number_outliners);
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
      const size_t number_values = result.number_measurements - result.number_outliners;
      const auto bucket_size =
          result.h.scottsRuleBucketSize(number_values, result.standard_derivation);
      result.h.initBuckets(bucket_size, result.min_measurement, result.max_measurement);

      for (size_t i = 0; i < result.number_measurements; ++i) {
        const auto &measurement = timer->second[i];
        if (!result.is_outliner[i]) {
          for (auto &bucket : result.h.buckets) {
            if (bucket.begin <= measurement && measurement < bucket.end) {
              bucket.num++;
              break;
            }
          }
        }
      }

      for (const auto &bucket : result.h.buckets) {
        result.h.max_num_in_bucket = std::max(result.h.max_num_in_bucket, bucket.num);
      }
    };


    result.timer_name = name;

    // default deviation is maximal -> all values are inside outliner_range * deviation, no outliners
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
   * \brief toFile TODO
   */
  bool toFile(const std::string &file_name) {

    std::ofstream myfile;
    myfile.open(file_name);
    if (myfile.bad()) {
      return false;
    }
    myfile << this;
    myfile.close();
    return myfile.bad();
  }

 private:
  typedef std::conditional<std::chrono::high_resolution_clock::is_steady,
                           std::chrono::high_resolution_clock,
                           std::chrono::steady_clock>::type precisionClock;

  std::map<std::string, Timer::precisionClock::time_point> begin_measurements;
  typedef std::map<std::string, Timer::precisionClock::time_point>::iterator begin_measurements_it;
  std::map<std::string, std::vector<PreciseTime>> measurements;
};


class SingleTimer {
 public:
  void start() {
    started = true;
    start_time = precisionClock::now();
  }

  void reset() { started = false; }

  bool hasStarted() const { return started; }

  template <class T>
  T getPassedTime() const {
    const SingleTimer::precisionClock::time_point stop_time = precisionClock::now();
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

}  // namespace tool

#endif
