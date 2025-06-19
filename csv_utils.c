#include "csv_utils.h"
#include <string.h>
#include <ctype.h>

const char* csv_utils_error_string(CSVUtilsResult result) {
    switch (result) {
        case CSV_UTILS_OK: return "Success";
        case CSV_UTILS_ERROR_NULL_POINTER: return "Null pointer error";
        case CSV_UTILS_ERROR_BUFFER_OVERFLOW: return "Buffer overflow";
        case CSV_UTILS_ERROR_INVALID_INPUT: return "Invalid input";
        default: return "Unknown error";
    }
}

bool csv_utils_is_whitespace(char c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

CSVUtilsResult csv_utils_trim_whitespace(char *str, size_t max_len) {
    if (!str) return CSV_UTILS_ERROR_NULL_POINTER;
    if (max_len == 0) return CSV_UTILS_ERROR_INVALID_INPUT;
    
    char *start = str;
    char *end;
    
    while (*start && csv_utils_is_whitespace(*start)) {
        start++;
    }
    
    if (*start == '\0') {
        str[0] = '\0';
        return CSV_UTILS_OK;
    }
    
    end = start + strlen(start) - 1;
    while (end > start && csv_utils_is_whitespace(*end)) {
        end--;
    }
    
    size_t trimmed_len = end - start + 1;
    if (trimmed_len >= max_len) {
        return CSV_UTILS_ERROR_BUFFER_OVERFLOW;
    }
    
    if (start != str) {
        memmove(str, start, trimmed_len);
    }
    str[trimmed_len] = '\0';
    
    return CSV_UTILS_OK;
}

CSVUtilsResult csv_utils_validate_csv_chars(char delimiter, char enclosure, char escape) {
    if (delimiter == enclosure || delimiter == escape || enclosure == escape) {
        return CSV_UTILS_ERROR_INVALID_INPUT;
    }
    
    if (delimiter == '\0' || enclosure == '\0') {
        return CSV_UTILS_ERROR_INVALID_INPUT;
    }
    
    return CSV_UTILS_OK;
}

bool csv_utils_needs_escaping(const char *field, char delimiter, char enclosure) {
    if (!field) return false;
    
    return strchr(field, delimiter) != NULL ||
           strchr(field, enclosure) != NULL ||
           strchr(field, '\r') != NULL ||
           strchr(field, '\n') != NULL;
}

char* trim_whitespace(char *str) {
    char *end;
    
    while (*str == ' ' || *str == '\t' || *str == '\r' || *str == '\n') {
        str++;
    }
    
    if (*str == '\0') {
        return str;
    }
    
    end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n')) {
        end--;
    }
    end[1] = '\0';
    
    return str;
} 
