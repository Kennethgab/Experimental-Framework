#include <utility.h>

/**
 * \brief XOR's bitwise two buffers containing bytes together
 * \param buffer1 First buffer to be XOR.
 * \param buffer2 Second buffer to be XOR.
 * \param output_buffer Buffer to store result in.
 * \param buffer_size the size of all buffer arrays.
 *
 *
 * all buffers should have equal size, where the size, or the length of the
 array,
 * is denoted by \a buffer_size.
*/
void XorBytes(byte *buffer1, byte *buffer2, byte *output_buffer,
              int buffer_size) {
  for (int i = 0; i < buffer_size; i++) {
    output_buffer[i] = buffer1[i] ^ buffer2[i];
  }
}

/**
 * \brief Takes a buffer with bytes and outputs a hexstring representation
 * \param buffer char buffer to be loaded with hexstring
 * \param bytes buffer containing bytes
 * \param buffer_size size of the byte buffer, amount of bytes
 */
void HexString(char *buffer, byte *bytes, size_t buffer_size) {
  for (int i = 0; i < buffer_size; i++) {
    buffer += sprintf(buffer, "%02x", bytes[i]);
  }
}

/**
 * \brief Takes a byte buffer and prints its contents as hexidecimal
 * representation.
 * \param bytes the byte buffer to be printed.
 * \param buffer_size size of the byte buffer.
 */
void HexPrint(byte *bytes, size_t buffer_size) {
  for (int i = 0; i < buffer_size; i++) {
    char str[3];
    sprintf(str, "%02x", (int)bytes[i]);
    Serial.print(str);
  }
  Serial.print("\n");
}

/**
 * \brief Takes a hexidecimal string and loads a byte buffer with those bytes.
 * \param hex_str hexadecimal byte reprentation to be put into byte buffer.
 * \param byte_array pointer to buffer that will be filled.
 * \param byte_array_max should atleast be as small as the size of the byte
 * array.
 */
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

/**
 *\brief Checks if two buffers are equal in memory, i.e each entry is equal to
 each Returnts true if they are equal, false if not.
 * \param buffer1 pointer to first buffer to be compared
 * \param buffer2 pointer to second buffer to be compared
 * \param buffer_len length of both buth buffers
 *
 *
*/
bool EqualBuffer(const void *buffer1, const void *buffer2, size_t buffer_len) {
  if (memcmp(buffer1, buffer2, buffer_len) == 0) {
    return true;
  } else {
    return false;
  }
}
