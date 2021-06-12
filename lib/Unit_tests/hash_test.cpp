#include <Arduino.h>
#include <utility.h>
#include <RNG.h>
#include <unit_tests.h>
#include <Hash.h>

TestableHashing::TestableHashing(char *log_type, Hash *hash, size_t input_size,
                                 size_t output_size, char *input,
                                 char *expected_hash_hex) {
  this->hash              = hash;
  this->input_size        = input_size;
  this->output_size       = output_size;
  this->input             = input;
  this->expected_hash_hex = expected_hash_hex;
  this->log_type          = log_type;
}

bool TestableHashing::TestingPrimitive() {
  byte expected_hash_buffer[output_size];
  HexToBytes(expected_hash_hex, expected_hash_buffer, output_size);
  byte hash_buffer[output_size];
  hash->update(input, strlen(input));
  hash->finalize(hash_buffer, output_size);
  hash->clear();
  return EqualBuffer(hash_buffer, expected_hash_buffer, output_size);
}

float TestableHashing::TimingPrimitive(SerialLogWriter *logger, size_t samples,
                                       bool verbose_sampling) {
  byte random_buffer[input_size];
  long start, end, execution_time;
  float avg;
  if (verbose_sampling) {
    PrintSimpleHeader(logger, log_type, samples);
    PrintSimpleColumns(logger);
  }
  long total = 0;
  for (size_t i = 0; i < samples; i++) {
    RNG.loop();
    RNG.rand(random_buffer, input_size);
    start = micros();
    byte hash_buffer[32];
    hash->update(random_buffer, 32);
    hash->finalize(hash_buffer, 32);
    end = micros();
    hash->clear();
    execution_time = end - start;
    total += execution_time;
    if (verbose_sampling) {
      PrintSimpleRow(logger, i + 1, execution_time);
    }
  }
  avg = total / (float)samples;
  if (!verbose_sampling) {
    PrintSimpleAvg(logger, log_type, avg);
  }
  return avg;
}
