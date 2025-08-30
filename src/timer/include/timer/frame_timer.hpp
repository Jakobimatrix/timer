/**
 * @file frame_timer.hpp
 * @brief Implements a timer that accumulates the timings of (same) named
 * timers within a Thread/Loop for each frame. Accumulated times will be  saved
 * and resetted after each fram/loopstart.
 * @date 12.11.2021
 * @author Jakob Wandel
 * @version 1.0
 **/

#ifndef FRAME_TIMER_H
#define FRAME_TIMER_H

#include "precise_time.hpp"
#include "scoped_timer.hpp"
#include <fstream>
#include <iterator>
#include <list>
#include <map>
#include <memory>

class FrameTimer {
 public:
  FrameTimer() {
    report_back = std::bind(&FrameTimer::reportBack,
                            this,
                            std::placeholders::_1,
                            std::placeholders::_2,
                            std::placeholders::_3);
  };

  /*!
   * @brief Must be called on each cycle start to reset the akkumulated timers.
   */
  template <bool debug_to_console = false>
  void frameStart() {
    frameStop();
    frame_start   = PreciseTime::PrecisionClock::now();
    frame_stopped = false;
  }

  template <bool debug_to_console = false>
  void frameStop() {
    if (frame_stopped) {
      return;
    }
    frame_stopped        = true;
    const auto frame_end = PreciseTime::PrecisionClock::now();
    if (!current_timers->empty()) {
      const std::chrono::nanoseconds duration =
        std::chrono::duration_cast<std::chrono::nanoseconds>(frame_end - frame_start);
      frame_records.emplace_back(std::make_pair(duration, std::move(current_timers)));
      current_timers = std::make_shared<TimerMap>();
      if constexpr (debug_to_console) {
        debug2Console();
      }
    }
  }

  /*!
   * @brief Start a scoped timer. The results/timings will be collected on
   * destruction automatically.
   * @return A ScopedTimer
   */
  [[nodiscard]] ScopedTimer startScopedTimer(const std::string& name) {
    return ScopedTimer(name, report_back);
  }

  /*!
   * @brief Writes all measurements from all timers into the given file
   * (appends) for further analysis with Excel or Matlab.
   * @tparam T a std::chrono duration in which the time (as double values)
   * should be printed.
   * @param file_name The name of the file to write into. If its a path, the
   * path must exist.
   * @param seperator A character to seperate the input fields.
   * @return true if writeing was successfull.
   */
  template <class T>
  bool measurementsToFile(const std::string& file_name, char seperator) {

    const size_t num_timers = frame_records.size();
    if (num_timers == 0) {
      return false;
    }

    std::ofstream file;
    file.open(file_name.c_str(), std::ios_base::app);
    if (file.bad()) {
      return false;
    }

    auto inputIntoFile = [&file](std::string& input_line) {
      input_line += "\n";
      std::copy(input_line.begin(), input_line.end(), std::ostream_iterator<char>(file));
      input_line = "";
    };

    // first we need all timer names, and when we are on it, save the highst
    // time todo mybe use percentage rather than the time
    std::map<std::string, std::pair<int, PreciseTime>> timers;

    int i = 0;
    for (const auto& m : frame_records) {
      for (const auto& t : *(m.second)) {
        const auto it = timers.find(t.first);
        if (it != timers.end()) {
          if (t.second.accumulation > it->second.second) {
            it->second.second = t.second.accumulation;
          }
        } else {
          timers.emplace(std::make_pair(t.first, std::make_pair(i, t.second.accumulation)));
        }
      }
      i++;
    }

    const auto unit = timeunit2String<T>();
    // Write the header, names are sorted alphabetically because of map
    std::string input_line = std::string("Frame ") + unit;
    for (const auto& t : timers) {
      input_line +=
        seperator + t.first + " " + unit + seperator + t.first + " %";
    }
    inputIntoFile(input_line);

    // the data
    i = 0;
    for (const auto& m : frame_records) {
      const double frame_time         = m.first.toDouble<T>();
      const double frame_time_percent = 100. / frame_time;
      input_line                      = std::to_string(frame_time);
      for (const auto& t : timers) {
        const auto it = (m.second)->find(t.first);
        if (it == (m.second)->end()) {
          // lol?
          continue;
        }
        const double function_time = it->second.accumulation.toDouble<T>();
        const double function_percent_frame_time = function_time * frame_time_percent;
        input_line += seperator + std::to_string(function_time) + seperator +
                      std::to_string(function_percent_frame_time);
      }
      inputIntoFile(input_line);
    }

    return file.bad();
  }

