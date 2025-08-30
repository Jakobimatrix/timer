/**
 * @file scoped_timer.hpp
 * @brief Implements a scoped timer wich will stop automatically on destruction
 * and reports via callback function.
 * @date 12.11.2021
 * @author Jakob Wandel
 * @version 1.0
 **/

#ifndef SCOPED_TIMER_H
#define SCOPED_TIMER_H

#include "precise_time.hpp"
#include <functional>

/*!
 * @brief A Scoped timer. It will start recording on creation and stop recording
 * on destruction. The recorded time will be reported via a given callback
 * function.
 */
class ScopedTimer {
 public:
  using reportBack =
    std::function<void(const std::string&, const PreciseTime::PrecisionClock::time_point&, const PreciseTime& time)>;

  /*!
   * @brief Constructor, Starts timer.
   * @param name The name of the timer.
   * @param report_back_callback A callbackfunction of Type
   * ScopedTimer::reportBack which will be called on destruction reporting the
   * time and the name.
   */
  ScopedTimer(const std::string& timer_name, const reportBack& report_back_callback)
      : name(timer_name),
        start(PreciseTime::PrecisionClock::now()),
        report_back(report_back_callback) {}

  /*!
   * @brief Constructor, Starts timer. WHen the timer ends it will automatically print the timeing via cout.
   * @param name The name of the timer.
   */
  ScopedTimer(const std::string& timer_name)
      : name(timer_name),
        start(PreciseTime::PrecisionClock::now()),
        report_back([](const std::string& name_,
                       const PreciseTime::PrecisionClock::time_point&,
                       const PreciseTime& time) {
          printf("Timer %s stopped after %s\n", name_.c_str(), time.toString().c_str());
        }) {}

  void stop() {
    if (stopped) {
      return;
    }
    stopped         = true;
    const auto stop = PreciseTime::PrecisionClock::now();
    const auto duration =
      std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);
    report_back(name, start, duration);
  }

  ~ScopedTimer() { stop(); }

 private:
  const std::string name;
  const PreciseTime::PrecisionClock::time_point start;
  const reportBack report_back;
  bool stopped = false;
};

#endif
