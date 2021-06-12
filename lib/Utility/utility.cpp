#include <Arduino.h>
#include <unit_tests.h>
#include <Crypto.h>
#include <utility.h>
#include <RNG.h>

#define XSTR(x) #x
#define STR(x) XSTR(x)

void InitializeRNG() {
  char *current_time = (char *)STR(CURRENT_TIME);
  RNG.begin(STR(BOARDTYPE));
  RNG.stir((const uint8_t *)current_time, (size_t)strlen(current_time));
}


void XorBytes(byte *buffer1, byte *buffer2, byte *output_buffer, int buffer_size) {
  for (int i = 0; i < buffer_size; i++) {
    output_buffer[i] = buffer1[i] ^ buffer2[i];
  }
}

void HexString(char *buffer, byte *bytes, size_t buffer_size) {
  for (int i = 0; i < buffer_size; i++) {
    buffer += sprintf(buffer, "%02x", bytes[i]);
  }
}

void HexPrint(byte *bytes, size_t buffer_size) {
  for (int i = 0; i < buffer_size; i++) {
    char str[3];
    sprintf(str, "%02x", (int)bytes[i]);
    Serial.print(str);
  }
  Serial.print("\n");
}

int HexToBytes(const char *hex_str, byte *byte_array, int byte_array_max) {
  int hex_str_len = strlen(hex_str);
  int i = 0, j = 0;

  // The output array size is half the hex_str length (rounded up)
  int byte_array_size = (hex_str_len + 1) / 2;

  if (byte_array_size > byte_array_max) {
    // Too big for the output array
    return -1;
  }

  if (hex_str_len % 2 == 1) {
    // hex_str is an odd length, so assume an implicit "0" prefix
    if (sscanf(&(hex_str[0]), "%1hhx", &(byte_array[0])) != 1) {
      return -1;
    }

    i = j = 1;
  }

  for (; i < hex_str_len; i += 2, j++) {
    if (sscanf(&(hex_str[i]), "%2hhx", &(byte_array[j])) != 1) {
      return -1;
    }
  }

  return byte_array_size;
}

bool EqualBuffer(const void *buffer1, const void *buffer2, size_t buffer_len) {
  if (memcmp(buffer1, buffer2, buffer_len) == 0) {
    return true;
  } else {
    return false;
  }
}
