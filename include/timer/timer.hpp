#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <functional>
#include <iostream>
#include <map>
#include <math.h>
#include <string>
#include <vector>

namespace tool {

template <class t> auto mu2ns(t micro) { return static_cast<t>(micro * 1000.); }

template <class t> auto ms2ns(t milli) {
  return static_cast<t>(milli * 1000000.);
}

template <class t> auto s2ns(t sec) {
  return static_cast<t>(sec * 1000000000.);
}

template <class t> auto ns2us(t ns) { return static_cast<t>(ns / 1000.); }

template <class t> auto us2ns(t us) { return static_cast<t>(us * 1000.); }

template <class t> auto ns2ms(t ns) { return static_cast<t>(ns / 1000000.); }

template <class t> auto ns2s(t ns) { return static_cast<t>(ns / 1000000000.); }

template <class t> auto ns2m(t ns) {
  return static_cast<t>(ns / (1000000000. * 60.));
}

template <class t> auto ns2h(t ns) {
  return static_cast<t>(ns / (1000000000. * 3600.));
}

template <class t> auto m2s(t m) { return static_cast<t>(m * 60.); }

template <class t> auto h2s(t h) { return static_cast<t>(h * 3600.); }

template <class t> auto h2m(t h) { return static_cast<t>(h * 60.); }

template <class t> auto h2ms(t h) { return static_cast<t>(h * 3600. * 1000.); }

template <class t> auto h2us(t h) {
  return static_cast<t>(h * 3600. * 1000000.);
}

template <class t> auto h2ns(t h) {
  return static_cast<t>(h * 3600. * 1000000000.);
}

template <class t> auto s2us(t s) { return static_cast<t>(s * 1000000.); }

template <class t> auto s2ms(t s) { return static_cast<t>(s * 1000.); }

template <class t> auto s2m(t s) { return static_cast<t>(s / 60.); }

template <class t> auto s2h(t s) { return static_cast<t>(s / 3600.); }

template <class t> auto s2hf(t s) {
  return static_cast<t>(std::floor(s / 3600.));
}

class PreciseTime {
public:
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

  PreciseTime operator+(const PreciseTime &pt) const {
    PreciseTime ret(pt);
    ret.nano_seconds += nano_seconds;
    ret.seconds += seconds;
    ret.hours += hours;

    ret.sanitize();
    return ret;
  }

  void operator+=(const PreciseTime &pt) {
    nano_seconds += pt.nano_seconds;
    seconds += pt.seconds;
    hours += pt.hours;

    sanitize();
  }

  PreciseTime operator-(const PreciseTime &pt) const {
    PreciseTime ret(pt);
    ret.nano_seconds -= nano_seconds;
    ret.seconds -= seconds;
    ret.hours -= hours;

    ret.sanitize();
    return ret;
  }

  void operator-=(const PreciseTime &pt) {
    nano_seconds -= pt.nano_seconds;
    seconds -= pt.seconds;
    hours -= pt.hours;

    sanitize();
  }

  PreciseTime operator*(const double multi) const {
    const double hours = this->hours.count() * multi;
    double remaining_hours;
    const double seconds_rest = h2s(std::modf(hours, &remaining_hours));

    const double seconds = this->seconds.count() * multi + seconds_rest;
    double remaining_seconds;
    const double nanoseconds_rest =
        s2ns(std::modf(seconds, &remaining_seconds));

    const double nanoseconds =
        this->nano_seconds.count() * multi + nanoseconds_rest;

    PreciseTime ret;
    ret.nano_seconds =
        std::chrono::nanoseconds(static_cast<long>(std::round(nanoseconds)));
    ret.seconds =
        std::chrono::seconds(static_cast<long>(std::round(remaining_seconds)));
    ret.hours =
        std::chrono::hours(static_cast<long>(std::round(remaining_hours)));

    ret.sanitize();
    return ret;
  }

