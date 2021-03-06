
#include <integration_tests.h>
#include <utility.h>
#include <RNG.h>
#include <unit_tests.h>
#include <uECC.h>
#include <SHA256.h>
#include <SHA3.h>
#include <BLAKE2s.h>
#include <NewHope.h>
#include <SerialLogWriter.h>
#include <rand.h>

// used on ESP32 to change stack size dynamically
#if !(USING_DEFAULT_ARDUINO_LOOP_STACK_SIZE)
uint16_t USER_CONFIG_ARDUINO_LOOP_STACK_SIZE = 16384;
#endif

void setup() {
  Serial.begin(115200);
  while (!Serial)  // wait for Serial to be ready
    ;
  Serial.print("\n\n");
  asm(".global _printf_float");  // enable float printf s for arduino
  InitializeRNG();

  bool verbose_sampling = true;
  size_t samples        = 100;
  char* delimiter       = ",";

  Header default_headers[] = {{"device", (char*)STR(BOARDTYPE)},
                              {"delimiter", delimiter}};

  // used if not verbose sampling
  char samples_str[15];
  snprintf(samples_str, 15, "%d", samples);
  Header aggregrate_headers[] = {{"log_type", "all"}, {"samples", samples_str}};
  const char* aggregate_columns[] = {"primitive", "AVG time [microseconds]"};

  const size_t def_header_len =
      sizeof(default_headers) / sizeof(default_headers[0]);
  const size_t aggr_header_len =
      sizeof(aggregrate_headers) / sizeof(aggregrate_headers[0]);
  const size_t aggr_column_len =
      sizeof(aggregate_columns) / sizeof(aggregate_columns[0]);

  SerialLogWriter* logger =
      new SerialLogWriter(default_headers, def_header_len, ",");

  if (!verbose_sampling) {
    logger->PrintHeaders(aggregrate_headers, aggr_header_len);
    logger->PrintRow(aggregate_columns, aggr_column_len);
  }

  TestablePrimitive* xor_test = new TestableXOR(32);
  TestablePrimitive* rng_test = new TestableRNG(32);
  SHA256 h_sha256;
  SHA3_256 h_sha3_256;
  BLAKE2s h_blake2s;
  const struct uECC_Curve_t* curve = uECC_secp256r1();
  TestablePrimitive* sha256_test   = new TestableHashing(
      "sha2_256", &h_sha256, 32, 32, "not used", "not used");
  TestablePrimitive* sha3_256_test = new TestableHashing(
      "sha3_256", &h_sha3_256, 32, 32, "not used", "not used");
  TestablePrimitive* blake2s_test = new TestableHashing(
      "blake2s", &h_blake2s, 32, 32, "not used", "not used");
  TestablePrimitive* secp256r1_test  = new TestableElliptic(curve, 32, 64, 32);
  TestablePrimitive* aes256_enc_test = new TestableAES(true);
  TestablePrimitive* aes256_dec_test = new TestableAES(false);
  TestablePrimitive* aesgcm256_test  = new TestableAesGcm256();
  TestablePrimitive* ecies_test      = new TestableEcies(curve);
  TestablePrimitive* hashxor_test    = new TestableHashXor();
  TestablePrimitive* newhope_test    = new TestableNewHope();

  TestablePrimitive* logged_primitives[] = {
      xor_test,       rng_test,       sha256_test, sha3_256_test,
      secp256r1_test, aesgcm256_test, ecies_test,  hashxor_test};

  // TestablePrimitive* logged_primitives[] = {blake2s_test, aes256_enc_test,
  //                                           aes256_dec_test};

  // TestablePrimitive* logged_primitives[] = {newhope_test};

  const size_t test_len =
      sizeof(logged_primitives) / sizeof(logged_primitives[0]);

  for (size_t i = 0; i < test_len; i++) {
    TestablePrimitive* primitive = logged_primitives[i];
    primitive->TimingPrimitive(logger, samples, verbose_sampling);
  }

  Serial.println(">done");
}

void loop() {}
