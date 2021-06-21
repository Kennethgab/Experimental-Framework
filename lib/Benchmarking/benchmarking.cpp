#include <benchmarking.h>

/** \brief Benchmarks the given function  in terms of
 * execution time. returns avg execution time.
 *
 * \param logger the SerialLogWriter instance used for serial
 * communication
 * \param samples number of executions/samples to execute the
 * benchmarked function
 * \param verbose_sampling if true: print each execution if
 * false: print log_type: avg summary of entire execution time.
 *  \param log_type give name to executed function that will
 *  appear in the log
 * \param benchmarked_func pointer to function that gets benchmarked
 * \param loop_setup pointer to setting up for benchmarked function
 * before every iteration, for instance a buffer or memory
 * that the benchmarked function refers to.
 * \param setup initial setup that only occurs once before all iterations.
 * \param teardown func executed on teardown
 *
 * The benchmarked function could make use of anything set up by
 * the setup or loop_setup functions if those functions act on
 * a static buffer or allocated memery that the benchmarked
 * function has access/ be in scope to.
 *
 */
float BenchmarkAll(SerialLogWriter *logger, size_t samples,
                   bool verbose_sampling, const char *log_type,
                   func_ptr benchmarked_func, func_ptr loop_setup,
                   func_ptr setup, func_ptr teardown)
{
  long start, end, execution_time;
  float avg;
  long total = 0;
  if (verbose_sampling && logger != NULL)
  {
    PrintSimpleHeader(logger, log_type, samples);
    PrintSimpleColumns(logger);
  }
  if (setup != NULL)
  {
    setup();
  }
  for (size_t i = 0; i < samples; i++)
  {
    if (loop_setup != NULL)
    {
      loop_setup();
    }
    start = micros();
    benchmarked_func();
    end = micros();
    execution_time = end - start;
    total += execution_time;
    if (verbose_sampling && logger != NULL)
    {
      PrintSimpleRow(logger, i + 1, execution_time);
    }
  }
  avg = total / (float)samples;
  if (!verbose_sampling && logger != NULL)
  {
    PrintSimpleAvg(logger, log_type, avg);
  }
  if (teardown != NULL)
  {
    teardown();
  }
  return avg;
}

/**
 * \brief sets up default simple benchmarker with default serial
 *
 * Benchmarker will use sample size of 100 and verbose sampling will be enabled.
 * seriallogger uses delimiter ",".
 *
 *
 *
 */
SimpleBenchmarker::SimpleBenchmarker() : logger()
{
  this->samples = 100;
  this->verbose_sampling = true;
}

/** \brief Instantiate SimpleBenchmark with unique params
 * \param logger the SerialLogwriter instance to use
 * \param verbose_sampling enable/disable verbose sampling
 * \param samples amount of samples to execute
 *
 */
SimpleBenchmarker::SimpleBenchmarker(SerialLogWriter logger,
                                     bool verbose_sampling, size_t samples)
{
  this->verbose_sampling = verbose_sampling;
  this->logger = logger;
  this->samples = samples;
};

/** 
 * \brief Benchmarks a function with teardown, setup and setup_loop if necessary that 
 * can be set to NULL if not used.
 * \param log_type unique name for the log to be written
 * \param benchmarked_func the function to be benchmarked
 * \param setup fires once before benchmarking starts, function for setting up
 * \param setup_loop fires before every execution of \a benchmarked_func, used
 * to reset all parameters that benchmarked_func uses or set up data.
 * \param teardown used for any teardown, fires once after benchmarking.
 * \return the average execution time
 * 
 * 
 */
float SimpleBenchmarker::BenchmarkTotal(char *log_type,
                                        func_ptr benchmarked_func,
                                        func_ptr setup, func_ptr setup_loop,
                                        func_ptr teardown)
{
  return BenchmarkAll(&this->logger, this->samples, this->verbose_sampling,
                      log_type, benchmarked_func, setup_loop, setup, teardown);
}

/**
 * \brief Simple benchmark that only fires off the benchmarked function 
 * \param log_type unique name for the log to be written
 * \param benchmarked_func function to be benchmarked
 * \return the average execution time
*/

float SimpleBenchmarker::Benchmark(char *log_type, func_ptr benchmarked_func)
{
  return this->BenchmarkTotal(log_type, benchmarked_func, NULL, NULL, NULL);
}
