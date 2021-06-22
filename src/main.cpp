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

  SimpleBenchmarker benchmarker; // verbose sampling by default = each execution is written. if verbose sampling is disabled, only avg execution time is printed for each primitive.

  // this method enables/disables verbose_sampling, and collects all avg primitive execution times under all.log. can comment this out/in since verbose_sampling is default.
  //benchmarker.LogOnlyAverage(true, "all");

  // benchmarks 32 byte XOR and 32 byte RNG, the implementations of RNG primitive are only used to show functioality of the benchmarker and should not be used in normal code or for actual benchmarking.l

  // benchmarktotal method includes functions for setup (run once), setup_loop(before every benchmark), and teardown. if anyone of them are not needed ,set them to NULL.
  benchmarker.BenchmarkTotal("xor_256", primitive_xor, NULL, setup_xor_loop, NULL);

  // Benchmark method only takes the primitive, assumes no setup/teardown is required.
  benchmarker.Benchmark("rng_256", random_val);

  // signals to Seriallogger.py that logging is done and last logfile can be written.
  benchmarker.Finalize();
}

void loop()
{
  // put your main code here, to run repeatedly:
}