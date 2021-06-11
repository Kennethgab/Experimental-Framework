#ifndef UNIT_h
#define UNIT_h
#include <RNG.h>
#include <Hash.h>
#include <GCM.h>
#include <AES.h>

class TestablePrimitive {
private:
public:
  size_t samples                  = 0;
  bool verbose_sampling           = false;
  virtual float TimingPrimitive() = 0;
  virtual bool TestingPrimitive() = 0;
  void set_samples(size_t samples) { this->samples = samples; }
  void set_verbose_sampling(bool verbose) { this->verbose_sampling = verbose; }
};

class TestableRNG : public TestablePrimitive {
private:
  size_t buffer_size;

public:
  TestableRNG(size_t buffer_size);
  virtual float TimingPrimitive();
  virtual bool TestingPrimitive();
};

class TestableHashing : public TestablePrimitive {
private:
  Hash *hash;
  size_t input_size;
  size_t output_size;
  char *expected_hash_hex;
  char *input;

public:
  TestableHashing(Hash *hash, size_t input_size, size_t output_size, char *input, char *expected_hash_hex);
  virtual bool TestingPrimitive();
  virtual float TimingPrimitive();
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
  TestableAesGcm256(char *privkey_hex, char *nonce_hex, char *input_hex, char *expected_output_hex, char *expected_tag_hex);
  // used for timing with random values,
  TestableAesGcm256() = default;
  virtual bool TestingPrimitive();
  virtual float TimingPrimitive();
};

class TestableElliptic : public TestablePrimitive {
private:
  size_t privkey_size;
  size_t secret_size;
  size_t pubkey_size;
  const struct uECC_Curve_t *curve;

public:
  TestableElliptic(const struct uECC_Curve_t *curve, size_t privkey_size, size_t pubkey_size, size_t secret_size);
  virtual bool TestingPrimitive();
  virtual float TimingPrimitive();
};

class TestableXOR : public TestablePrimitive {
private:
  size_t buffer_size;

public:
  TestableXOR(size_t buffer_size);
  virtual float TimingPrimitive();
  virtual bool TestingPrimitive();
};

#endif