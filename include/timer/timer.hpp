/*
 * \file: timer.hpp
 * \brief: Implements the PreciseTime class useing std::chrono. Implements an
 * easy to use timer class.
 *
 * PreciseTime: Is able to convert between any std::chrono time and PreciseTime.
 * Features:
 *  - adding/substracting with
 *    PreciseTime class considering the unit:
 *    1[s] + 1[s] -> ok; 1[s] + 1[s^2] -> error
 *  - division/multiplication with
 *    a) int/double/etc
 *    b) PreciseTime class while keeping track of the unit:
 *       1[s] * 1[s] = 1[s^2]
 *    - calculateing the square root of a PreciseTime keeping track of
 *      he unit: sqrt(4[s^2]) = 2[s].
 *    - Comparisons: {==, !=, <=, >=, <, >}
 *    - Nice print output: std::cout << my_precise_time << std::endl;
 *    - Max Time:
 *      h: [9223372036854775807] m: [59]  s: [59]  ms: [999] us: [999] ns: [999]
 *    - Min Time:
 *      h: [-9223372036854775808] m: [-59]  s: [-59]  ms: [-999] us: [-999] ns:
 *      [-999]
 *
 * Timer: Useing the PreciseTime class to track time.
 * Features:
 *  - No need to register new timers.
 *  - As many timers as you like.
 *  - On demand output of max, min, mean, standard deviation for all timers.
 *
Example Usage:

#include <random>
#include<timer.hpp>

int main() {

  tool::PreciseTime max_pt = tool::PreciseTime::max();
  tool::PreciseTime min_pt = tool::PreciseTime::min();
  std::cout << max_pt << std::endl << min_pt << std::endl;

  // construction
  tool::PreciseTime my_time1(std::chrono::nanoseconds(987654321));
  tool::PreciseTime my_time2(std::chrono::milliseconds(42));
  tool::PreciseTime my_time3 = std::chrono::nanoseconds(22) +
                               std::chrono::microseconds(450) +
                               std::chrono::milliseconds(12);
  std::cout << my_time1 << std::endl;

  // calculations
  my_time1 = my_time1 + my_time1;
  my_time1 -= my_time2;

  my_time2 *= 1.5;
  my_time2 = my_time2 / 3.3;

  tool::PreciseTime timeSquared = my_time1 * my_time2;
  // This throws a runtime error if build in debug mode.
  tool::PreciseTime corruptedTime = timeSquared + my_time1;

  // Takeing the square root only works with usints s^n where n is even.
  tool::PreciseTime normalTime = tool::PreciseTime::sqrt(timeSquared);

  my_time1 = std::chrono::nanoseconds(22) + std::chrono::microseconds(450) +
             std::chrono::milliseconds(12);
  std::cout << my_time1 << std::endl;

  // coversations
  typedef std::chrono::microseconds exampleType;

  // This gets the time in exampleType ! this might result in resolution loss
  // if the time is 22ns and 450us and 12ms, This returns exact 12450us
  exampleType us = my_time3.convert<exampleType>();
  std::cout << us.count() << std::endl;

  // if the time is 22ns and 450us and 12ms, This returns exact 450us
  exampleType part_us = my_time3.get<exampleType>();
  std::cout << part_us.count() << std::endl;

  // if the time is 22ns and 450us and 12ms, This returns exact* 12450.022
  // double precision exact
  double floatingPoint = my_time3.toDouble<exampleType>();
  std::cout << std::fixed << floatingPoint << std::endl << std::endl;

  // Usage of timer class
  tool::Timer timer;

  timer.start("for-timer");

  std::random_device rd;
  std::default_random_engine generator(rd());
  const double mean = 1.;
  const double deviation = 0.15;
  timer.start("for-timer");
  for (int i = 0; i < 10000; i++) {
    std::normal_distribution<double> distribution(mean, deviation);
    const double sleep = distribution(generator);
    timer.start("sleepTimer");
    usleep(sleep * 100);
    timer.stop("sleepTimer");
  }
  timer.stop("for-timer");

  return 0;
}

 * \date: 30.07.2019
 * \author: Jakob Wandel
 * \version: 1.0
 */

