#ifndef TIMEUTIL_H
#define TIMEUTIL_H

#include <stdint.h>
#include <sys/time.h>
#include <time.h>

#include <chrono>

class TimeUtil {
 private:
  using Clock = std::chrono::steady_clock;
  using Seconds = std::chrono::seconds;
  using Milliseconds = std::chrono::milliseconds;
  using Microseconds = std::chrono::microseconds;
  using Nanoseconds = std::chrono::nanoseconds;
  using TimePoint = std::chrono::time_point<Clock, Seconds>;
  using MilliTimePoint = std::chrono::time_point<Clock, Milliseconds>;
  using MicroTimePoint = std::chrono::time_point<Clock, Microseconds>;
  using NanoTimePoint = std::chrono::time_point<Clock, Nanoseconds>;
#define TimePointCast(duration) std::chrono::time_point_cast<Seconds>(duration)
#define MilliTimePointCast(duration) \
  std::chrono::time_point_cast<Milliseconds>(duration)
#define MicroTimePointCast(duration) \
  std::chrono::time_point_cast<Microseconds>(duration)
#define NanoTimePointCast(duration) \
  std::chrono::time_point_cast<Nanoseconds>(duration)
#define DurationCast(duration) std::chrono::duration_cast<Seconds>(duration)
#define MilliDurationCast(duration) \
  std::chrono::duration_cast<Milliseconds>(duration)
#define MicroDurationCast(duration) \
  std::chrono::duration_cast<Microseconds>(duration)
#define NanoDurationCast(duration) \
  std::chrono::duration_cast<Nanoseconds>(duration)

 public:
  static int64_t CurrentTimeInNanoseconds() {
    return NanoDurationCast(NanoTimePointCast(Clock::now()).time_since_epoch())
        .count();
  }

  static int64_t CurrentTimeInMicroseconds() {
    return MicroDurationCast(
        MicroTimePointCast(Clock::now()).time_since_epoch())
        .count();
  }

  static int64_t CurrentTimeInMilliseconds() {
    return MilliDurationCast(
        MilliTimePointCast(Clock::now()).time_since_epoch())
        .count();
  }

  static int64_t CurrentTimeInSeconds() {
    return DurationCast(TimePointCast(Clock::now()).time_since_epoch()).count();
  }

  static int Year(time_t time = CurrentTimeInSeconds()) {
    struct tm *t = localtime(&time);
    return t->tm_year;
  }

  static int Month(time_t time = CurrentTimeInSeconds()) {
    struct tm *t = localtime(&time);
    return t->tm_mon;
  }

  static int MonthDay(time_t time = CurrentTimeInSeconds()) {
    struct tm *t = localtime(&time);
    return t->tm_mday;
  }

  static int WeekDay(time_t time = CurrentTimeInSeconds()) {
    struct tm *t = localtime(&time);
    return t->tm_wday;
  }

  static int Hour(time_t time = CurrentTimeInSeconds()) {
    struct tm *t = localtime(&time);
    return t->tm_hour;
  }

  static int Minute(time_t time = CurrentTimeInSeconds()) {
    struct tm *t = localtime(&time);
    return t->tm_min;
  }

  static int Second(time_t time = CurrentTimeInSeconds()) {
    struct tm *t = localtime(&time);
    return t->tm_sec;
  }
};

#endif  // TIMEUTIL_H
