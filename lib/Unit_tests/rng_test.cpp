#include <Arduino.h>
#include <RNG.h>
#include <utility.h>
#include <unit_tests.h>

TestableRNG::TestableRNG(size_t buffer_size) {
  this->buffer_size = buffer_size;
}

float TestableRNG::TimingPrimitive(SerialLogWriter *logger, size_t samples,
                                   bool verbose_sampling) {
  long start, end, execution_time;
  float avg;
  long total = 0;
  if (verbose_sampling) {
    PrintSimpleHeader(logger, "rng_256", samples);
    PrintSimpleColumns(logger);
  }
  for (size_t i = 0; i < samples; i++) {
    RNG.loop();
    start = micros();
    byte random_buffer[buffer_size];
    RNG.rand(random_buffer, buffer_size);
    end            = micros();
    execution_time = end - start;
    total += execution_time;
    if (verbose_sampling) {
      PrintSimpleRow(logger, i + 1, execution_time);
    }
  }
  avg = total / (float)samples;
  if (!verbose_sampling) {
    PrintSimpleAvg(logger, "rng_256", avg);
  }
  return avg;
}

bool TestableRNG::TestingPrimitive() {
  uint8_t random_buffer[buffer_size];
  uint8_t zero_buffer[buffer_size];
  RNG.rand(random_buffer, buffer_size);
  return !EqualBuffer(random_buffer, zero_buffer, buffer_size);
}