#ifndef TIMER_H
#define TIMER_H

#include <math.h>

#include <cassert>
#include <chrono>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace tool {

/*
 * \brief x2y(t x)
 * \param x time in x
 * \return time in y
 */
template <class t>
auto mu2ns(t micro) {
  return static_cast<t>(micro * 1000.);
}

template <class t>
auto ms2ns(t milli) {
  return static_cast<t>(milli * 1000000.);
}

template <class t>
auto s2ns(t sec) {
  return static_cast<t>(sec * 1000000000.);
}

template <class t>
auto ns2us(t ns) {
  return static_cast<t>(ns / 1000.);
}

template <class t>
auto us2ns(t us) {
  return static_cast<t>(us * 1000.);
}

template <class t>
auto ns2ms(t ns) {
  return static_cast<t>(ns / 1000000.);
}

template <class t>
auto ns2s(t ns) {
  return static_cast<t>(ns / 1000000000.);
}

template <class t>
auto ns2m(t ns) {
  return static_cast<t>(ns / (1000000000. * 60.));
}

template <class t>
auto ns2h(t ns) {
  return static_cast<t>(ns / (1000000000. * 3600.));
}

template <class t>
auto m2s(t m) {
  return static_cast<t>(m * 60.);
}

template <class t>
auto h2s(t h) {
  return static_cast<t>(h * 3600.);
}

template <class t>
auto h2m(t h) {
  return static_cast<t>(h * 60.);
}

template <class t>
auto h2ms(t h) {
  return static_cast<t>(h * 3600. * 1000.);
}

template <class t>
auto h2us(t h) {
  return static_cast<t>(h * 3600. * 1000000.);
}

template <class t>
auto h2ns(t h) {
  return static_cast<t>(h * 3600. * 1000000000.);
}

template <class t>
auto s2us(t s) {
  return static_cast<t>(s * 1000000.);
}

template <class t>
auto s2ms(t s) {
  return static_cast<t>(s * 1000.);
}

template <class t>
auto s2m(t s) {
  return static_cast<t>(s / 60.);
}

template <class t>
auto s2h(t s) {
  return static_cast<t>(s / 3600.);
}

template <class t>
auto s2hf(t s) {
  return static_cast<t>(std::floor(s / 3600.));
}

class PreciseTime {
 public:
  /// <Construction Area>
  ///
  ///
  PreciseTime() {}

  PreciseTime(const std::chrono::nanoseconds &nano_seconds_) {
    nano_seconds = nano_seconds_;
    sanitize();
  }

  PreciseTime(const std::chrono::microseconds &micro_seconds_) {
    nano_seconds = std::chrono::nanoseconds(mu2ns(micro_seconds_.count()));
    sanitize();
  }

  PreciseTime(const std::chrono::milliseconds &milli_seconds_) {
    nano_seconds = std::chrono::nanoseconds(ms2ns(milli_seconds_.count()));
    sanitize();
  }

  PreciseTime(const std::chrono::seconds &seconds_) {
    seconds = seconds_;
    sanitize();
  }

  PreciseTime(const std::chrono::minutes &minutes_) {
    seconds = std::chrono::seconds(m2s(minutes_.count()));
    sanitize();
  }

  PreciseTime(const std::chrono::hours &hours_) { hours = hours_; }
  ///
  ///
  /// </Construction Area>

  /*!
   * \brief setExponent allowes to alter the exponent of the PreciseTime.
   * \param exp The exponent.
   */
  void setExponent(int exp) { exponent = exp; }

  /*!
   * \brief getExponent returns the exponent of the PreciseTime.
   * \return The exponent.
   */
  int getExponent() const { return exponent; }

  /// <Arithmetics>
  ///
  ///
  void operator+=(const PreciseTime &pt) {
    assert(pt.exponent == exponent &&
           "You can not add different units like s + s^2");
    nano_seconds += pt.nano_seconds;
    seconds += pt.seconds;
    hours += pt.hours;

    sanitize();
  }

  PreciseTime operator+(const PreciseTime &pt) const {
    assert(pt.exponent == exponent &&
           "You can not add different units like s + s^2");
    PreciseTime ret(*this);
    ret += pt;
    return ret;
  }

