#include <Arduino.h>
#include <integration_tests.h>
#include <utility.h>
#include <RNG.h>
#include <unit_tests.h>
#include <uECC.h>
#include <SHA256.h>
#include <SHA3.h>

void setup() {
  Serial.begin(115200);
  while (!Serial)  // wait for Serial to be ready
    ;

  asm(".global _printf_float");  // enable float printf s for arduino
  InitializeRNG();

  TestableXOR* xor_test = new TestableXOR(32);
  TestableRNG* rng_test = new TestableRNG(32);
  SHA256 h_sha256;
  SHA3_256 h_sha3_256;
  const struct uECC_Curve_t* curve  = uECC_secp256r1();
  TestableHashing* sha256_test      = new TestableHashing(&h_sha256, 32, 32, "not used", "not used");
  TestableHashing* sha3_256_test    = new TestableHashing(&h_sha3_256, 32, 32, "not used", "not used");
  TestableElliptic* secp256r1_test  = new TestableElliptic(curve, 32, 64, 32);
  TestableAesGcm256* aesgcm256_test = new TestableAesGcm256();
  TestableEcies* ecies_test         = new TestableEcies(curve);
  TestableHashXor* hashxor_test     = new TestableHashXor();
  SerialLogWriter* logger           = new SerialLogWriter(100, true);

  logger->LogPrimitive(xor_test, "xor_256");
  logger->LogPrimitive(rng_test, "rng_256");
  logger->LogPrimitive(sha256_test, "sha2_256");
  logger->LogPrimitive(sha3_256_test, "sha3_256");
  logger->LogPrimitive(secp256r1_test, "secp256r1_ecc_dh");
  logger->LogPrimitive(ecies_test, "ecies");
  logger->LogPrimitive(aesgcm256_test, "aes256_gcm");
  logger->LogPrimitive(hashxor_test, "hashxor");

  Serial.println(">done");
}

void loop() {}