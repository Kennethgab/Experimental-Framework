/**
 * @file test_crypto.cpp
 * @author Kennethgab
 * @brief Contains unit tests for associated benchmarked 
 * primitives.
 * @version 1.0
 * 
 * 
 */

#include <unity.h>
#include <Arduino.h>

/**
 * @brief function that returns true,
 * used to describe testing environment.
 * 
 * @return true 
 * 
 */
bool true_func()
{
  return true;
}

/**
 * @brief Unit test for true_func that utilizes the
 * ASSERT_TRUE function. this needs to be put between
 * UNITY_BEGIN and UNITY_END in setup() function
 * to be tested against.
 * 
 */
void always_true(void)
{
  TEST_ASSERT_TRUE(true_func);
}

void setup()
{
  UNITY_BEGIN();
  // put unit tests line by line here
  RUN_TEST(always_true);
  UNITY_END();
}

void loop() {}