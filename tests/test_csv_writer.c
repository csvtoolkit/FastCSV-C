#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../csv_writer.h"
#include "../csv_config.h"
#include "../arena.h"

void test_csv_writer_init() {
    printf("Testing csv_writer_init...\n");
    
    Arena arena;
    if (arena_create(&arena, 1024 * 1024) != ARENA_OK) {
        printf("Failed to create arena\n");
        return;
    }
    
    CSVConfig *config = csv_config_create(&arena);
    csv_config_set_path(config, "test_output.csv");
    
    CSVWriter *writer;
    char *headers[] = {"header1", "header2", "header3"};
    CSVWriterResult result = csv_writer_init(&writer, config, headers, 3, &arena);
    
    if (result == CSV_WRITER_OK && writer != NULL) {
        printf("✓ csv_writer_init passed\n");
        csv_writer_free(writer);
    } else {
        printf("✗ csv_writer_init failed\n");
    }
    
    arena_destroy(&arena);
}

void test_csv_writer_init_null_inputs() {
    printf("Testing csv_writer_init with null inputs...\n");
    
    Arena arena;
    if (arena_create(&arena, 1024 * 1024) != ARENA_OK) {
        printf("Failed to create arena\n");
        return;
    }
    
    CSVConfig *config = csv_config_create(&arena);
    csv_config_set_path(config, "test.csv");
    char *headers[] = {"Name", "Age"};
    CSVWriter *writer;
    
    CSVWriterResult result1 = csv_writer_init(NULL, config, headers, 2, &arena);
    assert(result1 == CSV_WRITER_ERROR_NULL_POINTER);
    
    CSVWriterResult result2 = csv_writer_init(&writer, NULL, headers, 2, &arena);
    assert(result2 == CSV_WRITER_ERROR_NULL_POINTER);
    
    CSVWriterResult result3 = csv_writer_init(&writer, config, headers, 2, NULL);
    assert(result3 == CSV_WRITER_ERROR_NULL_POINTER);
    
    arena_destroy(&arena);
    printf("✓ csv_writer_init null inputs test passed\n");
}

void test_csv_writer_init_with_file() {
    printf("Testing csv_writer_init_with_file...\n");
    
    Arena arena;
    if (arena_create(&arena, 1024 * 1024) != ARENA_OK) {
        printf("Failed to create arena\n");
        return;
    }
    
    FILE *file = tmpfile();
    CSVConfig *config = csv_config_create(&arena);
    char *headers[] = {"Col1", "Col2"};
    CSVWriter *writer;
    
    CSVWriterResult result = csv_writer_init_with_file(&writer, file, config, headers, 2, &arena);
    
    assert(result == CSV_WRITER_OK);
    assert(writer != NULL);
    assert(writer->file == file);
    assert(writer->owns_file == false);
    assert(writer->header_count == 2);
    
    csv_writer_free(writer);
    fclose(file);
    arena_destroy(&arena);
    printf("✓ csv_writer_init_with_file test passed\n");
}

void test_csv_writer_write_record() {
    printf("Testing csv_writer_write_record...\n");
    
    Arena arena;
    if (arena_create(&arena, 1024 * 1024) != ARENA_OK) {
        printf("Failed to create arena\n");
        return;
    }
    
    CSVConfig *config = csv_config_create(&arena);
    csv_config_set_path(config, "test_output.csv");
    
    CSVWriter *writer;
    char *headers[] = {"header1", "header2", "header3"};
    CSVWriterResult init_result = csv_writer_init(&writer, config, headers, 3, &arena);
    
    if (init_result != CSV_WRITER_OK || writer == NULL) {
        printf("✗ Failed to initialize writer\n");
        arena_destroy(&arena);
        return;
    }
    
    char *record[] = {"value1", "value2", "value3"};
    CSVWriterResult result = csv_writer_write_record(writer, record, 3);
    
    if (result == CSV_WRITER_OK) {
        printf("✓ csv_writer_write_record passed\n");
    } else {
        printf("✗ csv_writer_write_record failed\n");
    }
    
    csv_writer_free(writer);
    arena_destroy(&arena);
}

