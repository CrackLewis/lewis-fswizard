/**
 * @file util_time.hpp
 * @author CrackLewis (ghxx040406@163.com)
 * @brief
 * @version 0.1.0
 * @date 2023-04-30 12:43:27
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef UTIL_TIME_HPP_
#define UTIL_TIME_HPP_

#include <chrono>

class Time {
 public:
  static int stamp() {
    using namespace std::chrono;

    auto now = system_clock::now();
    nanoseconds nanosec = now.time_since_epoch();
    milliseconds millisec = duration_cast<milliseconds>(nanosec);

    return millisec.count() / 1000;
  }
};

#endif