  void operator*=(const double multi) {
    const double hours = this->hours.count() * multi;
    double remaining_hours;
    const double seconds_rest = h2s(std::modf(hours, &remaining_hours));

    const double seconds = this->seconds.count() * multi + seconds_rest;
    double remaining_seconds;
    const double nanoseconds_rest =
        s2ns(std::modf(seconds, &remaining_seconds));

    const double nanoseconds =
        this->nano_seconds.count() * multi + nanoseconds_rest;

    this->nano_seconds =
        std::chrono::nanoseconds(static_cast<long>(std::round(nanoseconds)));
    this->seconds =
        std::chrono::seconds(static_cast<long>(std::round(remaining_seconds)));
    this->hours =
        std::chrono::hours(static_cast<long>(std::round(remaining_hours)));

    sanitize();
  }

  PreciseTime operator/(const double div) const { return *this * (1. / div); }

  void operator/=(const double div) { *this *= (1. / div); }

  bool operator==(const PreciseTime &pt) const {
    return pt.nano_seconds == nano_seconds && pt.seconds == seconds &&
           pt.hours == hours;
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
    return h2ns(static_cast<double>(hours.count())) +
           s2ns(static_cast<double>(seconds.count())) +
           static_cast<double>(nano_seconds.count());
  }

  template <class c>
  typename std::enable_if<std::is_same<c, std::chrono::milliseconds>::value,
                          double>::type
  toDouble() const {
    return h2ms(static_cast<double>(hours.count())) +
           s2ms(static_cast<double>(seconds.count())) +
           ns2ms(static_cast<double>(nano_seconds.count()));
  }

  template <class c>
  typename std::enable_if<std::is_same<c, std::chrono::seconds>::value,
                          double>::type
  toDouble() const {
    return h2s(static_cast<double>(hours.count())) +
           static_cast<double>(seconds.count()) +
           ns2s(static_cast<double>(nano_seconds.count()));
  }

  template <class c>
  typename std::enable_if<std::is_same<c, std::chrono::minutes>::value,
                          double>::type
  toDouble() const {
    return h2m(static_cast<double>(hours.count())) +
           s2m(static_cast<double>(seconds.count())) +
           ns2m(static_cast<double>(nano_seconds.count()));
  }

  template <class c>
  typename std::enable_if<std::is_same<c, std::chrono::hours>::value,
                          double>::type
  toDouble() const {
    return static_cast<double>(hours.count()) +
           s2h(static_cast<double>(seconds.count())) +
           ns2h(static_cast<double>(nano_seconds.count()));
  }

  // todo overflow!!! if e.g microsseconds are at max
  template <class c>
  typename std::enable_if<std::is_same<c, std::chrono::nanoseconds>::value,
                          c>::type
  convert() const {
    return std::chrono::nanoseconds(
        h2ns(hours.count()) + s2ns(seconds.count()) + nano_seconds.count());
  }

  template <class c>
  typename std::enable_if<std::is_same<c, std::chrono::microseconds>::value,
                          c>::type
  convert() const {
    return std::chrono::microseconds(h2us(hours.count()) +
                                     s2us(seconds.count()) +
                                     ns2us(nano_seconds.count()));
  }

  template <class c>
  typename std::enable_if<std::is_same<c, std::chrono::milliseconds>::value,
                          c>::type
  convert() const {
    return std::chrono::milliseconds(h2ms(hours.count()) +
                                     s2ms(seconds.count()) +
                                     ns2ms(nano_seconds.count()));
  }

  // preciseTime = 2h,3m,44s,40ms,66us,12ns > get nanoseconds will return
  // 2*3600s+3*60s+44s
  template <class c>
  typename std::enable_if<std::is_same<c, std::chrono::seconds>::value, c>::type
  convert() const {
    return std::chrono::seconds(h2s(hours.count())) + seconds;
  }

  // preciseTime = 2h,3m,44s,40ms,66us,12ns > get nanoseconds will return
  // 2*60m+3m
  template <class c>
  typename std::enable_if<std::is_same<c, std::chrono::minutes>::value, c>::type
  convert() const {
    return std::chrono::minutes(s2m(seconds.count() + h2m(hours.count())));
    ;
  }

