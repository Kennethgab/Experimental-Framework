#ifndef SERIALLOGWRITER_h
#define SERIALLOGWRITER_h
#include <Arduino.h>
#include <utility.h>



/**
 * \brief Used to hold key-value pairs for the YAML header in each logfile.
 * \param key the key name ie "device"
 * \param value the value of the key ie "ESP32"
 *
 */
typedef struct
{
  const char *key;
  const char *value;
} Header;

class SerialLogWriter
{

public:
  const char *delimiter;
  Header *default_headers;
  size_t default_header_len;
  SerialLogWriter();
  SerialLogWriter(Header *default_header, size_t len, const char *delimiter);
  void PrintRow(const char **row, size_t len);
  void PrintHeaders(Header *headers, size_t len, bool print_default);
  void set_delimiter(const char *delimiter) { this->delimiter = delimiter; }
  void set_default_headers(Header *headers, size_t len);
};

extern void PrintSimpleRow(SerialLogWriter *logger, size_t sample,
                           long execution_time);

extern void PrintSimpleAvg(SerialLogWriter *logger, const char *log_type,
                           float avg);

extern void PrintSimpleHeader(SerialLogWriter *logger, const char *log_type,
                              size_t samples);

extern void PrintSimpleColumns(SerialLogWriter *logger);

#endif
