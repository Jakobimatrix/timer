#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <iostream>
#include <map>
#include <math.h>
#include <string>
#include <vector>

namespace tool {

class PreciseTime {
public:
  PreciseTime(){};

  PreciseTime(const std::chrono::nanoseconds &nano_seconds_) {
    nano_seconds = nano_seconds_;
    sanitize();
  }

  PreciseTime(const std::chrono::microseconds &micro_seconds_) {
    micro_seconds = micro_seconds_;
    sanitize();
  }

  PreciseTime(const std::chrono::milliseconds &milli_seconds_) {
    milli_seconds = milli_seconds_;
    sanitize();
  }

  PreciseTime(const std::chrono::seconds &seconds_) {
    seconds = seconds_;
    sanitize();
  }

  PreciseTime(const std::chrono::minutes &minutes_) {
    minutes = minutes_;
    sanitize();
  }

  PreciseTime(const std::chrono::hours &hours_) {
    hours = hours_;
    sanitize();
  }

  PreciseTime operator+(const PreciseTime &pt) const {
    PreciseTime ret(pt);
    ret.nano_seconds += nano_seconds;
    ret.micro_seconds += micro_seconds;
    ret.milli_seconds += milli_seconds;
    ret.seconds += seconds;
    ret.minutes += minutes;
    ret.hours += hours;

    ret.sanitize();
    return ret;
  }

  void operator+=(const PreciseTime &pt) {
    nano_seconds += pt.nano_seconds;
    micro_seconds += pt.micro_seconds;
    milli_seconds += pt.milli_seconds;
    seconds += pt.seconds;
    minutes += pt.minutes;
    hours += pt.hours;

    sanitize();
  }

  PreciseTime operator-(const PreciseTime &pt) const {
    PreciseTime ret(pt);
    ret.nano_seconds -= nano_seconds;
    ret.micro_seconds -= micro_seconds;
    ret.milli_seconds -= milli_seconds;
    ret.seconds -= seconds;
    ret.minutes -= minutes;
    ret.hours -= hours;

    ret.sanitize();
    return ret;
  }

  void operator-=(const PreciseTime &pt) {
    nano_seconds -= pt.nano_seconds;
    micro_seconds -= pt.micro_seconds;
    milli_seconds -= pt.milli_seconds;
    seconds -= pt.seconds;
    minutes -= pt.minutes;
    hours -= pt.hours;

    sanitize();
  }

  PreciseTime operator*(const double multi) const {
    const double hours = this->hours.count() * multi;
    double remaining_hours;
    const double minutes_rest = std::modf(hours, &remaining_hours) * 60.;

    const double minutes = this->minutes.count() * multi + minutes_rest;
    double remaining_minutes;
    const double seconds_rest = std::modf(minutes, &remaining_minutes) * 60.;

    const double seconds = this->seconds.count() * multi + seconds_rest;
    double remaining_seconds;
    const double milliseconds_rest =
        std::modf(seconds, &remaining_seconds) * 1000.;

    const double milliseconds =
        this->milli_seconds.count() * multi + milliseconds_rest;
    double remaining_milliseconds;
    const double microseconds_rest =
        std::modf(milliseconds, &remaining_milliseconds) * 1000.;

    const double microseconds =
        this->micro_seconds.count() * multi + microseconds_rest;
    double remaining_microseconds;
    const double nanoseconds_rest =
        std::modf(microseconds, &remaining_microseconds) * 1000.;

    const double nanoseconds =
        this->nano_seconds.count() * multi + nanoseconds_rest;

    const int sign = nanoseconds < 0 ? -1 : 1;

    PreciseTime ret;
    ret.nano_seconds =
        std::chrono::nanoseconds(static_cast<int>(std::round(nanoseconds)));
    ret.micro_seconds = std::chrono::microseconds(
        static_cast<int>(sign * remaining_microseconds));
    ret.milli_seconds = std::chrono::milliseconds(
        static_cast<int>(sign * remaining_milliseconds));
    ret.seconds =
        std::chrono::seconds(static_cast<int>(sign * remaining_seconds));
    ret.minutes =
        std::chrono::minutes(static_cast<int>(sign * remaining_minutes));
    ret.hours = std::chrono::hours(static_cast<int>(sign * remaining_hours));

    ret.sanitize();
    return ret;
  }

