#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../csv_utils.h"

void test_csv_utils_is_whitespace() {
    printf("Testing csv_utils_is_whitespace...\n");
    
    assert(csv_utils_is_whitespace(' ') == true);
    assert(csv_utils_is_whitespace('\t') == true);
    assert(csv_utils_is_whitespace('\r') == true);
    assert(csv_utils_is_whitespace('\n') == true);
    
    assert(csv_utils_is_whitespace('a') == false);
    assert(csv_utils_is_whitespace('1') == false);
    assert(csv_utils_is_whitespace(',') == false);
    assert(csv_utils_is_whitespace('"') == false);
    assert(csv_utils_is_whitespace('\0') == false);
    
    printf("✓ csv_utils_is_whitespace passed\n");
}

void test_csv_utils_trim_whitespace() {
    printf("Testing csv_utils_trim_whitespace...\n");
    
    char test1[] = "  hello world  ";
    CSVUtilsResult result1 = csv_utils_trim_whitespace(test1, sizeof(test1));
    assert(result1 == CSV_UTILS_OK);
    assert(strcmp(test1, "hello world") == 0);
    
    char test2[] = "\t\r\ntest\t\r\n";
    CSVUtilsResult result2 = csv_utils_trim_whitespace(test2, sizeof(test2));
    assert(result2 == CSV_UTILS_OK);
    assert(strcmp(test2, "test") == 0);
    
    char test3[] = "no_whitespace";
    CSVUtilsResult result3 = csv_utils_trim_whitespace(test3, sizeof(test3));
    assert(result3 == CSV_UTILS_OK);
    assert(strcmp(test3, "no_whitespace") == 0);
    
    char test4[] = "   ";
    CSVUtilsResult result4 = csv_utils_trim_whitespace(test4, sizeof(test4));
    assert(result4 == CSV_UTILS_OK);
    assert(strcmp(test4, "") == 0);
    
    printf("✓ csv_utils_trim_whitespace passed\n");
}

void test_csv_utils_trim_whitespace_null() {
    printf("Testing csv_utils_trim_whitespace with null...\n");
    
    CSVUtilsResult result = csv_utils_trim_whitespace(NULL, 100);
    assert(result == CSV_UTILS_ERROR_NULL_POINTER);
    
    printf("✓ csv_utils_trim_whitespace null test passed\n");
}

void test_csv_utils_trim_whitespace_zero_size() {
    printf("Testing csv_utils_trim_whitespace with zero size...\n");
    
    char test[] = "test";
    CSVUtilsResult result = csv_utils_trim_whitespace(test, 0);
    assert(result == CSV_UTILS_ERROR_INVALID_INPUT);
    
    printf("✓ csv_utils_trim_whitespace zero size test passed\n");
}

void test_csv_utils_trim_whitespace_buffer_overflow() {
    printf("Testing csv_utils_trim_whitespace buffer overflow...\n");
    
    char test[] = "  very long string that should cause overflow  ";
    CSVUtilsResult result = csv_utils_trim_whitespace(test, 5);
    assert(result == CSV_UTILS_ERROR_BUFFER_OVERFLOW);
    
    printf("✓ csv_utils_trim_whitespace buffer overflow test passed\n");
}

void test_csv_utils_validate_csv_chars() {
    printf("Testing csv_utils_validate_csv_chars...\n");
    
    CSVUtilsResult result1 = csv_utils_validate_csv_chars(',', '"', '\\');
    assert(result1 == CSV_UTILS_OK);
    
    CSVUtilsResult result2 = csv_utils_validate_csv_chars(';', '\'', '\\');
    assert(result2 == CSV_UTILS_OK);
    
    CSVUtilsResult result3 = csv_utils_validate_csv_chars('\t', '"', '\\');
    assert(result3 == CSV_UTILS_OK);
    
    printf("✓ csv_utils_validate_csv_chars passed\n");
}

void test_csv_utils_validate_csv_chars_invalid() {
    printf("Testing csv_utils_validate_csv_chars with invalid chars...\n");
    
    CSVUtilsResult result1 = csv_utils_validate_csv_chars(',', ',', '"');
    assert(result1 == CSV_UTILS_ERROR_INVALID_INPUT);
    
    CSVUtilsResult result2 = csv_utils_validate_csv_chars(',', '"', ',');
    assert(result2 == CSV_UTILS_ERROR_INVALID_INPUT);
    
    CSVUtilsResult result3 = csv_utils_validate_csv_chars(',', '"', ',');
    assert(result3 == CSV_UTILS_ERROR_INVALID_INPUT);
    
    CSVUtilsResult result4 = csv_utils_validate_csv_chars('\0', '"', '\\');
    assert(result4 == CSV_UTILS_ERROR_INVALID_INPUT);
    
    CSVUtilsResult result5 = csv_utils_validate_csv_chars(',', '\0', '\\');
    assert(result5 == CSV_UTILS_ERROR_INVALID_INPUT);
    
    printf("✓ csv_utils_validate_csv_chars invalid test passed\n");
}

