#ifndef SIMPLE_MAIN
#ifndef INTEGRATION_h
#define INTEGRATION_h

#include <unit_tests.h>
#include <uECC.h>
#include <SHA256.h>
#include <GCM.h>
#include <AES.h>

class TestableEcies : public TestablePrimitive {
private:
  SHA256 hash;
  GCM<AES256> gcm;
  const struct uECC_Curve_t* curve;

public:
  TestableEcies(const struct uECC_Curve_t* curve);
  virtual bool TestingPrimitive();
  virtual float TimingPrimitive(SerialLogWriter* logger, size_t samples,
                                bool verbose_sampling);
};

class TestableHashXor : public TestablePrimitive {
private:
  byte cluster_identity[16];
  byte random_num1[16];
  byte random_num2[16];
  byte ip[3];
  byte identity[8];

public:
  TestableHashXor();
  virtual bool TestingPrimitive();
  virtual float TimingPrimitive(SerialLogWriter* logger, size_t samples,
                                bool verbose_sampling);
  void ResetRnd();
  void ResetInitial();
};

class TestableNewHope : public TestablePrimitive {
public:
  TestableNewHope() = default;
  virtual bool TestingPrimitive();
  virtual float TimingPrimitive(SerialLogWriter* logger, size_t samples,
                                bool verbose_sampling);
};

#endif
#endif