  void operator*=(const double multi) {
    const double hours = this->hours.count() * multi;
    double remaining_hours;
    const double minutes_rest = std::modf(hours, &remaining_hours) * 60.;

    const double minutes = this->minutes.count() * multi + minutes_rest;
    double remaining_minutes;
    const double seconds_rest = std::modf(minutes, &remaining_minutes) * 60.;

    const double seconds = this->seconds.count() * multi + seconds_rest;
    double remaining_seconds;
    const double milliseconds_rest =
        std::modf(seconds, &remaining_seconds) * 1000.;

    const double milliseconds =
        this->milli_seconds.count() * multi + milliseconds_rest;
    double remaining_milliseconds;
    const double microseconds_rest =
        std::modf(milliseconds, &remaining_milliseconds) * 1000.;

    const double microseconds =
        this->micro_seconds.count() * multi + microseconds_rest;
    double remaining_microseconds;
    const double nanoseconds_rest =
        std::modf(microseconds, &remaining_microseconds) * 1000.;

    const double nanoseconds =
        this->nano_seconds.count() * multi + nanoseconds_rest;

    nano_seconds =
        std::chrono::nanoseconds(static_cast<int>(std::round(nanoseconds)));
    micro_seconds =
        std::chrono::microseconds(static_cast<int>(remaining_microseconds));
    milli_seconds =
        std::chrono::milliseconds(static_cast<int>(remaining_milliseconds));
    this->seconds = std::chrono::seconds(static_cast<int>(remaining_seconds));
    this->minutes = std::chrono::minutes(static_cast<int>(remaining_minutes));
    this->hours = std::chrono::hours(static_cast<int>(remaining_hours));

    sanitize();
  }

  PreciseTime operator/(const double div) const {
    const double hours = this->hours.count() / div;
    double remaining_hours;
    const double minutes_rest = std::modf(hours, &remaining_hours) * 60.;

    const double minutes = this->minutes.count() / div + minutes_rest;
    double remaining_minutes;
    const double seconds_rest = std::modf(minutes, &remaining_minutes) * 60.;

    const double seconds = this->seconds.count() / div + seconds_rest;
    double remaining_seconds;
    const double milliseconds_rest =
        std::modf(seconds, &remaining_seconds) * 1000.;

    const double milliseconds =
        this->milli_seconds.count() / div + milliseconds_rest;
    double remaining_milliseconds;
    const double microseconds_rest =
        std::modf(milliseconds, &remaining_milliseconds) * 1000.;

    const double microseconds =
        this->micro_seconds.count() / div + microseconds_rest;
    double remaining_microseconds;
    const double nanoseconds_rest =
        std::modf(microseconds, &remaining_microseconds) * 1000.;

    const double nanoseconds =
        this->nano_seconds.count() / div + nanoseconds_rest;

    PreciseTime ret;
    ret.nano_seconds =
        std::chrono::nanoseconds(static_cast<int>(std::round(nanoseconds)));
    ret.micro_seconds =
        std::chrono::microseconds(static_cast<int>(remaining_microseconds));
    ret.milli_seconds =
        std::chrono::milliseconds(static_cast<int>(remaining_milliseconds));
    ret.seconds = std::chrono::seconds(static_cast<int>(remaining_seconds));
    ret.minutes = std::chrono::minutes(static_cast<int>(remaining_minutes));
    ret.hours = std::chrono::hours(static_cast<int>(remaining_hours));

    ret.sanitize();
    return ret;
  }

