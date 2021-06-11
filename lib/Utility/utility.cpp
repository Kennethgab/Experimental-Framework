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

// Loggedfunction returns avg execution time, and should also print all running samples.
// Takes amount of samples to execute, and size of bytes of crypto operation

void LogTemplate(TestablePrimitive *timed_primitive, const char *log_type, size_t samples, bool verbose_sampling) {
  char *device_name = (char *)STR(BOARDTYPE);
  FormatPrint("LOG START: %s %s\n", device_name, log_type);
  if (verbose_sampling) {
    FormatPrint("%-7s | %7s\n", "Sample", "Time [microseconds]");
    FormatPrint("--------+--------\n");
  }
  timed_primitive->set_samples(samples);
  timed_primitive->set_verbose_sampling(verbose_sampling);
  float avg = timed_primitive->TimingPrimitive();
  if (verbose_sampling) {
    FormatPrint("--------+--------\n");
  }
  FormatPrint("SAMPLES: %d, AVG: %.2f microseconds\n", samples, avg);
  FormatPrint("LOG END: %s %s\n", device_name, log_type);
}

bool EqualBuffer(const void *buffer1, const void *buffer2, size_t buffer_len) {
  if (memcmp(buffer1, buffer2, buffer_len) == 0) {
    return true;
  } else {
    return false;
  }
}

SerialLogWriter::SerialLogWriter(size_t samples, bool verbose_sampling) {
  this->samples          = samples;
  this->verbose_sampling = verbose_sampling;
  this->device_name      = (char *)STR(BOARDTYPE);
  if (!verbose_sampling) {
    Serial.print("---\n");
    FormatPrint("device: %s\n", device_name);
    FormatPrint("samples: %d\n", samples);
    FormatPrint("log_type: all\n");
    Serial.print("---\n");
    FormatPrint("%-7s  %7s\n", "Primitive", "AVG Time [microseconds]");
  }
}

void SerialLogWriter::PrintTableDelimiter() { FormatPrint("--------+--------\n"); }
void SerialLogWriter::set_samples(size_t samples) { this->samples = samples; }
void SerialLogWriter::set_verbose_sampling(bool verbose_sampling) { this->verbose_sampling = verbose_sampling; }
void SerialLogWriter::LogStart() {
  Serial.print("---\n");
  FormatPrint("device: %s\n", device_name);
  FormatPrint("samples: %d\n", samples);
  FormatPrint("log_type: %s\n", log_type);
  Serial.print("...\n");
  FormatPrint("%-7s | %7s\n", "Sample", "Time [microseconds]");
  PrintTableDelimiter();
}

void SerialLogWriter::LogEnd(float avg) {
  PrintTableDelimiter();
  FormatPrint("SAMPLES: %d, AVG: %d microseconds\n", samples, (int)round(avg));
}
void SerialLogWriter::LogResults(float avg) { FormatPrint("%-7s  %d\n", log_type, (int)round(avg)); }

void SerialLogWriter::LogPrimitive(TestablePrimitive *timed_primitive, char *log_type) {
  this->log_type = log_type;
  timed_primitive->set_samples(samples);
  timed_primitive->set_verbose_sampling(verbose_sampling);
  if (verbose_sampling) {
    LogStart();
  }
  int avg = timed_primitive->TimingPrimitive();
  if (verbose_sampling) {
    LogEnd(avg);
  } else {
    LogResults(avg);
  }
}
