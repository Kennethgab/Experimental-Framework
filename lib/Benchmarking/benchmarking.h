#ifndef BENCHMARKING_h
#define BENCHMARKING_h
#include <Arduino.h>
#include <SerialLogWriter.h>

/**
 * \brief simple pointer to a function to be executed.
 *
 */
typedef void (*func_ptr)();

class SimpleBenchmarker
{
private:
  bool verbose_sampling;

public:
  SerialLogWriter logger;
  size_t samples;
  SimpleBenchmarker();
  SimpleBenchmarker(SerialLogWriter logger, bool verbose_sampling,
                    size_t samples);
  float BenchmarkTotal(char *log_type, func_ptr benchmarked_func,
                       func_ptr setup, func_ptr setup_loop, func_ptr teardown);
  float Benchmark(char *log_type, func_ptr benchmarked_func);
  void LogOnlyAverage(bool enabled, char *log_type);
  void Finalize() { Serial.println(">done"); }
};

#endif