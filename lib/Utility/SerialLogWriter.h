#ifndef SERIALLOGWRITER_h
#define SERIALLOGWRITER_h
#include <Arduino.h>
#include <utility.h>

class SerialLogWriter;

class TestablePrimitive {
private:
public:
  virtual float TimingPrimitive(SerialLogWriter *logger, size_t samples,
                                bool verbose_sampling) = 0;
  virtual bool TestingPrimitive()                      = 0;
};

typedef struct {
  char *key;
  char *value;
} Header;

class SerialLogWriter {
private:
  char *delimiter;
  Header *default_headers;
  size_t default_header_len;

public:
  SerialLogWriter(Header *default_header, size_t len, char *delimiter);
  void PrintRow(char **row, size_t len);
  void LogPrimitive(TestablePrimitive *timed_primitive, char *log_type);
  void PrintHeaders(Header *headers, size_t len);
  void set_delimiter(char *delimiter) { this->delimiter = delimiter; }
  void set_default_headers(Header *headers, size_t len);
};

// used specifically with TestablePrimitive
extern void PrintSimpleRow(SerialLogWriter *logger, size_t sample,
                           long execution_time);

extern void PrintSimpleAvg(SerialLogWriter *logger, char *log_type, float avg);

extern void PrintSimpleHeader(SerialLogWriter *logger, char *log_type,
                              size_t samples);

extern void PrintSimpleColumns(SerialLogWriter *logger);

#endif
