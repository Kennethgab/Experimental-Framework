#include <unit_tests.h>
#include <integration_tests.h>
#include <rand.h>

TestableEcies::TestableEcies(const struct uECC_Curve_t* curve) {
  this->curve = curve;
}

float TestableEcies::TimingPrimitive(SerialLogWriter* logger, size_t samples,
                                     bool verbose_sampling) {
  uECC_set_rng(&RandomVal);
  long start, end, execution_time;
  float avg;
  long total = 0;
  if (verbose_sampling) {
    PrintSimpleHeader(logger, "ecies", samples);
    PrintSimpleColumns(logger);
  }
  byte permanent_pubkey[64], permanent_privkey[32], message[8], ciphertext[8];
  RNG.rand(message, 8);
  uECC_make_key(permanent_pubkey, permanent_privkey, curve);
  for (size_t i = 0; i < samples; i++) {
    RNG.loop();
    hash.clear();
    start = micros();
    byte ephemeral_pubkey[64], ephemeral_privkey[32], shared_secret[32];
    byte nonce[16], tag[16];
    RNG.rand(nonce, 16);
    uECC_make_key(ephemeral_pubkey, ephemeral_privkey, curve);
    uECC_shared_secret(permanent_pubkey, ephemeral_privkey, shared_secret,
                       curve);
    hash.update(shared_secret, 32);
    hash.finalize(shared_secret, 32);
    gcm.setIV(nonce, 16);
    gcm.setKey(shared_secret, 32);
    gcm.encrypt(ciphertext, message, 8);
    gcm.computeTag(tag, 16);

    end            = micros();
    execution_time = end - start;
    total += execution_time;
    if (verbose_sampling) {
      PrintSimpleRow(logger, i + 1, execution_time);
    }
  }
  avg = total / (float)samples;
  if (!verbose_sampling) {
    PrintSimpleAvg(logger, "ecies", avg);
  }
  return avg;
}

bool TestableEcies::TestingPrimitive() {
  byte permanent_pubkey[64], permanent_privkey[32], ephemeral_privkey[32],
      ephemeral_pubkey[64];
  byte shared_secret_permanent[32], shared_secret_ephemeral[32], nonce[16],
      tag[16];
  byte message[16];
  byte ciphertext[16];
  byte plaintext[16];
  uECC_set_rng(&RandomVal);
  uECC_make_key(permanent_pubkey, permanent_privkey, curve);
  uECC_make_key(ephemeral_pubkey, ephemeral_privkey, curve);
  RNG.rand(nonce, 16);
  RNG.rand(message, 16);
  // shared secret generation
  uECC_shared_secret(permanent_pubkey, ephemeral_privkey,
                     shared_secret_ephemeral, curve);
  uECC_shared_secret(ephemeral_pubkey, permanent_privkey,
                     shared_secret_permanent, curve);

  hash.clear();
  hash.update(shared_secret_ephemeral, 32);
  hash.finalize(shared_secret_ephemeral, 32);
  hash.clear();
  hash.update(shared_secret_permanent, 32);
  hash.finalize(shared_secret_permanent, 32);
  hash.clear();

  if (!EqualBuffer(shared_secret_permanent, shared_secret_ephemeral, 32)) {
    return false;
  }

  // gcm encrypt
  gcm.clear();
  gcm.setKey(shared_secret_ephemeral, 32);
  gcm.setIV(nonce, 16);
  gcm.encrypt(ciphertext, message, 16);
  gcm.computeTag(tag, sizeof(tag));

  // gcm decrypt
  gcm.clear();
  gcm.setKey(shared_secret_permanent, 32);
  gcm.setIV(nonce, 16);
  gcm.decrypt(plaintext, ciphertext, 16);

  return EqualBuffer(message, plaintext, 16) && gcm.checkTag(tag, sizeof(tag));
}