void test_csv_writer_write_record_with_quotes() {
    printf("Testing csv_writer_write_record with quotes...\n");
    
    Arena arena;
    if (arena_create(&arena, 1024 * 1024) != ARENA_OK) {
        printf("Failed to create arena\n");
        return;
    }
    
    FILE *file = tmpfile();
    CSVConfig *config = csv_config_create(&arena);
    char *headers[] = {"Name", "Description"};
    CSVWriter *writer;
    
    csv_writer_init_with_file(&writer, file, config, headers, 2, &arena);
    
    char *record[] = {"John Doe", "A person with, comma"};
    CSVWriterResult result = csv_writer_write_record(writer, record, 2);
    assert(result == CSV_WRITER_OK);
    
    csv_writer_flush(writer);
    
    rewind(file);
    char buffer[1000];
    memset(buffer, 0, sizeof(buffer));
    size_t bytes_read = fread(buffer, 1, sizeof(buffer) - 1, file);
    buffer[bytes_read] = '\0';
    
    assert(strstr(buffer, "\"A person with, comma\"") != NULL);
    
    csv_writer_free(writer);
    fclose(file);
    arena_destroy(&arena);
    printf("✓ csv_writer_write_record with quotes test passed\n");
}

void test_csv_writer_write_record_map() {
    printf("Testing csv_writer_write_record_map...\n");
    
    Arena arena;
    if (arena_create(&arena, 1024 * 1024) != ARENA_OK) {
        printf("Failed to create arena\n");
        return;
    }
    
    FILE *file = tmpfile();
    CSVConfig *config = csv_config_create(&arena);
    char *headers[] = {"Name", "Age", "City"};
    CSVWriter *writer;
    
    csv_writer_init_with_file(&writer, file, config, headers, 3, &arena);
    
    char *field_names[] = {"City", "Name", "Age"};
    char *field_values[] = {"Boston", "Alice", "28"};
    
    CSVWriterResult result = csv_writer_write_record_map(writer, field_names, field_values, 3);
    assert(result == CSV_WRITER_OK);
    
    csv_writer_flush(writer);
    
    rewind(file);
    char buffer[1000];
    memset(buffer, 0, sizeof(buffer));
    size_t bytes_read = fread(buffer, 1, sizeof(buffer) - 1, file);
    buffer[bytes_read] = '\0';
    
    assert(strstr(buffer, "Alice,28,Boston") != NULL);
    
    csv_writer_free(writer);
    fclose(file);
    arena_destroy(&arena);
    printf("✓ csv_writer_write_record_map test passed\n");
}

void test_csv_writer_custom_delimiter() {
    printf("Testing csv_writer with custom delimiter...\n");
    
    Arena arena;
    if (arena_create(&arena, 1024 * 1024) != ARENA_OK) {
        printf("Failed to create arena\n");
        return;
    }
    
    FILE *file = tmpfile();
    CSVConfig *config = csv_config_create(&arena);
    csv_config_set_delimiter(config, ';');
    char *headers[] = {"Name", "Age"};
    CSVWriter *writer;
    
    csv_writer_init_with_file(&writer, file, config, headers, 2, &arena);
    
    char *record[] = {"John", "25"};
    CSVWriterResult result = csv_writer_write_record(writer, record, 2);
    assert(result == CSV_WRITER_OK);
    
    csv_writer_flush(writer);
    
    rewind(file);
    char buffer[1000];
    memset(buffer, 0, sizeof(buffer));
    size_t bytes_read = fread(buffer, 1, sizeof(buffer) - 1, file);
    buffer[bytes_read] = '\0';
    
    assert(strstr(buffer, "John;25") != NULL);
    
    csv_writer_free(writer);
    fclose(file);
    arena_destroy(&arena);
    printf("✓ csv_writer custom delimiter test passed\n");
}

void test_csv_writer_custom_enclosure() {
    printf("Testing csv_writer with custom enclosure...\n");
    
    Arena arena;
    if (arena_create(&arena, 1024 * 1024) != ARENA_OK) {
        printf("Failed to create arena\n");
        return;
    }
    
    FILE *file = tmpfile();
    CSVConfig *config = csv_config_create(&arena);
    csv_config_set_enclosure(config, '\'');
    char *headers[] = {"Name", "Description"};
    CSVWriter *writer;
    
    csv_writer_init_with_file(&writer, file, config, headers, 2, &arena);
    
    char *record[] = {"John", "A person with, comma"};
    CSVWriterResult result = csv_writer_write_record(writer, record, 2);
    assert(result == CSV_WRITER_OK);
    
    csv_writer_flush(writer);
    
    rewind(file);
    char buffer[1000];
    memset(buffer, 0, sizeof(buffer));
    size_t bytes_read = fread(buffer, 1, sizeof(buffer) - 1, file);
    buffer[bytes_read] = '\0';
    
    assert(strstr(buffer, "'A person with, comma'") != NULL);
    
    csv_writer_free(writer);
    fclose(file);
    arena_destroy(&arena);
    printf("✓ csv_writer custom enclosure test passed\n");
}

