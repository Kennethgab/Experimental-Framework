#include <Arduino.h>
#include <utility.h>
#include <RNG.h>
#include <unit_tests.h>
#include <Hash.h>

TestableHashing::TestableHashing(Hash *hash, size_t input_size, size_t output_size, char *input, char *expected_hash_hex) {
  this->hash              = hash;
  this->input_size        = input_size;
  this->output_size       = output_size;
  this->input             = input;
  this->expected_hash_hex = expected_hash_hex;
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

float TestableHashing::TimingPrimitive() {
  byte random_buffer[input_size];
  long start, end, execution_time;
  float avg;
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
      FormatPrint("%-7d | %7d\n", i + 1, execution_time);
    }
  }
  avg = total / (float)samples;
  return avg;
}
