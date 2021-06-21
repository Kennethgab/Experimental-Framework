#include <rand.h>
#include <utility.h>

/**
 * \brief Used in micro-ecc test script to fill buffer with random values.
 * Very basic.
 * https://github.com/kmackay/micro-ecc/blob/master/examples/ecc_test/ecc_test.ino
 * \param dest pointer to buffer to fill with randomness.
 * \param size size of buffer to fill with randomness.
 *
 */
static int RandomUECC(uint8_t *dest, unsigned size) {
  // Use the least-significant bits from the ADC for an unconnected pin (or
  // connected to a source of random noise). This can take a long time to
  // generate random data if the result of analogRead(0) doesn't change very
  // frequently.
  while (size) {
    uint8_t val = 0;
    for (unsigned i = 0; i < 8; ++i) {
      int init  = analogRead(0);
      int count = 0;
      while (analogRead(0) == init) {
        ++count;
      }

      if (count == 0) {
        val = (val << 1) | (init & 0x01);
      } else {
        val = (val << 1) | (count & 0x01);
      }
    }
    *dest = val;
    ++dest;
    --size;
  }
  // NOTE: it would be a good idea to hash the resulting random data using
  // SHA-256 or similar.
  return 1;
}

/**
 * \brief used to initialize Arduino Crypto library RNG class.
 * Necessary for the class to work.
 * also increases entropy slightly.
 *
 */
void InitializeRNG() {
#ifdef USE_CRYPTO_RNG
  char *current_time = (char *)STR(CURRENT_TIME);
  RNG.begin(STR(BOARDTYPE));
  RNG.stir((const uint8_t *)current_time, (size_t)strlen(current_time));
#endif
}

/**
 * \brief Used by uECC as source of randomness, by default use RNG.h instead
 * \param dest pointer to buffer to fill with randomness
 * \param buffer_size size of buffer to be filled with randomness.
 *
 */
int RandomVal(uint8_t *dest, size_t buffer_size) {
#ifdef USE_CRYPTO_RNG
  RNG.rand(dest, buffer_size);
  RNG.loop();
#else
  RandomUECC(dest, buffer_size);
#endif
  return 1;
}
