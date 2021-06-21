

#ifndef PRIMITIVE_TESTS_h
#define PRIMITIVE_TESTS_h
#include <SerialLogWriter.h>
#include <rand.h>

/** 
 * \brief used as simple interface for testing and benchmarking
*/
class TestablePrimitive
{
private:
public:
  /** 
  *  method that class implements for timing a function,
 * returns avg execution time
 */
  virtual float TimingPrimitive(SerialLogWriter *logger, size_t samples,
                                bool verbose_sampling) = 0;
  /**
   * method that class implements for
 * running a test against testvectors provided in constructor of 
 * implementor class
 */
  virtual bool TestingPrimitive() = 0;
};

byte buffer1[32];
byte buffer2[32];
byte result_buffer[32];

/**
 * @brief Simple RNG primitive to be benchmarked.
 * 
 */
void random_val()
{
  RandomVal(buffer1, 32);
}

/**
 * @brief Set the up xor loop object loop for XOR primitive
 * to load buffers with random data.
 * 
 */
void setup_xor_loop()
{
  RandomVal(buffer1, 32);
  RandomVal(buffer2, 32);
}
/**
 * @brief XOR primitive to be benchmarked
 * 
 */
void primitive_xor()
{
  XorBytes(buffer1, buffer2, result_buffer, 32);
}

#endif