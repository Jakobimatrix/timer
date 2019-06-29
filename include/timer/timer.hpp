#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <math.h>

namespace tool{


class Time{
public:
  int hours;
  int minutes;
  int seconds;
  int milliseconds;
  int microseconds;
  int nanoseconds;
};

class Timer{
public:

  void start(const std::string& s = ""){
    const Timer::precisionClock::time_point start = precisionClock::now();
    begin_measurements[s] = start;
  }

  void stop(const std::string& s = ""){
    const Timer::precisionClock::time_point stop = precisionClock::now();
    const begin_measurements_it start = begin_measurements.find(s);
    if(start == begin_measurements.end()){
      // TODO debugMsg: The timer with name s was never started
      return;
    }

    const std::chrono::nanoseconds duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start->second);
    measurements[s].emplace_back(duration);

    result_needs_update = true;
  }

  struct Result{
    std::chrono::nanoseconds min_measurement = std::chrono::nanoseconds::max();
    std::chrono::nanoseconds max_measurement = std::chrono::nanoseconds::min();
    std::chrono::nanoseconds average;
    std::chrono::nanoseconds variance;
    int number_measurements;

    friend std::ostream &operator<<(std::ostream &os, const Result &r) {

      os << "###Timer Result###" << std::endl
         << "E{X}: \t" << r.average.count() << "ns" << std::endl
         << "Max{X}: " << r.max_measurement.count() << "ns" << std::endl
         << "Min{X}: " << r.min_measurement.count() << "ns" << std::endl
         << "D{X}: \t" << std::sqrt(r.variance.count()) << "ns" << std::endl
         << "N: \t" << r.number_measurements << std::endl;
      return os;
    }
  };

  const Result& getResult(){
    if(!result_needs_update){
      return result;
    }

    result.number_measurements = 0;
    std::chrono::nanoseconds sum = std::chrono::nanoseconds::zero();
    //mean, min and max
    for(const auto& timer : measurements){
      for(const auto& measurement : timer.second){
        if(measurement > result.max_measurement){
          result.max_measurement = measurement;
        }
        if(measurement < result.min_measurement){
          result.min_measurement = measurement;
        }
        sum += measurement;
        result.number_measurements++;
      }
    }

    result.average = sum/result.number_measurements;

    // variance
    sum = std::chrono::nanoseconds::zero();
    for(const auto& timer : measurements){
      for(const auto& measurement : timer.second){
        const std::chrono::nanoseconds diff = measurement - result.average;
        sum += diff*diff.count();
      }
    }

    result.variance = sum / result.number_measurements;
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

  std::map<std::string,Timer::precisionClock::time_point> begin_measurements;
  typedef std::map<std::string,Timer::precisionClock::time_point>::iterator begin_measurements_it;
  std::map<std::string, std::vector<std::chrono::nanoseconds>> measurements;

  Result result;
  bool result_needs_update = true;
};

} //namespace tool

#endif
