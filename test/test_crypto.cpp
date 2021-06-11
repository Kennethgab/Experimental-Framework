#include <unity.h>
#include <integration_tests.h>
#include <Arduino.h>
#include <unit_tests.h>
#include <utility.h>
#include <uECC.h>
#include <SHA256.h>
#include <SHA3.h>
#include "mbedtls/hkdf.h"


void AssertRNG(void) { TEST_ASSERT_TRUE(TestableRNG(32).TestingPrimitive()) }
void AssertXOR(void) { TEST_ASSERT_TRUE(TestableXOR(32).TestingPrimitive()); }
void AssertSHA256(void) {
  SHA256 hash;
  TEST_ASSERT_TRUE(TestableHashing(&hash, 32, 32, "Hello World!", "7f83b1657ff1fc53b92dc18148a1d65dfc2d4b1fa3d677284addd200126d9069").TestingPrimitive())
}
void AssertSHA3_256(void) {
  SHA3_256 hash;
  TEST_ASSERT_TRUE(TestableHashing(&hash, 32, 32, "Hello World!", "d0e47486bbf4c16acac26f8b653592973c1362909f90262877089f9c8a4536af").TestingPrimitive())
}
void AssertAesGcm256(void) {
  TEST_ASSERT_TRUE(TestableAesGcm256("7aabea926e9ea6f70764d4733cc94e8e627e8d5b23225d604194b74ce34308b2", "fc24b6e1828b5d54ec8c3b9a49e5d59c", "Hello World",
                                     "4d23c0ff490042ba2a7f1d", "028a1d75f4c9eadf024a66e5e1faefd5")
                       .TestingPrimitive());
}
void AssertSecp256r1(void) { TEST_ASSERT_TRUE(TestableElliptic(uECC_secp256r1(), 32, 64, 32).TestingPrimitive()); }
void AssertHashXor(void) { TEST_ASSERT_TRUE(TestableHashXor().TestingPrimitive()); }
void AssertEcies(void) { TEST_ASSERT_TRUE(TestableEcies(uECC_secp256r1()).TestingPrimitive()); }
void setup() {
  InitializeRNG();
  delay(2000);
  UNITY_BEGIN();
  RUN_TEST(AssertRNG);
  RUN_TEST(AssertXOR);
  RUN_TEST(AssertSHA256);
  RUN_TEST(AssertSHA3_256);
  RUN_TEST(AssertSecp256r1);
  RUN_TEST(AssertAesGcm256);
  RUN_TEST(AssertHashXor);
  RUN_TEST(AssertEcies);
  UNITY_END();
}

void loop() {}