  // preciseTime = 2h,3m,44s,40ms,66us,12ns > get nanoseconds will return 2h
  template <class c>
  typename std::enable_if<std::is_same<c, std::chrono::hours>::value, c>::type
  convert() const {
    return hours;
  }

  // preciseTime = 2h,3m,44s,40ms,66us,12ns > get nanoseconds will return 12ns
  template <class c>
  typename std::enable_if<std::is_same<c, std::chrono::nanoseconds>::value,
                          c>::type
  get() const {
    const long ns = nano_seconds.count();
    if (std::abs(ns) > 999) {
      double us;
      const double ns_rest = std::modf(ns / 1000., &us) * 1000.;
      return std::chrono::nanoseconds(static_cast<long>(std::round(ns_rest)));
    }
    return nano_seconds;
  }

  // preciseTime = 2h,3m,44s,40ms,66us,12ns > get microseconds will return 66su
  template <class c>
  typename std::enable_if<std::is_same<c, std::chrono::microseconds>::value,
                          c>::type
  get() const {
    const long us =
        ns2us(nano_seconds.count() - get<std::chrono::nanoseconds>().count());
    if (std::abs(us) > 999) {
      double ms;
      const double us_rest = std::modf(us / 1000., &ms) * 1000.;
      return std::chrono::microseconds(static_cast<long>(std::round(us_rest)));
    }
    return std::chrono::microseconds(us);
  }

  // preciseTime = 2h,3m,44s,40ms,66us,12ns > get milliseconds will return 40ms
  template <class c>
  typename std::enable_if<std::is_same<c, std::chrono::milliseconds>::value,
                          c>::type
  get() const {
    const long ms =
        ns2ms(nano_seconds.count() - get<std::chrono::microseconds>().count());
    if (std::abs(ms) > 999) {
      double s;
      const double ms_rest = std::modf(ms / 1000., &s) * 1000.;
      return std::chrono::milliseconds(static_cast<long>(std::round(ms_rest)));
    }
    return std::chrono::milliseconds(ms);
  }

  // preciseTime = 2h,3m,44s,40ms,66us,12ns > get seconds will return 44s
  template <class c>
  typename std::enable_if<std::is_same<c, std::chrono::seconds>::value, c>::type
  get() const {
    const long sec = seconds.count();
    if (std::abs(sec) > 59) {
      double m;
      const double s_rest = std::modf(sec / 60., &m) * 60.;
      return std::chrono::seconds(static_cast<long>(std::round(s_rest)));
    }
    return seconds;
  }

  // preciseTime = 2h,3m,44s,40ms,66us,12ns > get munutes will return 3m
  template <class c>
  typename std::enable_if<std::is_same<c, std::chrono::minutes>::value, c>::type
  get() const {
    const long m = s2m(seconds.count() - get<std::chrono::seconds>().count());
    if (std::abs(m) > 59) {
      double h;
      const double m_rest = std::modf(m / 60., &h) * 60.;
      return std::chrono::minutes(static_cast<long>(std::round(m_rest)));
    }
    return std::chrono::minutes(m);
  }

  // preciseTime = 2h,3m,44s,40ms,66us,12ns > get munutes will return 2h
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

    const long hours = pt.get<std::chrono::hours>().count();
    const long minutes = pt.get<std::chrono::minutes>().count();
    const long seconds = pt.get<std::chrono::seconds>().count();
    const long ms = pt.get<std::chrono::milliseconds>().count();
    const long us = pt.get<std::chrono::microseconds>().count();
    const long ns = pt.get<std::chrono::nanoseconds>().count();

