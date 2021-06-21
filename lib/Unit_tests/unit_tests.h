#ifndef SIMPLE_MAIN
#ifndef UNIT_h
#define UNIT_h
#include <RNG.h>
#include <Hash.h>
#include <GCM.h>
#include <AES.h>
#include <SerialLogWriter.h>

class TestableRNG : public TestablePrimitive {
private:
  size_t buffer_size;

public:
  TestableRNG(size_t buffer_size);
  virtual float TimingPrimitive(SerialLogWriter *logger, size_t samples,
                                bool verbose_sampling);
  virtual bool TestingPrimitive();
};

class TestableHashing : public TestablePrimitive {
private:
  Hash *hash;
  size_t input_size;
  size_t output_size;
  char *expected_hash_hex;
  char *input;
  char *log_type;

public:
  TestableHashing(char *log_type, Hash *hash, size_t input_size,
                  size_t output_size, char *input, char *expected_hash_hex);
  virtual bool TestingPrimitive();
  virtual float TimingPrimitive(SerialLogWriter *logger, size_t samples,
                                bool verbose_sampling);
};

class TestableAES : public TestablePrimitive {
private:
  AES256 aes;
  char *privkey_hex;
  char *input_raw;
  char *expected_output_hex;
  bool time_encryption;

public:
  // timing constructor
  TestableAES(bool time_encryption);
  // testing constructor
  TestableAES(char *privkey_hex, char *input_raw, char *expected_output_hex);

  virtual bool TestingPrimitive();
  virtual float TimingPrimitive(SerialLogWriter *logger, size_t samples,
                                bool verbose_sampling);
};

class TestableAesGcm256 : public TestablePrimitive {
private:
  char *nonce_hex;
  GCM<AES256> gcm;
  char *privkey_hex;
  char *input_raw;
  char *expected_output_hex;
  char *expected_tag_hex;

public:
  // used for testing
  TestableAesGcm256(char *privkey_hex, char *nonce_hex, char *input_hex,
                    char *expected_output_hex, char *expected_tag_hex);
  // used for timing with random values,
  TestableAesGcm256() = default;
  virtual bool TestingPrimitive();
  virtual float TimingPrimitive(SerialLogWriter *logger, size_t samples,
                                bool verbose_sampling);
};

class TestableElliptic : public TestablePrimitive {
private:
  size_t privkey_size;
  size_t secret_size;
  size_t pubkey_size;
  const struct uECC_Curve_t *curve;

public:
  TestableElliptic(const struct uECC_Curve_t *curve, size_t privkey_size,
                   size_t pubkey_size, size_t secret_size);
  virtual bool TestingPrimitive();
  virtual float TimingPrimitive(SerialLogWriter *logger, size_t samples,
                                bool verbose_sampling);
};

class TestableXOR : public TestablePrimitive {
private:
  size_t buffer_size;

public:
  TestableXOR(size_t buffer_size);
  virtual bool TestingPrimitive();
  virtual float TimingPrimitive(SerialLogWriter *logger, size_t samples,
                                bool verbose_sampling);
};

#endif
#endif