void test_field_needs_quoting() {
    printf("Testing field_needs_quoting...\n");
    
    // Basic quoting tests
    assert(field_needs_quoting("field,with,comma", ',', '"', false));
    assert(field_needs_quoting("field\nwith\nnewline", ',', '"', false));
    assert(field_needs_quoting("field\"with\"quote", ',', '"', false));
    assert(!field_needs_quoting("simple field", ',', '"', false));
    
    // Strict mode tests
    assert(field_needs_quoting("field with space", ',', '"', true));
    assert(!field_needs_quoting("field with space", ',', '"', false));
    assert(!field_needs_quoting("simplefield", ',', '"', true));
    assert(field_needs_quoting("field,comma", ',', '"', true));
    
    printf("✓ field_needs_quoting test passed\n");
}

void test_write_field() {
    printf("Testing write_field...\n");
    
    Arena arena;
    if (arena_create(&arena, 1024 * 1024) != ARENA_OK) {
        printf("Failed to create arena\n");
        return;
    }
    
    FILE *file = tmpfile();
    
    FieldWriteOptions options = {
        .field = "field,with,comma",
        .delimiter = ',',
        .enclosure = '"',
        .escape = '\\',
        .needs_quoting = true,
        .strictMode = false
    };
    
    CSVWriterResult result = write_field(file, &options);
    assert(result == CSV_WRITER_OK);
    
    rewind(file);
    char buffer[1000];
    memset(buffer, 0, sizeof(buffer));
    size_t bytes_read = fread(buffer, 1, sizeof(buffer) - 1, file);
    buffer[bytes_read] = '\0';
    
    assert(strstr(buffer, "\"field,with,comma\"") != NULL);
    
    fclose(file);
    arena_destroy(&arena);
    printf("✓ write_field test passed\n");
}

void test_csv_writer_error_string() {
    printf("Testing csv_writer_error_string...\n");
    
    assert(strcmp(csv_writer_error_string(CSV_WRITER_OK), "Success") == 0);
    assert(strcmp(csv_writer_error_string(CSV_WRITER_ERROR_NULL_POINTER), "Null pointer error") == 0);
    assert(strcmp(csv_writer_error_string(CSV_WRITER_ERROR_MEMORY_ALLOCATION), "Memory allocation failed") == 0);
    assert(strcmp(csv_writer_error_string(CSV_WRITER_ERROR_FILE_OPEN), "Failed to open file") == 0);
    assert(strcmp(csv_writer_error_string(CSV_WRITER_ERROR_FILE_WRITE), "Failed to write to file") == 0);
    assert(strcmp(csv_writer_error_string(CSV_WRITER_ERROR_INVALID_FIELD_COUNT), "Invalid field count") == 0);
    assert(strcmp(csv_writer_error_string(CSV_WRITER_ERROR_FIELD_NOT_FOUND), "Field not found") == 0);
    assert(strcmp(csv_writer_error_string(CSV_WRITER_ERROR_BUFFER_OVERFLOW), "Buffer overflow") == 0);
    assert(strcmp(csv_writer_error_string((CSVWriterResult)999), "Unknown error") == 0);
    
    printf("✓ csv_writer_error_string test passed\n");
}

void test_csv_writer_bom_support() {
    printf("Testing csv_writer BOM support...\n");
    
    Arena arena;
    if (arena_create(&arena, 1024 * 1024) != ARENA_OK) {
        printf("Failed to create arena\n");
        return;
    }
    
    FILE *file = tmpfile();
    CSVConfig *config = csv_config_create(&arena);
    csv_config_set_encoding(config, CSV_ENCODING_UTF8);
    csv_config_set_write_bom(config, true);
    char *headers[] = {"Name", "Age"};
    CSVWriter *writer;
    
    CSVWriterResult result = csv_writer_init_with_file(&writer, file, config, headers, 2, &arena);
    assert(result == CSV_WRITER_OK);
    
    // Write a record
    char *record[] = {"John", "25"};
    result = csv_writer_write_record(writer, record, 2);
    assert(result == CSV_WRITER_OK);
    
    csv_writer_flush(writer);
    
    // Check for UTF-8 BOM at the beginning
    rewind(file);
    unsigned char buffer[10];
    size_t bytes_read = fread(buffer, 1, 3, file);
    assert(bytes_read == 3);
    assert(buffer[0] == 0xEF && buffer[1] == 0xBB && buffer[2] == 0xBF);
    
    csv_writer_free(writer);
    fclose(file);
    arena_destroy(&arena);
    printf("✓ csv_writer BOM support test passed\n");
}

