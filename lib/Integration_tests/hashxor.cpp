#include <integration_tests.h>
#include <SHA3.h>

TestableHashXor::TestableHashXor() {
  ResetInitial();
  ResetRnd();
}

void TestableHashXor::ResetInitial() {
  RNG.loop();
  RNG.rand(cluster_identity, 16);
  RNG.rand(ip, 3);
  RNG.rand(identity, 8);
}
void TestableHashXor::ResetRnd() {
  RNG.rand(random_num1, 16);
  RNG.rand(random_num2, 16);
}

bool TestableHashXor::TestingPrimitive() {
  SHA256 hash1;
  SHA3_256 hash2;
  byte b_1[32], r_f[32], c_1[3], a_1[16], a_2[8], b_2[32];
  hash1.update(random_num1, 16);
  hash1.finalize(b_1, 16);
  hash1.reset();
  hash1.update(random_num2, 16);
  hash1.finalize(b_2, 16);
  hash2.update(random_num1, 16);
  hash2.finalize(r_f, 16);
  XorBytes(random_num1, cluster_identity, a_1, 16);
  XorBytes(r_f, ip, c_1, sizeof(ip));
  XorBytes(random_num2, identity, a_2, 8);
  return true;
}

float TestableHashXor::TimingPrimitive(SerialLogWriter *logger, size_t samples,
                                       bool verbose_sampling) {
  long start, end, execution_time;
  float avg;
  long total = 0;
  SHA256 hash1;
  SHA3_256 hash2;
  if (verbose_sampling) {
    PrintSimpleHeader(logger, "hashxor", samples);
    PrintSimpleColumns(logger);
  }
  for (int i = 0; i < samples; i++) {
    ResetInitial();
    start = micros();
    ResetRnd();
    byte b_1[32], r_f[32], c_1[3], a_1[16], a_2[8], b_2[32];
    hash1.update(random_num1, 16);
    hash1.finalize(b_1, 16);
    hash1.reset();
    hash1.update(random_num2, 16);
    hash1.finalize(b_2, 16);
    hash2.update(random_num1, 16);
    hash2.finalize(r_f, 16);
    XorBytes(random_num1, cluster_identity, a_1, 16);
    XorBytes(r_f, ip, c_1, sizeof(ip));
    XorBytes(random_num2, identity, a_2, 8);
    end = micros();
    hash1.clear();
    hash2.clear();
    execution_time = end - start;
    total += execution_time;
    if (verbose_sampling) {
      PrintSimpleRow(logger, i + 1, execution_time);
    }
  }
  avg = total / (float)samples;
  if (!verbose_sampling) {
    PrintSimpleAvg(logger, "hashxor", avg);
  }
  return avg;
}