void test_csv_utils_needs_escaping() {
    printf("Testing csv_utils_needs_escaping...\n");
    
    assert(csv_utils_needs_escaping("hello,world", ',', '"') == true);
    assert(csv_utils_needs_escaping("hello\"world", ',', '"') == true);
    assert(csv_utils_needs_escaping("hello\rworld", ',', '"') == true);
    assert(csv_utils_needs_escaping("hello\nworld", ',', '"') == true);
    
    assert(csv_utils_needs_escaping("hello world", ',', '"') == false);
    assert(csv_utils_needs_escaping("simple", ',', '"') == false);
    assert(csv_utils_needs_escaping("123", ',', '"') == false);
    
    assert(csv_utils_needs_escaping(NULL, ',', '"') == false);
    
    printf("✓ csv_utils_needs_escaping passed\n");
}

void test_csv_utils_needs_escaping_different_chars() {
    printf("Testing csv_utils_needs_escaping with different chars...\n");
    
    assert(csv_utils_needs_escaping("hello;world", ';', '\'') == true);
    assert(csv_utils_needs_escaping("hello'world", ';', '\'') == true);
    assert(csv_utils_needs_escaping("hello\tworld", '\t', '"') == true);
    
    assert(csv_utils_needs_escaping("hello,world", ';', '\'') == false);
    assert(csv_utils_needs_escaping("hello\"world", ';', '\'') == false);
    
    printf("✓ csv_utils_needs_escaping different chars test passed\n");
}

void test_trim_whitespace_legacy() {
    printf("Testing trim_whitespace (legacy function)...\n");
    
    char test1[] = "  hello world  ";
    char *result1 = trim_whitespace(test1);
    assert(strcmp(result1, "hello world") == 0);
    
    char test2[] = "\t\r\ntest\t\r\n";
    char *result2 = trim_whitespace(test2);
    assert(strcmp(result2, "test") == 0);
    
    char test3[] = "no_whitespace";
    char *result3 = trim_whitespace(test3);
    assert(strcmp(result3, "no_whitespace") == 0);
    
    char test4[] = "   ";
    char *result4 = trim_whitespace(test4);
    assert(strcmp(result4, "") == 0);
    
    printf("✓ trim_whitespace legacy function passed\n");
}

void test_csv_utils_error_string() {
    printf("Testing csv_utils_error_string...\n");
    
    const char *msg1 = csv_utils_error_string(CSV_UTILS_OK);
    assert(strcmp(msg1, "Success") == 0);
    
    const char *msg2 = csv_utils_error_string(CSV_UTILS_ERROR_NULL_POINTER);
    assert(strcmp(msg2, "Null pointer error") == 0);
    
    const char *msg3 = csv_utils_error_string(CSV_UTILS_ERROR_BUFFER_OVERFLOW);
    assert(strcmp(msg3, "Buffer overflow") == 0);
    
    const char *msg4 = csv_utils_error_string(CSV_UTILS_ERROR_INVALID_INPUT);
    assert(strcmp(msg4, "Invalid input") == 0);
    
    const char *msg5 = csv_utils_error_string((CSVUtilsResult)999);
    assert(strcmp(msg5, "Unknown error") == 0);
    
    printf("✓ csv_utils_error_string passed\n");
}

int main() {
    printf("Running CSV Utils Tests...\n\n");
    
    test_csv_utils_is_whitespace();
    test_csv_utils_trim_whitespace();
    test_csv_utils_trim_whitespace_null();
    test_csv_utils_trim_whitespace_zero_size();
    test_csv_utils_trim_whitespace_buffer_overflow();
    test_csv_utils_validate_csv_chars();
    test_csv_utils_validate_csv_chars_invalid();
    test_csv_utils_needs_escaping();
    test_csv_utils_needs_escaping_different_chars();
    test_trim_whitespace_legacy();
    test_csv_utils_error_string();
    
    printf("\n✅ All CSV Utils tests passed!\n");
    return 0;
} 