void test_is_numeric_field() {
    printf("Testing is_numeric_field...\n");
    
    // Test numeric fields
    assert(is_numeric_field("123") == true);
    assert(is_numeric_field("123.45") == true);
    assert(is_numeric_field("-123") == true);
    assert(is_numeric_field("+123.45") == true);
    assert(is_numeric_field("0") == true);
    assert(is_numeric_field("0.0") == true);
    
    // Test non-numeric fields
    assert(is_numeric_field("abc") == false);
    assert(is_numeric_field("123abc") == false);
    assert(is_numeric_field("") == false);
    assert(is_numeric_field(NULL) == false);
    assert(is_numeric_field("12.34.56") == false);
    
    // Test whitespace handling
    assert(is_numeric_field("  123  ") == true);
    assert(is_numeric_field("\t-45.67\t") == true);
    
    printf("✓ is_numeric_field test passed\n");
}

void test_csv_writer_encoding_support() {
    printf("Testing csv_writer encoding support...\n");
    
    Arena arena;
    if (arena_create(&arena, 1024 * 1024) != ARENA_OK) {
        printf("Failed to create arena\n");
        return;
    }
    
    // Test different encodings
    CSVEncoding encodings[] = {
        CSV_ENCODING_UTF8,
        CSV_ENCODING_UTF16LE,
        CSV_ENCODING_UTF16BE,
        CSV_ENCODING_UTF32LE,
        CSV_ENCODING_UTF32BE,
        CSV_ENCODING_ASCII,
        CSV_ENCODING_LATIN1
    };
    
    for (int i = 0; i < 7; i++) {
        FILE *file = tmpfile();
        CSVConfig *config = csv_config_create(&arena);
        csv_config_set_encoding(config, encodings[i]);
        char *headers[] = {"Name"};
        CSVWriter *writer;
        
        CSVWriterResult result = csv_writer_init_with_file(&writer, file, config, headers, 1, &arena);
        assert(result == CSV_WRITER_OK);
        
        csv_writer_free(writer);
        fclose(file);
    }
    
    arena_destroy(&arena);
    printf("✓ csv_writer encoding support test passed\n");
}

void test_csv_writer_line_endings() {
    printf("Testing csv_writer line endings...\n");
    
    Arena arena;
    if (arena_create(&arena, 1024 * 1024) != ARENA_OK) {
        printf("Failed to create arena\n");
        return;
    }
    
    FILE *file = tmpfile();
    CSVConfig *config = csv_config_create(&arena);
    char *headers[] = {"Name", "Age"};
    CSVWriter *writer;
    
    csv_writer_init_with_file(&writer, file, config, headers, 2, &arena);
    
    char *record[] = {"John", "25"};
    CSVWriterResult result = csv_writer_write_record(writer, record, 2);
    assert(result == CSV_WRITER_OK);
    
    csv_writer_flush(writer);
    
    rewind(file);
    char buffer[1000];
    memset(buffer, 0, sizeof(buffer));
    size_t bytes_read = fread(buffer, 1, sizeof(buffer) - 1, file);
    buffer[bytes_read] = '\0';
    
    // Should use Unix line endings (\n) not Windows (\r\n)
    assert(strstr(buffer, "\r\n") == NULL);
    assert(strstr(buffer, "\n") != NULL);
    
    csv_writer_free(writer);
    fclose(file);
    arena_destroy(&arena);
    printf("✓ csv_writer line endings test passed\n");
}

int main() {
    printf("Running CSV Writer Tests...\n\n");
    
    test_csv_writer_init();
    test_csv_writer_init_null_inputs();
    test_csv_writer_init_with_file();
    test_csv_writer_write_record();
    test_csv_writer_write_record_with_quotes();
    test_csv_writer_write_record_map();
    test_csv_writer_custom_delimiter();
    test_csv_writer_custom_enclosure();
    test_field_needs_quoting();
    test_write_field();
    test_csv_writer_error_string();
    test_csv_writer_bom_support();
    test_is_numeric_field();
    test_csv_writer_encoding_support();
    test_csv_writer_line_endings();
    
    printf("\n✅ All CSV Writer tests passed!\n");
    return 0;
} 