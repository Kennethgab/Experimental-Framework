
#ifndef UTILITY_h
#define UTILITY_h
#include <Arduino.h>
#ifdef BOARDTYPE
#define XSTR(x) #x
#else
#define XSTR(x) "error, set BOARDTYPE env in platformio.ini"
#endif
#define STR(x) XSTR(x)

extern void HexString(char *buffer, byte *bytes, size_t buffer_size);

extern void XorBytes(byte *buffer1, byte *buffer2, byte *output_buffer,
                     int buffer_size);
extern void HexPrint(byte *bytes, size_t buffer_size);

extern int HexToBytes(const char *hex_str, byte *byte_array,
                      int byte_array_max);

extern bool EqualBuffer(const void *buffer1, const void *buffer2,
                        size_t buffer_len);

template <typename... T>
void FormatPrint(const char *str, T... args) {
  int len = snprintf(NULL, 0, str, args...);
  if (len) {
    char buff[len];
    sprintf(buff, str, args...);
    Serial.print(buff);
  }
}

#endif