  void operator-=(const PreciseTime &pt) {
    assert(pt.exponent == exponent &&
           "You can not substartc different units like s - s^2");
    nano_seconds -= pt.nano_seconds;
    seconds -= pt.seconds;
    hours -= pt.hours;

    sanitize();
  }

  PreciseTime operator-(const PreciseTime &pt) const {
    assert(pt.exponent == exponent &&
           "You can not substract different units like s - s^2");
    PreciseTime ret(*this);
    ret -= pt;
    return ret;
  }

  void operator*=(const double multi) {
    const double hours = this->hours.count() * multi;
    double remaining_hours;
    const double seconds_rest = h2s(std::modf(hours, &remaining_hours));

    const double seconds = this->seconds.count() * multi + seconds_rest;
    double remaining_seconds;
    const double nanoseconds_rest = s2ns(std::modf(seconds, &remaining_seconds));

    const double nanoseconds = this->nano_seconds.count() * multi + nanoseconds_rest;

    this->nano_seconds =
        std::chrono::nanoseconds(static_cast<long>(std::round(nanoseconds)));
    this->seconds =
        std::chrono::seconds(static_cast<long>(std::round(remaining_seconds)));
    this->hours = std::chrono::hours(static_cast<long>(std::round(remaining_hours)));

    sanitize();
  }

  PreciseTime operator*(const double multi) const {
    PreciseTime ret(*this);
    ret *= multi;
    return ret;
  }

  PreciseTime operator*(const PreciseTime &pt) const {
    double resulting_s =
        pt.toDouble<std::chrono::seconds>() * toDouble<std::chrono::seconds>();

    PreciseTime ret(
        std::chrono::nanoseconds(static_cast<long>(std::round(s2ns(resulting_s)))));
    ret.exponent = pt.exponent + exponent;
    return ret;
  }

  PreciseTime operator/(const double div) const { return *this * (1. / div); }

  void operator/=(const double div) { *this *= (1. / div); }

  PreciseTime operator/(const PreciseTime &pt) const {
    double resulting_s =
        pt.toDouble<std::chrono::seconds>() / toDouble<std::chrono::seconds>();
    PreciseTime ret(
        std::chrono::nanoseconds(static_cast<long>(std::round(s2ns(resulting_s)))));
    ret.exponent = exponent - pt.exponent;
    return ret;
  }

  static const PreciseTime sqrt(const PreciseTime &pt) {
    assert(pt.exponent % 2 == 0 &&
           "squareroot of Precise time with odd exponent not supported.");
    PreciseTime ret(std::chrono::nanoseconds(static_cast<long>(
        std::round(s2ns(std::sqrt(pt.toDouble<std::chrono::seconds>()))))));

    ret.exponent = pt.exponent;
    ret.exponent /= 2;
    return ret;
  }

  void sqrt() {
    assert(exponent % 2 == 0 &&
           "squareroot of Precise time with odd exponent not supported.");

    PreciseTime ret(std::chrono::nanoseconds(static_cast<long>(
        std::round(s2ns(std::sqrt(toDouble<std::chrono::seconds>()))))));

    ret.exponent = ret.exponent;
    ret.exponent /= 2;
    *this = ret;
  }

  ///
  ///
  /// </Arithmetics>

  /// <Comparisons>
  ///
  ///
  bool operator==(const PreciseTime &pt) const {
    return pt.exponent == exponent && pt.nano_seconds == nano_seconds &&
           pt.seconds == seconds && pt.hours == hours;
  }

  bool operator!=(const PreciseTime &pt) const { return !(*this == pt); }

