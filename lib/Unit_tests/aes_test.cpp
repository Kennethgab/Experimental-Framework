#include <unit_tests.h>

TestableAES::TestableAES(bool time_encryption) {
  this->time_encryption = time_encryption;
}

TestableAES::TestableAES(char *privkey_hex, char *input_raw,
                         char *expected_output_hex) {
  this->privkey_hex         = privkey_hex;
  this->input_raw           = input_raw;
  this->expected_output_hex = expected_output_hex;
}

bool TestableAES::TestingPrimitive() {
  byte priv_key[32];
  int ciphertext_len = 16;
  byte expected_ciphertext[ciphertext_len];
  byte ciphertext[ciphertext_len];
  byte decrypted[ciphertext_len];
  HexToBytes(privkey_hex, priv_key, 32);
  HexToBytes(expected_output_hex, expected_ciphertext, ciphertext_len);
  aes.setKey(priv_key, 32);
  aes.encryptBlock(ciphertext, (const uint8_t *)input_raw);
  aes.decryptBlock(decrypted, ciphertext);
  return EqualBuffer(ciphertext, expected_ciphertext, ciphertext_len);
}

float TestableAES::TimingPrimitive(SerialLogWriter *logger, size_t samples,
                                   bool verbose_sampling) {
  long start_enc, end_enc, execution_time_enc;
  long start_dec, end_dec, execution_time_dec;
  float avg_enc, avg_dec;
  long total_enc = 0;
  long total_dec = 0;
  if (verbose_sampling) {
    PrintSimpleHeader(logger, time_encryption ? "aes256_enc" : "aes256_dec",
                      samples);
    PrintSimpleColumns(logger);
  }
  for (size_t i = 0; i < samples; i++) {
    RNG.loop();
    aes.clear();
    byte priv_key[32];
    byte plaintext[32];
    RNG.rand(plaintext, 32);
    RNG.rand(priv_key, 32);

    // encryption
    start_enc = micros();
    byte ciphertext[32];
    aes.setKey(priv_key, 32);
    aes.encryptBlock(ciphertext, plaintext);
    end_enc            = micros();
    execution_time_enc = end_enc - start_enc;
    total_enc += execution_time_enc;

    // decryption
    start_dec = micros();
    byte decrypted[32];
    aes.setKey(priv_key, 32);
    aes.decryptBlock(decrypted, ciphertext);
    end_dec            = micros();
    execution_time_dec = end_dec - start_dec;
    total_dec += execution_time_dec;

    if (verbose_sampling) {
      PrintSimpleRow(logger, i + 1,
                     time_encryption ? execution_time_enc : execution_time_dec);
    }
  }
  avg_enc = total_enc / (float)samples;
  avg_dec = total_dec / (float)samples;
  if (!verbose_sampling) {
    PrintSimpleAvg(logger, time_encryption ? "aes256_enc" : "aes256_dec",
                   time_encryption ? avg_enc : avg_dec);
  }
  return time_encryption ? avg_enc : avg_dec;
}

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