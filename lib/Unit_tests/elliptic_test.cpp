#include <Arduino.h>
#include <uECC.h>
#include <unit_tests.h>
#include <utility.h>

TestableElliptic::TestableElliptic(const struct uECC_Curve_t *curve,
                                   size_t privkey_size, size_t pubkey_size,
                                   size_t secret_size) {
  this->curve        = curve;
  this->privkey_size = privkey_size;
  this->pubkey_size  = pubkey_size;
  this->secret_size  = secret_size;
}

bool TestableElliptic::TestingPrimitive() {
  byte permanent_public[privkey_size * 2];
  byte permanent_private[privkey_size];
  byte ephemeral_public[privkey_size * 2];
  byte ephemeral_private[privkey_size];
  byte shared_secret1[privkey_size];
  byte shared_secret2[privkey_size];
  uECC_set_rng(&RandomVal);
  uECC_make_key(permanent_public, permanent_private, curve);
  uECC_make_key(ephemeral_public, ephemeral_private, curve);
  uECC_shared_secret(permanent_public, ephemeral_private, shared_secret1,
                     curve);
  uECC_shared_secret(ephemeral_public, permanent_private, shared_secret2,
                     curve);
  return EqualBuffer(shared_secret2, shared_secret1, privkey_size);
}

float TestableElliptic::TimingPrimitive(SerialLogWriter *logger, size_t samples,
                                        bool verbose_sampling) {
  long start, end, execution_time;
  float avg;
  if (verbose_sampling) {
    PrintSimpleHeader(logger, "secp256r1_ecc_dh", samples);
    PrintSimpleColumns(logger);
  }
  long total = 0;
  uECC_set_rng(&RandomVal);
  byte permanent_public[pubkey_size];
  byte permanent_private[privkey_size];
  uECC_make_key(permanent_public, permanent_private, curve);
  for (int i = 0; i < samples; i++) {
    start = micros();
    byte ephemeral_public[pubkey_size];
    byte ephemeral_private[privkey_size];
    byte client_secret[secret_size];
    uECC_make_key(ephemeral_public, ephemeral_private, curve);
    uECC_shared_secret(permanent_public, ephemeral_private, client_secret,
                       curve);
    end            = micros();
    execution_time = end - start;
    total += execution_time;
    if (verbose_sampling) {
      PrintSimpleRow(logger, i + 1, execution_time);
    }
  }
  avg = total / (float)samples;
  if (!verbose_sampling) {
    PrintSimpleAvg(logger, "secp256r1_ecc_dh", avg);
  }
  return avg;
}
