#ifndef KDF_h
#define KDF_h
#include "mbedtls/hkdf.h"
#include "mbedtls/md.h"
#include <Arduino.h>

void hkdf_sha256_simple(uint8_t* input, size_t input_size, uint8_t* output, size_t output_size) {
  mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;
  mbedtls_hkdf(mbedtls_md_info_from_type(md_type), NULL, NULL, input, input_size, NULL, NULL, output, output_size);
}

#endif