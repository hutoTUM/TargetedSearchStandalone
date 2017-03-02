#ifndef TIMIMG_H_
#define TIMIMG_H_

#include <stdint.h>
#include <sys/time.h>

static inline uint64_t getCurrentTimeInMicroSeconds() {
  struct timeval time;
  gettimeofday(&time, NULL);

  // microseconds = 1 000 000 * seconds = 1000 * milliseconds
  return time.tv_sec * 1000000 + time.tv_usec;
}

static inline uint64_t getCurrentTimeInMilliSeconds() {
  // milliseconds = microseconds / 1000
  // 0.5 for the rounding logic
  return (getCurrentTimeInMicroSeconds() / 1000.0 + 0.5);
}


#endif  // TIMIMG_H_