  void operator/=(const double div) {
    const double hours = this->hours.count() / div;
    double remaining_hours;
    const double minutes_rest = std::modf(hours, &remaining_hours) * 60.;

    const double minutes = this->minutes.count() / div + minutes_rest;
    double remaining_minutes;
    const double seconds_rest = std::modf(minutes, &remaining_minutes) * 60.;

    const double seconds = this->seconds.count() / div + seconds_rest;
    double remaining_seconds;
    const double milliseconds_rest =
        std::modf(seconds, &remaining_seconds) * 1000.;

    const double milliseconds =
        this->milli_seconds.count() / div + milliseconds_rest;
    double remaining_milliseconds;
    const double microseconds_rest =
        std::modf(milliseconds, &remaining_milliseconds) * 1000.;

    const double microseconds =
        this->micro_seconds.count() / div + microseconds_rest;
    double remaining_microseconds;
    const double nanoseconds_rest =
        std::modf(microseconds, &remaining_microseconds) * 1000.;

    const double nanoseconds =
        this->nano_seconds.count() / div + nanoseconds_rest;

    nano_seconds =
        std::chrono::nanoseconds(static_cast<int>(std::round(nanoseconds)));
    micro_seconds =
        std::chrono::microseconds(static_cast<int>(remaining_microseconds));
    milli_seconds =
        std::chrono::milliseconds(static_cast<int>(remaining_milliseconds));
    this->seconds = std::chrono::seconds(static_cast<int>(remaining_seconds));
    this->minutes = std::chrono::minutes(static_cast<int>(remaining_minutes));
    this->hours = std::chrono::hours(static_cast<int>(remaining_hours));

    sanitize();
  }

  bool operator==(const PreciseTime &pt) const {
    return pt.nano_seconds == nano_seconds &&
           pt.micro_seconds == micro_seconds &&
           pt.milli_seconds == milli_seconds && pt.seconds == seconds &&
           pt.minutes == minutes && pt.hours == hours;
  }

  bool operator!=(const PreciseTime &pt) const { return !(*this == pt); }

