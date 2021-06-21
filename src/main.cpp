#include <Arduino.h>
#include <utility.h>
#include <benchmarking.h>
#include <primitive_tests.h>

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial)
    ;

  SimpleBenchmarker benchmarker;
  //float avg = benchmarker.BenchmarkTotal("xor_256", primitive_xor, NULL, setup_xor_loop, NULL);
  float avg = benchmarker.Benchmark("rng_256", random_val);
  benchmarker.Finalize();
}

void loop()
{
  // put your main code here, to run repeatedly:
}