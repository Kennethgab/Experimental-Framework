
#ifndef UTILITY_h
#define UTILITY_h
#include <RNG.h>
#include <Arduino.h>
#define XSTR(x) #x
#define STR(x) XSTR(x)

extern void InitializeRNG();
extern void HexString(char *buffer, byte *bytes, size_t buffer_size);

extern void XorBytes(byte *buffer1, byte *buffer2, byte *output_buffer, int buffer_size);
extern void HexPrint(byte *bytes, size_t buffer_size);
extern int HexToBytes(const char *hex_str, byte *byte_array, int byte_array_max);

// returns true if buffers are equal in memory
extern bool EqualBuffer(const void *buffer1, const void *buffer2, size_t buffer_len);

template <typename... T>
void FormatPrint(const char *str, T... args) {
  int len = snprintf(NULL, 0, str, args...);
  if (len) {
    char buff[len];
    sprintf(buff, str, args...);
    Serial.print(buff);
  }
}
typedef float (*timed_func)(size_t, size_t, bool);
static int RandomVal(uint8_t *dest, size_t buffer_size) {
  RNG.rand(dest, buffer_size);
  RNG.loop();
  return 1;
}

#endif