    // clang-format off
    os << "h: ["  << hours   << "] " << blanks(hours)
       << "m: ["  << minutes << "] " << blanks(minutes)
       << "s: ["  << seconds << "] " << blanks(seconds)
       << "ms: [" << ms << "] " << blanks(ms)
       << "us: [" << us << "] " << blanks(us)
       << "ns: [" << ns  << "]";
    // clang-format on
    return os;
  }

  static const PreciseTime max() {
    PreciseTime ps(std::chrono::hours::max());
    ps.seconds = MAX_VALIDE_S;
    ps.nano_seconds = MAX_VALIDE_NS;
    return ps;
  }

  static const PreciseTime min() {
    PreciseTime ps(std::chrono::hours::min());
    ps.seconds = MIN_VALIDE_S;
    ps.nano_seconds = MIN_VALIDE_NS;
    return ps;
  }

  static const PreciseTime zero() {
    const PreciseTime ps;
    return ps;
  }

private:
  void sanitizeNS() {
    if (nano_seconds > MAX_VALIDE_NS || nano_seconds < MIN_VALIDE_NS) {
      double seconds_from_nanoseconds = ns2s(nano_seconds.count());
      double carry_seconds;
      std::modf(seconds_from_nanoseconds, &carry_seconds);
      const long carry_seconds_l = static_cast<long>(std::round(carry_seconds));

      // substract the nanoseconds which made up a second
      nano_seconds = std::chrono::nanoseconds(nano_seconds.count() -
                                              s2ns(carry_seconds_l));
      // add them to the seconds
      seconds += std::chrono::seconds(carry_seconds_l);
    }
  }

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
        return; // we dont need to worry about different signs
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

  void sanitize() {

    sanitizeNS();
    sanitizeS();
    sanitizeSign();
  }

  static constexpr std::chrono::seconds MAX_VALIDE_S =
      std::chrono::seconds(3599);
  static constexpr std::chrono::seconds MIN_VALIDE_S =
      std::chrono::seconds(-3599);
  static constexpr std::chrono::nanoseconds MAX_VALIDE_NS =
      std::chrono::nanoseconds(999999999);
  static constexpr std::chrono::nanoseconds MIN_VALIDE_NS =
      std::chrono::nanoseconds(-999999999);

  std::chrono::nanoseconds nano_seconds = std::chrono::nanoseconds::zero();
  std::chrono::seconds seconds = std::chrono::seconds::zero();
  std::chrono::hours hours = std::chrono::hours::zero();
};

class Timer {
public:
  ~Timer() { std::cout << *this << std::endl; }

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

    results[s].needs_update = true;
  }

  struct Result {
    PreciseTime min_measurement = PreciseTime::max();
    PreciseTime max_measurement = PreciseTime::min();
    PreciseTime average;
    PreciseTime standard_derivation;
    int number_measurements;
    bool needs_update = true;

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

    const double average_ms =
        results[name].average.toDouble<std::chrono::nanoseconds>();
    // variance
    double var_sum = 0.;
    for (const auto &measurement : timer->second) {
      const double diff =
          measurement.toDouble<std::chrono::nanoseconds>() - average_ms;

      // avoid possible overflow when calculateing diff*diff: use PreciseTime
      var_sum += diff * diff;
    }

    const double std_derivation =
        std::sqrt(var_sum / results[name].number_measurements);
    results[name].standard_derivation =
        std::chrono::nanoseconds(static_cast<int>(std_derivation));

    results[name].needs_update = false;
    return results[name];
  }

  friend std::ostream &operator<<(std::ostream &os, Timer &t) {
    for (const auto &timer : t.measurements) {
      os << "Timer: " << timer.first << std::endl
         << t.getResult(timer.first) << std::endl;
    }
    return os;
  }

private:
  typedef std::chrono::high_resolution_clock precisionClock;

  std::map<std::string, Timer::precisionClock::time_point> begin_measurements;
  typedef std::map<std::string, Timer::precisionClock::time_point>::iterator
      begin_measurements_it;
  std::map<std::string, std::vector<PreciseTime>> measurements;

  std::map<std::string, Result> results;
};

} // namespace tool

#endif