  bool operator<(const PreciseTime &pt) const {
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

  bool operator>(const PreciseTime &pt) const {
    if (*this < pt) {
      return false;
    }
    if (*this == pt) {
      return false;
    }
    return true;
  }

  bool operator<=(const PreciseTime &pt) const {
    if (*this > pt) {
      return false;
    }
    return true;
  }

  bool operator>=(const PreciseTime &pt) const {
    if (*this < pt) {
      return false;
    }
    return true;
  }
  ///
  ///
  /// </Comparisons>

  /*!
   * \brief toDouble() Casts the PrecisionTime to a double value in the unit
   * given with the template
   * \return the time as a double in the given unit.
   */
  template <class c>
  typename std::enable_if<std::is_same<c, std::chrono::nanoseconds>::value, double>::type toDouble() const {
    return h2ns(static_cast<double>(hours.count())) +
           s2ns(static_cast<double>(seconds.count())) +
           static_cast<double>(nano_seconds.count());
  }

  /*!
   * \brief toDouble() Casts the PrecisionTime to a double value in the unit
   * given with the template
   * \return the time as a double in the given unit.
   */
  template <class c>
  typename std::enable_if<std::is_same<c, std::chrono::microseconds>::value, double>::type toDouble() const {
    return h2us(static_cast<double>(hours.count())) +
           s2us(static_cast<double>(seconds.count())) +
           ns2us(static_cast<double>(nano_seconds.count()));
  }

  /*!
   * \brief toDouble() Casts the PrecisionTime to a double value in the unit
   * given with the template
   * \return the time as a double in the given unit.
   */
  template <class c>
  typename std::enable_if<std::is_same<c, std::chrono::milliseconds>::value, double>::type toDouble() const {
    return h2ms(static_cast<double>(hours.count())) +
           s2ms(static_cast<double>(seconds.count())) +
           ns2ms(static_cast<double>(nano_seconds.count()));
  }

  /*!
   * \brief toDouble() Casts the PrecisionTime to a double value in the unit
   * given with the template
   * \return the time as a double in the given unit.
   */
  template <class c>
  typename std::enable_if<std::is_same<c, std::chrono::seconds>::value, double>::type toDouble() const {
    return h2s(static_cast<double>(hours.count())) +
           static_cast<double>(seconds.count()) +
           ns2s(static_cast<double>(nano_seconds.count()));
  }

  /*!
   * \brief toDouble() Casts the PrecisionTime to a double value in the unit
   * given with the template
   * \return the time as a double in the given unit.
   */
  template <class c>
  typename std::enable_if<std::is_same<c, std::chrono::minutes>::value, double>::type toDouble() const {
    return h2m(static_cast<double>(hours.count())) +
           s2m(static_cast<double>(seconds.count())) +
           ns2m(static_cast<double>(nano_seconds.count()));
  }

  /*!
   * \brief toDouble() Casts the PrecisionTime to a double value in the unit
   * given with the template
   * \return the time as a double in the given unit.
   */
  template <class c>
  typename std::enable_if<std::is_same<c, std::chrono::hours>::value, double>::type toDouble() const {
    return static_cast<double>(hours.count()) +
           s2h(static_cast<double>(seconds.count())) +
           ns2h(static_cast<double>(nano_seconds.count()));
  }

  /*!
   * \brief convert() Converts the PrecisionTime to the type given in the
   * template.
   * \return if preciseTime = 2h,3m,44s,40ms,66us,12ns > convert<nanoseconds>
   * will return (((3600*2h + 60*3m + 44s)*1000 + 40ms)*1000 + 66us)*1000 + 12ns
   * = 7364040066012ns
   */
  template <class c>
  typename std::enable_if<std::is_same<c, std::chrono::nanoseconds>::value, c>::type convert() const {
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
  typename std::enable_if<std::is_same<c, std::chrono::microseconds>::value, c>::type convert() const {
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
  typename std::enable_if<std::is_same<c, std::chrono::milliseconds>::value, c>::type convert() const {
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
  typename std::enable_if<std::is_same<c, std::chrono::seconds>::value, c>::type convert() const {
    return std::chrono::seconds(h2s(hours.count())) + seconds;
  }

  /*!
   * \brief convert() Converts the PrecisionTime to the type given in the
   * template.
   * \return if preciseTime = 2h,3m,44s,40ms,66us,12ns > convert<minutes> will
   * return 60*2h + 3m = 123m
   */
  template <class c>
  typename std::enable_if<std::is_same<c, std::chrono::minutes>::value, c>::type convert() const {
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
  typename std::enable_if<std::is_same<c, std::chrono::hours>::value, c>::type convert() const {
    return hours;
  }

  /*!
   * \brief get() Returns the part of the time of which the type was given.
   * \return if preciseTime = 2h,3m,44s,40ms,66us,12ns > get<nanoseconds> will
   * return 12ns
   */
  template <class c>
  typename std::enable_if<std::is_same<c, std::chrono::nanoseconds>::value, c>::type get() const {
    const long ns = nano_seconds.count();
    if (std::abs(ns) > 999) {
      double us;
      const double ns_rest = std::modf(ns / 1000., &us) * 1000.;
      return std::chrono::nanoseconds(static_cast<long>(std::round(ns_rest)));
    }
    return nano_seconds;
  }

  /*!
   * \brief get() Returns the part of the time of which the type was given.
   * \return if preciseTime = 2h,3m,44s,40ms,66us,12ns > get<microseconds> will
   * return 66us
   */
  template <class c>
  typename std::enable_if<std::is_same<c, std::chrono::microseconds>::value, c>::type get() const {
    const long us =
        ns2us(nano_seconds.count() - get<std::chrono::nanoseconds>().count());
    if (std::abs(us) > 999) {
      double ms;
      const double us_rest = std::modf(us / 1000., &ms) * 1000.;
      return std::chrono::microseconds(static_cast<long>(std::round(us_rest)));
    }
    return std::chrono::microseconds(us);
  }

  /*!
   * \brief get() Returns the part of the time of which the type was given.
   * \return if preciseTime = 2h,3m,44s,40ms,66us,12ns > get<milliseconds> will
   * return 40ms
   */
  template <class c>
  typename std::enable_if<std::is_same<c, std::chrono::milliseconds>::value, c>::type get() const {
    const long ms =
        ns2ms(nano_seconds.count() - get<std::chrono::microseconds>().count());
    if (std::abs(ms) > 999) {
      double s;
      const double ms_rest = std::modf(ms / 1000., &s) * 1000.;
      return std::chrono::milliseconds(static_cast<long>(std::round(ms_rest)));
    }
    return std::chrono::milliseconds(ms);
  }

  /*!
   * \brief get() Returns the part of the time of which the type was given.
   * \return if preciseTime = 2h,3m,44s,40ms,66us,12ns > get<seconds> will
   * return 44s
   */
  template <class c>
  typename std::enable_if<std::is_same<c, std::chrono::seconds>::value, c>::type get() const {
    const long sec = seconds.count();
    if (std::abs(sec) > 59) {
      double m;
      const double s_rest = std::modf(sec / 60., &m) * 60.;
      return std::chrono::seconds(static_cast<long>(std::round(s_rest)));
    }
    return seconds;
  }

  /*!
   * \brief get() Returns the part of the time of which the type was given.
   * \return if preciseTime = 2h,3m,44s,40ms,66us,12ns > get<minutes> will
   * return 3m
   */
  template <class c>
  typename std::enable_if<std::is_same<c, std::chrono::minutes>::value, c>::type get() const {
    const long m = s2m(seconds.count() - get<std::chrono::seconds>().count());
    if (std::abs(m) > 59) {
      double h;
      const double m_rest = std::modf(m / 60., &h) * 60.;
      return std::chrono::minutes(static_cast<long>(std::round(m_rest)));
    }
    return std::chrono::minutes(m);
  }

  /*!
   * \brief get Returns the part of the time of which the type was given.
   * \return if preciseTime = 2h,3m,44s,40ms,66us,12ns > get<hours> will
   * return 2h
   */
  template <class c>
  typename std::enable_if<std::is_same<c, std::chrono::hours>::value, c>::type get() const {
    return hours;
  }

  /*!
   * \brief Implements a clean print for the current time:
   * {h: [xxxx]   m: [xx]   s: [xxx]   ms: [xxx]   us: [xxx]  ns:
   * [xxx]}^exponent
   */
  friend std::ostream &operator<<(std::ostream &os, const PreciseTime &pt) {
    auto blanks = [](long num) {
      const long i = std::abs(num);
      if (i < 10) {
        return "  ";
      } else if (i < 100) {
        return " ";
      }
      return "";
    };

    const long hours = pt.get<std::chrono::hours>().count();
    const long minutes = pt.get<std::chrono::minutes>().count();
    const long seconds = pt.get<std::chrono::seconds>().count();
    const long ms = pt.get<std::chrono::milliseconds>().count();
    const long us = pt.get<std::chrono::microseconds>().count();
    const long ns = pt.get<std::chrono::nanoseconds>().count();
    const std::string exp = std::to_string(pt.exponent);

    // clang-format off
    os << "{h: ["  << hours   << "] " << blanks(hours)
       << "m: ["  << minutes << "] " << blanks(minutes)
       << "s: ["  << seconds << "] " << blanks(seconds)
       << "ms: [" << ms << "] " << blanks(ms)
       << "us: [" << us << "] " << blanks(us)
       << "ns: [" << ns  << "]}^"<<exp;
    // clang-format on
    return os;
  }

  /*!
   * \brief max Returns the greatest time the PreciseTime class can hold
   */
  static const PreciseTime max() {
    PreciseTime ps(std::chrono::hours::max());
    ps.seconds = MAX_VALIDE_S;
    ps.nano_seconds = MAX_VALIDE_NS;
    return ps;
  }

  /*!
   * \brief min Returns the smallest time the PreciseTime class can hold
   */
  static const PreciseTime min() {
    PreciseTime ps(std::chrono::hours::min());
    ps.seconds = MIN_VALIDE_S;
    ps.nano_seconds = MIN_VALIDE_NS;
    return ps;
  }

  /*!
   * \brief zero Returns a PreciseTime of zero
   */
  static const PreciseTime zero() {
    const PreciseTime ps;
    return ps;
  }

 private:
  /*!
   * \brief sanitizeNS If the internal counter for the nano seconds get over
   * 999999999 we adjust the nanoseconds by adding to the internal seconds.
   */
  void sanitizeNS() {
    if (nano_seconds > MAX_VALIDE_NS || nano_seconds < MIN_VALIDE_NS) {
      double seconds_from_nanoseconds = ns2s(nano_seconds.count());
      double carry_seconds;
      std::modf(seconds_from_nanoseconds, &carry_seconds);
      const long carry_seconds_l = static_cast<long>(std::round(carry_seconds));

      // substract the nanoseconds which made up a second
      nano_seconds =
          std::chrono::nanoseconds(nano_seconds.count() - s2ns(carry_seconds_l));
      // add them to the seconds
      seconds += std::chrono::seconds(carry_seconds_l);
    }
  }

  /*!
   * \brief sanitizeS If the internal counter for the seconds 3599
   * we adjust the seconds by adding to the internal hours.
   */
  void sanitizeS() {
    /// Sanitize seconds
    if (seconds > MAX_VALIDE_S || seconds < MIN_VALIDE_S) {
      double hours_from_seconds = s2h(seconds.count());
      double carry_hours;
      std::modf(hours_from_seconds, &carry_hours);
      const long carry_hours_l = static_cast<long>(std::round(carry_hours));

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
  void sanitizeSign() {
    std::function<bool(long)> pos = [](long probe) { return probe >= 0; };
    std::function<bool(long)> neg = [](long probe) { return probe <= 0; };
    std::function<bool(long)> sign;
    if (hours.count() < 0) {
      sign = neg;
    } else if (hours.count() > 0) {
      sign = pos;
    } else {
      if (seconds.count() < 0) {
        sign = neg;
      } else if (seconds.count() > 0) {
        sign = pos;
      } else {
        return;  // we dont need to worry about different signs
      }
    }

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
  void sanitize() {

    sanitizeNS();
    sanitizeS();
    sanitizeSign();
  }

  static constexpr std::chrono::seconds MAX_VALIDE_S = std::chrono::seconds(3599);
  static constexpr std::chrono::seconds MIN_VALIDE_S = std::chrono::seconds(-3599);
  static constexpr std::chrono::nanoseconds MAX_VALIDE_NS =
      std::chrono::nanoseconds(999999999);
  static constexpr std::chrono::nanoseconds MIN_VALIDE_NS =
      std::chrono::nanoseconds(-999999999);

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
   * \brief Destructor will print every timer into the cout.
   */
  ~Timer() { std::cout << *this << std::endl; }

  /*!
   * \brief start starts a new measurement.
   * \param s The name under which the measurement/timer shall be saved.
   */
  void start(const std::string &s = "") {
    const Timer::precisionClock::time_point start = precisionClock::now();
    begin_measurements[s] = start;
  }

  /*!
   * \brief stop stops a started measurement, computes the time since start()
   * and saves it. \param s The name under which the measurement/timer shall be
   * saved.
   */
  void stop(const std::string &s = "") {
    const Timer::precisionClock::time_point stop = precisionClock::now();
    const begin_measurements_it start = begin_measurements.find(s);
    if (start == begin_measurements.end()) {
      // TODO debugMsg: The timer with name s was never started
      return;
    }

    const std::chrono::nanoseconds duration =
        std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start->second);
    measurements[s].emplace_back(duration);
    results[s].needs_update = true;
  }

  // struct to bundel the statistics for one timers measurments
  struct Result {
    PreciseTime min_measurement = PreciseTime::max();
    PreciseTime max_measurement = PreciseTime::min();
    PreciseTime average;
    PreciseTime standard_derivation;
    int number_measurements;
    bool needs_update = true;

    /*!
     * \brief Implements a clean print for the statistics.
     */
    friend std::ostream &operator<<(std::ostream &os, const Result &r) {
      os << "###Result###"
         << "\n"
         << "E{X}: \t" << r.average << "\n"
         << "Max{X}: " << r.max_measurement << "\n"
         << "Min{X}: " << r.min_measurement << "\n"
         << "D{X}: \t" << r.standard_derivation << "\n"
         << "N: \t" << r.number_measurements << std::endl;
      return os;
    }
  };

  /*!
   * \brief getResult Calculates for the given timer the statiscs and returns
   * them
   * \param name The name of the timer the statistics shall be calculated for.
   */
  const Result &getResult(const std::string &name) {

    if (!results[name].needs_update) {
      return results[name];
    }

    const auto timer = measurements.find(name);
    if (timer == measurements.end()) {
      return results[name];
    }

    results[name].number_measurements = 0;
    PreciseTime sum;
    // mean, min and max

    for (const auto &measurement : timer->second) {
      if (measurement > results[name].max_measurement) {
        results[name].max_measurement = measurement;
      }
      if (measurement < results[name].min_measurement) {
        results[name].min_measurement = measurement;
      }
      sum += measurement;
      results[name].number_measurements++;
    }

    results[name].average = sum / results[name].number_measurements;

    if (results[name].number_measurements < 2) {
      return results[name];
    }

    // variance
    PreciseTime var_sum;
    var_sum.setExponent(2);

    for (const auto &measurement : timer->second) {
      const PreciseTime diff_pt = measurement - results[name].average;
      var_sum += diff_pt * diff_pt;
    }

    const PreciseTime variance = var_sum / (results[name].number_measurements - 1);

    results[name].standard_derivation = PreciseTime::sqrt(variance);

    results[name].needs_update = false;
    return results[name];
  }

  /*!
   * \brief Calculates for all saved measurments/timers the statistics and
   * prints them.
   */
  friend std::ostream &operator<<(std::ostream &os, Timer &t) {
    for (const auto &timer : t.measurements) {
      os << "Timer: " << timer.first << std::endl
         << t.getResult(timer.first) << std::endl;
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

  std::map<std::string, Result> results;
};


class SingleTimer {
 public:
  void start() {
    started = true;
    start_ = precisionClock::now();
  }

  void stop() { started = false; }

  bool hasStarted() const { return started; }

  template <class T>
  T getPassedTime() const {
    const SingleTimer::precisionClock::time_point stop = precisionClock::now();
    return std::chrono::duration_cast<T>(stop - start_);
  }


 private:
  typedef std::conditional<std::chrono::high_resolution_clock::is_steady,
                           std::chrono::high_resolution_clock,
                           std::chrono::steady_clock>::type precisionClock;

  SingleTimer::precisionClock::time_point start_;
  bool started = false;
};

}  // namespace tool

#endif
