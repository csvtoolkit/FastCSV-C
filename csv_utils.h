#ifndef CSV_UTILS_H
#define CSV_UTILS_H

#include "csv_config.h"
#include <stddef.h>

typedef enum {
    CSV_UTILS_OK = 0,
    CSV_UTILS_ERROR_NULL_POINTER,
    CSV_UTILS_ERROR_BUFFER_OVERFLOW,
    CSV_UTILS_ERROR_INVALID_INPUT
} CSVUtilsResult;


CSVUtilsResult csv_utils_trim_whitespace(char *str, size_t max_len);
CSVUtilsResult csv_utils_validate_csv_chars(char delimiter, char enclosure, char escape);

bool csv_utils_is_whitespace(char c);
bool csv_utils_needs_escaping(const char *field, char delimiter, char enclosure);
const char* csv_utils_error_string(CSVUtilsResult result);


char* trim_whitespace(char *str);

#endif 
