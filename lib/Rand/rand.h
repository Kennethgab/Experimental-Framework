#ifndef RAND_h
#define RAND_h
#include <Arduino.h>
// if defined: use <RNG.h> from crypto library
#ifndef SIMPLE_MAIN
//#define USE_CRYPTO_RNG
#endif
#ifdef USE_CRYPTO_RNG
#include <RNG.h>
#endif

extern void InitializeRNG();
extern int RandomVal(uint8_t *dest, size_t size);

#endif