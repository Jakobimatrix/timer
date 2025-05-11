/*
 * \file: collecting_timer.hpp
 * \brief: Implements a timer which is able to run multiple named timers and
 * store multiple runns of these named timers. The results can be written into a
 * .csv file as a time series or as a histogram for further analysis. There is
 * also a rudimentary console logging function.
 * \date: 17.07.2021
 * \author: Jakob Wandel
 * \source: https://github.com/Jakobimatrix/timer
 * \version: 2.0
 */

#ifndef COLLECTING_TIMER_H
#define COLLECTING_TIMER_H

#include "precise_time.hpp"
#include <algorithm>
#include <fstream>
#include <iterator>
#include <map>
#include <vector>

class CollectingTimer {
public:
  using time_point = PreciseTime::PrecisionClock::time_point;
  CollectingTimer() = default;
  CollectingTimer(const std::vector<PreciseTime> &given_measurements,
                  const std::string label) {
    measurements[label] = given_measurements;
  }

  /*!
   * \brief start starts a new measurement.
   * \param s The name under which the measurement/timer shall be saved.
   */
  void start(const std::string &s = "") noexcept {
    const time_point start = precisionClock::now();
    begin_measurements[s] = start;
  }

  /*!
   * \brief Stops a started measurement, computes the time since start()
   * and saves it.
   * \param s The name under which the measurement/timer shall be saved.
   */
  void stop(const std::string &s = "") noexcept {
    const time_point stop = precisionClock::now();
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
      constexpr size_t MAX_LENGT = 50;
      return std::max(MAX_LENGT, num_char_terminal_width - BucketSizeInfo);
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
         << "E{X}: \t  " << r.mean << "\n"
         << "Median{X}:" << r.median << "\n"
         << "Max{X}:   " << r.max_measurement << "\n"
         << "Min{X}:   " << r.min_measurement << "\n"
         << "D{X}: \t  " << r.standard_derivation << "\n"
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

      Histogram temp_histogram;
      temp_histogram.initBuckets(min_bucket_size, histogram_start, histogram_end);

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

      for (const auto &bucket_comb_histo : temp_histogram.buckets) {

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
              static_cast<size_t>(static_cast<double>(nc.first) / smallest_unit);
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
          sum / static_cast<double>(result.number_measurements - result.number_outliners);
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
          var_sum / static_cast<double>(result.number_measurements - result.number_outliners - 1);
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
  friend std::ostream &operator<<(std::ostream &os, CollectingTimer &t) {
    for (const auto &timer : t.measurements) {
      Result r;
      t.getResult(timer.first, r);
      os << "Timer: " << timer.first << std::endl << r << "\n";
    }
    return os;
  }

  /*!
   * \brief Writes all measurements from all timers into the given file
   * (appends) for further analysis with Excel or Matlab.
   * \template a std::chrono duration in which the time (as double values)
   * should be printed.
   * \param file_name The name of the file to write into. If its a path, the
   * path must exist.
   * \param seperator A character to seperate the input fields.
   * \return true if writeing was successfull.
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
   * given file (appends) for further analysis with Excel or Matlab.
   * \template a std::chrono duration in which the time (as double values)
   * should be printed.
   * \param file_name The name of the file to write into. If its a path, the
   * path must exist.
   * \param seperator A character to seperate the input fields.
   * \return true if writeing was successfull.
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
  PreciseTime findMedian(std::vector<PreciseTime> &values) noexcept {
    // https://www.geeksforgeeks.org/finding-median-of-unsorted-array-in-linear-time-using-c-stl/
    const long int n = static_cast<long int>(values.size());

    // If size of the arr[] is even
    if (n % 2 == 0) {

      // Applying nth_element
      // on n/2th index
      std::nth_element(values.begin(), values.begin() + n / 2, values.end());

      // Applying nth_element
      // on (n-1)/2 th index
      std::nth_element(values.begin(), values.begin() + (n - 1) / 2, values.end());

      // Find the average of value at
      // index N/2 and (N-1)/2
      const size_t right_mid_index = static_cast<size_t>(n / 2);
      const size_t left_mid_index = right_mid_index - 1;
      return (values[left_mid_index] + values[right_mid_index]) / 2.0;
    }

    // If size of the arr[] is odd
    else {

      // Applying nth_element
      // on n/2
      nth_element(values.begin(), values.begin() + n / 2, values.end());

      // Value at index (N/2)th
      // is the median
      const size_t mid_index = static_cast<size_t>(n / 2);
      return values[mid_index];
    }
  }

  PreciseTime findMedianCopy(std::vector<PreciseTime> values) {
    return findMedian(values);
  }

  typedef std::conditional<std::chrono::high_resolution_clock::is_steady,
                           std::chrono::high_resolution_clock,
                           std::chrono::steady_clock>::type precisionClock;

  std::map<std::string, time_point> begin_measurements;
  typedef std::map<std::string, time_point>::iterator begin_measurements_it;
  std::map<std::string, std::vector<PreciseTime>> measurements;
};

#endif
