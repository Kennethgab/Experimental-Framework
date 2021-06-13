
#include <SerialLogWriter.h>

SerialLogWriter::SerialLogWriter(Header *default_headers, size_t len,
                                 char *delimiter) {
  this->default_headers    = default_headers;
  this->default_header_len = len;
  this->delimiter          = delimiter;
}

void SerialLogWriter::set_default_headers(Header *default_headers, size_t len) {
  this->default_headers    = default_headers;
  this->default_header_len = len;
}

void SerialLogWriter::PrintRow(char **row, size_t len) {
  for (int i = 0; i < len; i++, row++) {
    if (i == len - 1) {
      FormatPrint("%s\n", *row);
    } else {
      FormatPrint("%s%s", *row, delimiter);
    }
  }
}

void SerialLogWriter::PrintHeaders(Header *headers, size_t len) {
  Serial.print("---\n");
  // print default headers
  if (default_headers != NULL) {
    Header *def_ptr = default_headers;
    for (size_t i = 0; i < default_header_len; i++, def_ptr++) {
      Header def_header = *def_ptr;
      FormatPrint("\"%s\": \"%s\"\n", def_header.key, def_header.value);
    }
  }
  // print specific headers
  for (size_t i = 0; i < len; i++, headers++) {
    Header header = *headers;
    FormatPrint("\"%s\": \"%s\"\n", header.key, header.value);
  }
  Serial.print("...\n");
}

void PrintSimpleRow(SerialLogWriter *logger, size_t sample,
                    long execution_time) {
  char current_sample[15];
  char execution_time_str[15];
  snprintf(execution_time_str, 15, "%d", execution_time);
  snprintf(current_sample, 15, "%d", sample);
  char *row[] = {current_sample, execution_time_str};
  logger->PrintRow(row, 2);
}

void PrintSimpleColumns(SerialLogWriter *logger) {
  char *row[] = {"sample", "time [microseconds]"};
  logger->PrintRow(row, 2);
}

void PrintSimpleAvg(SerialLogWriter *logger, char *log_type, float avg) {
  int avg_int = (int)round(avg);  // round
  char avg_str[15];
  snprintf(avg_str, 15, "%d", avg_int);
  char *row[] = {log_type, avg_str};
  logger->PrintRow(row, 2);
}

void PrintSimpleHeader(SerialLogWriter *logger, char *log_type,
                       size_t samples) {
  char samples_str[15];
  snprintf(samples_str, 15, "%d", samples);
  Header headers[2] = {{"log_type", log_type}, {"samples", samples_str}};
  logger->PrintHeaders(headers, 2);
}
