
#include <SerialLogWriter.h>

/**
 * \brief instantiates with default headears and ',' as delimiter
 *
 *
 */

const char *delimiter_default = ",";
Header headers_default[] = {{"device", (char *)STR(BOARDTYPE)},
                            {"delimiter", delimiter_default}};

SerialLogWriter::SerialLogWriter()
{
  this->default_headers = headers_default;
  this->default_header_len = 2;
  this->delimiter = delimiter_default;
}

/**
 *\brief Constuctor for SerialLogWriter with default parameters
 *\param default_headers array of headers that will be common for every log
 *\param len length of header array
 *\param delimiter delimiter used when writing rows.
 */
SerialLogWriter::SerialLogWriter(Header *default_headers, size_t len,
                                 const char *delimiter)
{
  this->default_headers = default_headers;
  this->default_header_len = len;
  this->delimiter = delimiter;
}
/**
 * \brief Setter for default headers used in every log
 * \param default_headers the new array of headers to be default
 * \param len size of that new array of headers
 */
void SerialLogWriter::set_default_headers(Header *default_headers, size_t len)
{
  this->default_headers = default_headers;
  this->default_header_len = len;
}

/**
 * \brief Prints a row of data to serial using the delimiter
 * \param row array of strings, represents the row that will be commaseperated
 * \param len length of array of strings, ie length of columns.
 *
 */
void SerialLogWriter::PrintRow(const char **row, size_t len)
{
  for (int i = 0; i < len; i++, row++)
  {
    if (i == len - 1)
    {
      FormatPrint("%s\n", *row);
    }
    else
    {
      FormatPrint("%s%s", *row, delimiter);
    }
  }
}

/**
 * \brief Prints the YAML metaheader to Serial for a log, this data
 * is contained in the default headers (common for all log files) and the
 * additional specified by \a headers.
 * \param headers additional headers to beadded.
 * \param len length of \a headers array.
 *
 *
 */
void SerialLogWriter::PrintHeaders(Header *headers, size_t len, bool print_default)
{
  Serial.print("---\n");
  // print default headers
  if (default_headers != NULL && print_default)
  {
    Header *def_ptr = default_headers;
    for (size_t i = 0; i < default_header_len; i++, def_ptr++)
    {
      Header def_header = *def_ptr;
      FormatPrint("\"%s\": \"%s\"\n", def_header.key, def_header.value);
    }
  }
  // print specific headers
  for (size_t i = 0; i < len; i++, headers++)
  {
    Header header = *headers;
    FormatPrint("\"%s\": \"%s\"\n", header.key, header.value);
  }
  Serial.print("...\n");
}

/**
 * \brief Uses SerialLogWriter to print a simple row of sample, time
 * [microseconds] does the job of conversion from int to string for PrintRow
 * function
 * \param logger SerialLogWriter object to use for Serial communication
 * \param sample the current sample/execution number
 * \param execution_time the time it took for function to execute
 *
 */
void PrintSimpleRow(SerialLogWriter *logger, size_t sample,
                    long execution_time)
{
  char current_sample[15];
  char execution_time_str[15];
  snprintf(execution_time_str, 15, "%d", execution_time);
  snprintf(current_sample, 15, "%d", sample);
  const char *row[] = {current_sample, execution_time_str};
  logger->PrintRow(row, 2);
}

/**
 * \brief Prints the columns used for simple benchmarking, sample and
 * time[microseconds]
 * \param logger SerialLogWriter instance to use for serial communication.
 */
void PrintSimpleColumns(SerialLogWriter *logger)
{
  const char *row[] = {"sample", "time [microseconds]"};
  logger->PrintRow(row, 2);
}

/**
 * \brief Used for simple benchmarking incase you only want to print the avg
 * execution time
 * \param logger SerialLogWriter instance to be used for serial communication.
 * \param log_type specifies the primitive/function/scheme that will be logged
 * \param avg The avg execution time for the primitive/function/scheme, to be
 * printed.
 *
 */
void PrintSimpleAvg(SerialLogWriter *logger, const char *log_type, float avg)
{
  int avg_int = (int)round(avg); // round
  char avg_str[15];
  snprintf(avg_str, 15, "%d", avg_int);
  const char *row[] = {log_type, avg_str};
  logger->PrintRow(row, 2);
}

/**
 *  \brief Prints simple headers used for basic benchmarking, always in verbose
 *  \param logger the instance of seriallogwriter to use
 *  \param log_type inserts the name of the log into header
 *  \param samples inserts amount of samples into header
 */
void PrintSimpleHeader(SerialLogWriter *logger, const char *log_type,
                       size_t samples)
{
  char samples_str[15];
  snprintf(samples_str, 15, "%d", samples);
  Header headers[2] = {{"log_type", log_type}, {"samples", samples_str}};
  logger->PrintHeaders(headers, 2, true);
}
