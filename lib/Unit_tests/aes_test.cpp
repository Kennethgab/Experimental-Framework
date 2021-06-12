#include <GCM.h>
#include <utility.h>
#include <Arduino.h>
#include <unit_tests.h>
#include <AES.h>

TestableAesGcm256::TestableAesGcm256(char *privkey_hex, char *nonce_hex,
                                     char *input_raw, char *expected_output_hex,
                                     char *expected_tag_hex) {
  this->privkey_hex         = privkey_hex;
  this->nonce_hex           = nonce_hex;
  this->input_raw           = input_raw;
  this->expected_output_hex = expected_output_hex;
  this->expected_tag_hex    = expected_tag_hex;
}

bool TestableAesGcm256::TestingPrimitive() {
  gcm.clear();
  byte priv_key[32];
  int ciphertext_len = (strlen(expected_output_hex) + 1) / 2;
  int nonce_len      = (strlen(nonce_hex) + 1) / 2;
  byte nonce[nonce_len];
  byte expected_ciphertext[ciphertext_len];
  byte expected_tag[16];
  byte ciphertext[ciphertext_len];
  byte tag[16];
  HexToBytes(privkey_hex, priv_key, 32);
  HexToBytes(nonce_hex, nonce, nonce_len);
  HexToBytes(expected_tag_hex, expected_tag, 16);
  HexToBytes(expected_output_hex, expected_ciphertext, ciphertext_len);
  gcm.setKey(priv_key, 32);
  gcm.setIV(nonce, nonce_len);
  gcm.encrypt(ciphertext, (const uint8_t *)input_raw, strlen(input_raw));
  gcm.computeTag(tag, 16);
  return EqualBuffer(ciphertext, expected_ciphertext, ciphertext_len) &&
         EqualBuffer(tag, expected_tag, 16);
}

float TestableAesGcm256::TimingPrimitive(SerialLogWriter *logger,
                                         size_t samples,
                                         bool verbose_sampling) {
  long start, end, execution_time;
  float avg;
  long total = 0;
  if (verbose_sampling) {
    PrintSimpleHeader(logger, "aes256_gcm", samples);
    PrintSimpleColumns(logger);
  }
  for (size_t i = 0; i < samples; i++) {
    RNG.loop();
    gcm.clear();
    byte priv_key[32];
    byte nonce[16];
    byte plaintext[32];
    RNG.rand(plaintext, 32);
    RNG.rand(priv_key, 32);
    RNG.rand(nonce, 16);
    start = micros();
    byte ciphertext[32];
    byte tag[16];
    gcm.setKey(priv_key, 32);
    gcm.setIV(nonce, 16);
    gcm.encrypt(ciphertext, plaintext, 32);
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
    PrintSimpleAvg(logger, "aes256_gcm", avg);
  }
  return avg;
}