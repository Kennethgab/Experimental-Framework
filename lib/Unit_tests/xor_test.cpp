
#include <Arduino.h>
#include <RNG.h>
#include <utility.h>
#include <unit_tests.h>

TestableXOR::TestableXOR(size_t buffer_size) { this->buffer_size = buffer_size; }

bool TestableXOR::TestingPrimitive() {
  const char* hex_1   = "c0535e4be2b79ffd93291305436bf889314e4a3faec05ecffcbb7df31ad9e51a";
  const char* hex_2   = "d6ea8f9a1f22e1298e5a9506bd066f23cc56001f5d36582344a628649df53ae8";
  const char* hex_xor = "16b9d1d1fd957ed41d738603fe6d97aafd184a20f3f606ecb81d5597872cdff2";
  byte hex_1_buffer[32];
  byte hex_2_buffer[32];
  byte hex_xor_buffer[32];
  byte xor_result_buffer[32];
  HexToBytes(hex_1, hex_1_buffer, 32);
  HexToBytes(hex_2, hex_2_buffer, 32);
  HexToBytes(hex_xor, hex_xor_buffer, 32);
  XorBytes(hex_1_buffer, hex_2_buffer, xor_result_buffer, 32);
  return EqualBuffer(hex_xor_buffer, xor_result_buffer, 32);
}

float TestableXOR::TimingPrimitive() {
  byte random_buffer1[buffer_size];
  byte random_buffer2[buffer_size];
  long start, end, execution_time;
  float avg;
  long total = 0;
  for (int i = 0; i < samples; i++) {
    RNG.rand(random_buffer1, buffer_size);
    RNG.rand(random_buffer2, buffer_size);
    RNG.loop();
    start = micros();
    byte result_buffer[buffer_size];
    XorBytes(random_buffer1, random_buffer2, result_buffer, buffer_size);
    end            = micros();
    execution_time = end - start;
    total += execution_time;
    if (verbose_sampling) {
      FormatPrint("%-7d | %7d\n", i + 1, execution_time);
    }
  }
  avg = total / (float)samples;
  return avg;
}
