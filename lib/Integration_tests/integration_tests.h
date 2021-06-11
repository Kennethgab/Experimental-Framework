#ifndef INTEGRATION_h
#define INTEGRATION_h
#include <Arduino.h>
#include <utility.h>
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
  virtual float TimingPrimitive();
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
  virtual float TimingPrimitive();
  void ResetRnd();
  void ResetInitial();
};

#endif