 private:
  /*!
   * @brief Callback for all Started ScopedTimers. Collects the results of them.
   * @param name The name of the Timer.
   * @param timing The result of the timer.
   */
  void reportBack(const std::string& name,
                  const PreciseTime::PrecisionClock::time_point& start,
                  const PreciseTime& timeing) {
    const auto timer = current_timers->find(name);
    if (timer != current_timers->end()) {
      timer->second.single_events.emplace_back(start, timeing);
      timer->second.accumulation += timeing;
    } else {
      TimedValues tv;
      tv.accumulation = timeing;
      tv.single_events.emplace_back(start, timeing);
      current_timers->emplace(name, tv);
    }
  }

  /*!
   * @brief From the last frame, print 3 longest running timers in % and frame
   * time.
   */
  void debug2Console() const {
    using ns = std::chrono::nanoseconds;
    std::pair<std::string, PreciseTime> longest("", PreciseTime::zero());
    std::pair<std::string, PreciseTime> second_longest = longest;
    std::pair<std::string, PreciseTime> third_longest  = longest;
    const auto last_frame_record = (--frame_records.end());

    for (const auto& t : *(last_frame_record->second)) {
      if (t.second.accumulation > longest.second) {
        std::swap(second_longest, third_longest);
        std::swap(longest, second_longest);
        longest = std::make_pair(t.first, t.second.accumulation);
      } else if (t.second.accumulation > second_longest.second) {
        std::swap(second_longest, third_longest);
        second_longest = std::make_pair(t.first, t.second.accumulation);
      } else if (t.second.accumulation > third_longest.second) {
        third_longest = std::make_pair(t.first, t.second.accumulation);
      }
    }

    const double f = 100. / last_frame_record->first.toDouble<ns>();
    const int p1 = static_cast<int>(std::round(longest.second.toDouble<ns>() * f));
    const int p2 =
      static_cast<int>(std::round(second_longest.second.toDouble<ns>() * f));
    const int p3 = static_cast<int>(std::round(third_longest.second.toDouble<ns>() * f));

    // max name length is 10 characters. See printf %-10s
    const auto rs = [](std::string& s) {
      if (s.length() > 10) {
        s[6] = '\0';  // Sometimes my genius… It's almost frightening.
      }
      return s.c_str();
    };

    const auto N = (last_frame_record->second)->size();
    if (N == 1) {
      printf("[%-10s \033[1m%2d%%\033[0m] %s\n",
             rs(longest.first),
             p1,
             last_frame_record->first.getTimeString(2).c_str());
    } else if (N == 2) {
      printf("[%-10s \033[1m%2d%%\033[0m || %-10s \033[1m%2d%%\033[0m] %s\n",
             rs(longest.first),
             p1,
             rs(second_longest.first),
             p2,
             last_frame_record->first.getTimeString(2).c_str());
    } else if (N > 2) {
      printf(
        "[%-10s \033[1m%2d%%\033[0m || %-10s \033[1m%2d%%\033[0m || %-10s "
        "\033[1m%2d%%\033[0m] %s\n",
        rs(longest.first),
        p1,
        rs(second_longest.first),
        p2,
        rs(third_longest.first),
        p3,
        last_frame_record->first.getTimeString(2).c_str());
    }
  }

  using time_point = PreciseTime::PrecisionClock::time_point;
  struct TimedValues {
    PreciseTime accumulation = PreciseTime::zero();
    std::vector<std::pair<time_point, PreciseTime>> single_events;
  };
  using NamedTimer = std::pair<std::string, TimedValues>;
  using TimerMap   = std::map<std::string, TimedValues>;
  std::shared_ptr<TimerMap> current_timers = std::make_shared<TimerMap>();
  std::list<std::pair<PreciseTime, std::shared_ptr<TimerMap>>> frame_records;
  time_point frame_start;
  bool frame_stopped = false;
  ScopedTimer::reportBack report_back;
};

#endif