  bool operator<(const PreciseTime &pt) const {
    if (hours == pt.hours) {
      if (minutes == pt.minutes) {
        if (seconds == pt.seconds) {
          if (milli_seconds == pt.milli_seconds) {
            if (micro_seconds == pt.micro_seconds) {
              if (nano_seconds == pt.nano_seconds) {
                return false;
              } else if (nano_seconds < pt.nano_seconds) {
                return true;
              }
            } else if (micro_seconds < pt.micro_seconds) {
              return true;
            }
          } else if (milli_seconds < pt.milli_seconds) {
            return true;
          }
        } else if (seconds < pt.seconds) {
          return true;
        }
      } else if (minutes < pt.minutes) {
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

  template <class c>
  typename std::enable_if<std::is_same<c, std::chrono::nanoseconds>::value,
                          double>::type
  toDouble() const {
    return convert<c>().count();
  }

  template <class c>
  typename std::enable_if<std::is_same<c, std::chrono::microseconds>::value,
                          double>::type
  toDouble() const {
    return convert<c>().count() + micro_seconds.count() / 1000.;
  }

  template <class c>
  typename std::enable_if<std::is_same<c, std::chrono::milliseconds>::value,
                          double>::type
  toDouble() const {
    return convert<c>().count() + micro_seconds.count() / 1000. +
           nano_seconds.count() / 1000000.;
  }

  template <class c>
  typename std::enable_if<std::is_same<c, std::chrono::seconds>::value,
                          double>::type
  toDouble() const {
    return convert<c>().count() + milli_seconds.count() / 1000. +
           micro_seconds.count() / 1000000. +
           nano_seconds.count() / 1000000000.;
  }

  template <class c>
  typename std::enable_if<std::is_same<c, std::chrono::minutes>::value,
                          double>::type
  toDouble() const {
    return convert<c>().count() + seconds.count() / 60. +
           milli_seconds.count() / 1000000. +
           micro_seconds.count() / 1000000000. +
           nano_seconds.count() / 1000000000000.;
  }

  template <class c>
  typename std::enable_if<std::is_same<c, std::chrono::hours>::value,
                          double>::type
  toDouble() const {
    return convert<c>().count() + minutes.count() / 60. +
           seconds.count() / 3600. + milli_seconds.count() / 1000000000. +
           micro_seconds.count() / 1000000000000. +
           nano_seconds.count() / 1000000000000000.;
  }

  // todo overflow!!! if e.g microsseconds are at max
  template <class c>
  typename std::enable_if<std::is_same<c, std::chrono::nanoseconds>::value,
                          c>::type
  convert() const {
    return hours + minutes + seconds + milli_seconds + micro_seconds +
           nano_seconds;
  }

  template <class c>
  typename std::enable_if<std::is_same<c, std::chrono::microseconds>::value,
                          c>::type
  convert() const {
    return hours + minutes + seconds + milli_seconds + micro_seconds;
  }

  template <class c>
  typename std::enable_if<std::is_same<c, std::chrono::milliseconds>::value,
                          c>::type
  convert() const {
    return hours + minutes + seconds + milli_seconds;
  }

  template <class c>
  typename std::enable_if<std::is_same<c, std::chrono::seconds>::value, c>::type
  convert() const {
    return hours + minutes + seconds;
  }

  template <class c>
  typename std::enable_if<std::is_same<c, std::chrono::minutes>::value, c>::type
  convert() const {
    return hours + minutes;
  }

  template <class c>
  typename std::enable_if<std::is_same<c, std::chrono::hours>::value, c>::type
  convert() const {
    return hours;
  }

  template <class c>
  typename std::enable_if<std::is_same<c, std::chrono::nanoseconds>::value,
                          c>::type
  get() const {
    return nano_seconds;
  }

  template <class c>
  typename std::enable_if<std::is_same<c, std::chrono::microseconds>::value,
                          c>::type
  get() const {
    return micro_seconds;
  }

  template <class c>
  typename std::enable_if<std::is_same<c, std::chrono::milliseconds>::value,
                          c>::type
  get() const {
    return milli_seconds;
  }

  template <class c>
  typename std::enable_if<std::is_same<c, std::chrono::seconds>::value, c>::type
  get() const {
    return seconds;
  }

  template <class c>
  typename std::enable_if<std::is_same<c, std::chrono::minutes>::value, c>::type
  get() const {
    return minutes;
  }

  template <class c>
  typename std::enable_if<std::is_same<c, std::chrono::hours>::value, c>::type
  get() const {
    return hours;
  }

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

    // clang-format off
    os << "h: ["  << pt.hours.count()   << "] " << blanks(pt.hours.count())
       << "m: ["  << pt.minutes.count() << "] " << blanks(pt.minutes.count())
       << "s: ["  << pt.seconds.count() << "] " << blanks(pt.seconds.count())
       << "ms: [" << pt.milli_seconds.count() << "] " << blanks(pt.milli_seconds.count())
       << "us: [" << pt.micro_seconds.count() << "] " << blanks(pt.micro_seconds.count())
       << "ns: [" << pt.nano_seconds.count()  << "]";
    // clang-format on
    return os;
  }

  static const PreciseTime max() {
    PreciseTime ps(std::chrono::hours::max());
    ps.minutes = std::chrono::minutes(59);
    ps.seconds = std::chrono::seconds(59);
    ps.milli_seconds = std::chrono::milliseconds(999);
    ps.micro_seconds = std::chrono::microseconds(999);
    ps.nano_seconds = std::chrono::nanoseconds(999);
    return ps;
  }

  static const PreciseTime min() {
    PreciseTime ps(std::chrono::hours::min());
    ps.minutes = std::chrono::minutes(-59);
    ps.seconds = std::chrono::seconds(-59);
    ps.milli_seconds = std::chrono::milliseconds(-999);
    ps.micro_seconds = std::chrono::microseconds(-999);
    ps.nano_seconds = std::chrono::nanoseconds(-999);
    return ps;
  }

  static const PreciseTime zero() {
    const PreciseTime ps;
    return ps;
  }

private:
  void sanitize() {
    long carry = static_cast<long>(nano_seconds.count() / 1000.);
    micro_seconds += std::chrono::microseconds(carry);

    nano_seconds -= std::chrono::nanoseconds(1000 * carry);

    carry = static_cast<long>(micro_seconds.count() / 1000.);
    milli_seconds += std::chrono::milliseconds(carry);

    micro_seconds -= std::chrono::microseconds(1000 * carry);

    carry = static_cast<long>(milli_seconds.count() / 1000.);
    seconds += std::chrono::seconds(carry);

    milli_seconds -= std::chrono::milliseconds(1000 * carry);

    carry = static_cast<long>(seconds.count() / 60.);
    minutes += std::chrono::minutes(carry);

    seconds -= std::chrono::seconds(60 * carry);

    carry = static_cast<long>(minutes.count() / 60.);
    hours += std::chrono::hours(carry);

    minutes -= std::chrono::minutes(60 * carry);
  }

  std::chrono::nanoseconds nano_seconds = std::chrono::nanoseconds::zero();
  std::chrono::microseconds micro_seconds = std::chrono::microseconds::zero();
  std::chrono::milliseconds milli_seconds = std::chrono::milliseconds::zero();
  std::chrono::seconds seconds = std::chrono::seconds::zero();
  std::chrono::minutes minutes = std::chrono::minutes::zero();
  std::chrono::hours hours = std::chrono::hours::zero();
};

class Timer {
public:
  void start(const std::string &s = "") {
    const Timer::precisionClock::time_point start = precisionClock::now();
    begin_measurements[s] = start;
  }

  void stop(const std::string &s = "") {
    const Timer::precisionClock::time_point stop = precisionClock::now();
    const begin_measurements_it start = begin_measurements.find(s);
    if (start == begin_measurements.end()) {
      // TODO debugMsg: The timer with name s was never started
      return;
    }

    const std::chrono::nanoseconds duration =
        std::chrono::duration_cast<std::chrono::nanoseconds>(stop -
                                                             start->second);
    measurements[s].emplace_back(duration);

    result_needs_update = true;
  }

  struct Result {
    PreciseTime min_measurement = PreciseTime::max();
    PreciseTime max_measurement = PreciseTime::min();
    PreciseTime average;
    PreciseTime standard_derivation;
    int number_measurements;

    friend std::ostream &operator<<(std::ostream &os, const Result &r) {

      os << "###Timer Result###" << std::endl
         << "E{X}: \t" << r.average << std::endl
         << "Max{X}: " << r.max_measurement << std::endl
         << "Min{X}: " << r.min_measurement << std::endl
         << "D{X}: \t" << r.standard_derivation << std::endl
         << "N: \t" << r.number_measurements << std::endl;
      return os;
    }
  };

  const Result &getResult() {

    if (!result_needs_update) {
      return result;
    }

    result.number_measurements = 0;
    PreciseTime sum;
    // mean, min and max
    for (const auto &timer : measurements) {
      for (const auto &measurement : timer.second) {
        if (measurement > result.max_measurement) {
          result.max_measurement = measurement;
        }
        if (measurement < result.min_measurement) {
          result.min_measurement = measurement;
        }
        sum += measurement;
        result.number_measurements++;
      }
    }

    result.average = sum / result.number_measurements;

    const double average_ms =
        result.average.toDouble<std::chrono::nanoseconds>();
    // variance
    double var_sum = 0.;
    for (const auto &timer : measurements) {
      for (const auto &measurement : timer.second) {
        const double diff =
            measurement.toDouble<std::chrono::nanoseconds>() - average_ms;

        // avoid possible overflow when calculateing diff*diff: use PreciseTime
        var_sum += diff * diff;
      }
    }

    const double std_derivation =
        std::sqrt(var_sum / result.number_measurements);
    result.standard_derivation =
        std::chrono::nanoseconds(static_cast<int>(std_derivation));

    result_needs_update = false;
    return result;
  }

  friend std::ostream &operator<<(std::ostream &os, Timer &t) {
    const Result r = t.getResult();
    os << r;
    return os;
  }

private:
  typedef std::chrono::high_resolution_clock precisionClock;

  std::map<std::string, Timer::precisionClock::time_point> begin_measurements;
  typedef std::map<std::string, Timer::precisionClock::time_point>::iterator
      begin_measurements_it;
  std::map<std::string, std::vector<PreciseTime>> measurements;

  Result result;
  bool result_needs_update = true;
};

} // namespace tool

#endif
