#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../csv_reader.h"
#include "../csv_config.h"
#include "../arena.h"

void create_test_csv_file(const char *filename, const char *content) {
    FILE *file = fopen(filename, "w");
    if (file) {
        fputs(content, file);
        fclose(file);
    }
}

void test_csv_reader_optimized() {
    printf("Testing optimized CSV reader...\n");
    const char *test_content = "Name,Age,City\nJohn,25,New York\nJane,30,Los Angeles\n";
    create_test_csv_file("test_reader.csv", test_content);

    Arena arena;
    assert(arena_create(&arena, 4096) == ARENA_OK);
    CSVConfig *config = csv_config_create(&arena);
    csv_config_set_path(config, "test_reader.csv");
    csv_config_set_has_header(config, true);

    CSVReader *reader = csv_reader_init_standalone(config);
    assert(reader != NULL);
    assert(reader->headers_loaded == true);
    assert(reader->cached_header_count == 3);
    assert(strcmp(reader->cached_headers[0], "Name") == 0);
    assert(strcmp(reader->cached_headers[1], "Age") == 0);
    assert(strcmp(reader->cached_headers[2], "City") == 0);

    CSVRecord *record1 = csv_reader_next_record(reader);
    assert(record1 != NULL);
    assert(record1->field_count == 3);
    assert(strcmp(record1->fields[0], "John") == 0);
    assert(strcmp(record1->fields[1], "25") == 0);
    assert(strcmp(record1->fields[2], "New York") == 0);

    CSVRecord *record2 = csv_reader_next_record(reader);
    assert(record2 != NULL);
    assert(record2->field_count == 3);
    assert(strcmp(record2->fields[0], "Jane") == 0);
    assert(strcmp(record2->fields[1], "30") == 0);
    assert(strcmp(record2->fields[2], "Los Angeles") == 0);

    CSVRecord *record3 = csv_reader_next_record(reader);
    assert(record3 == NULL);

    csv_reader_free(reader);
    arena_destroy(&arena);
    remove("test_reader.csv");
    printf("✓ Optimized CSV reader test passed\n");
}

void test_csv_reader_get_headers() {
    printf("Testing csv_reader_get_headers...\n");
    const char *test_content = "ID,Name,Email\n1,Alice,alice@example.com\n2,Bob,bob@example.com\n";
    create_test_csv_file("test_headers.csv", test_content);

    Arena arena;
    assert(arena_create(&arena, 4096) == ARENA_OK);
    CSVConfig *config = csv_config_create(&arena);
    csv_config_set_path(config, "test_headers.csv");
    csv_config_set_has_header(config, true);

    CSVReader *reader = csv_reader_init_standalone(config);
    assert(reader != NULL);

    int header_count = 0;
    char **headers = csv_reader_get_headers(reader, &header_count);
    assert(headers != NULL);
    assert(header_count == 3);
    assert(strcmp(headers[0], "ID") == 0);
    assert(strcmp(headers[1], "Name") == 0);
    assert(strcmp(headers[2], "Email") == 0);

    csv_reader_free(reader);
    arena_destroy(&arena);
    remove("test_headers.csv");
    printf("✓ csv_reader_get_headers test passed\n");
}

void test_csv_reader_rewind() {
    printf("Testing csv_reader_rewind...\n");
    const char *test_content = "Name,Age\nAlice,25\nBob,30\nCharlie,35\n";
    create_test_csv_file("test_rewind.csv", test_content);

    Arena arena;
    assert(arena_create(&arena, 4096) == ARENA_OK);
    CSVConfig *config = csv_config_create(&arena);
    csv_config_set_path(config, "test_rewind.csv");
    csv_config_set_has_header(config, true);

    CSVReader *reader = csv_reader_init_standalone(config);
    assert(reader != NULL);

    // Read first record
    CSVRecord *record1 = csv_reader_next_record(reader);
    assert(record1 != NULL);
    assert(strcmp(record1->fields[0], "Alice") == 0);

    // Read second record
    CSVRecord *record2 = csv_reader_next_record(reader);
    assert(record2 != NULL);
    assert(strcmp(record2->fields[0], "Bob") == 0);

    // Rewind and read first record again
    csv_reader_rewind(reader);
    CSVRecord *record_after_rewind = csv_reader_next_record(reader);
    assert(record_after_rewind != NULL);
    assert(strcmp(record_after_rewind->fields[0], "Alice") == 0);

    csv_reader_free(reader);
    arena_destroy(&arena);
    remove("test_rewind.csv");
    printf("✓ csv_reader_rewind test passed\n");
}

void test_csv_reader_has_next() {
    printf("Testing csv_reader_has_next...\n");
    const char *test_content = "Name,Age\nAlice,25\nBob,30\n";
    create_test_csv_file("test_has_next.csv", test_content);

    Arena arena;
    assert(arena_create(&arena, 4096) == ARENA_OK);
    CSVConfig *config = csv_config_create(&arena);
    csv_config_set_path(config, "test_has_next.csv");
    csv_config_set_has_header(config, true);

    CSVReader *reader = csv_reader_init_standalone(config);
    assert(reader != NULL);

    // Should have records
    assert(csv_reader_has_next(reader) == 1);

    // Read first record
    CSVRecord *record1 = csv_reader_next_record(reader);
    assert(record1 != NULL);
    assert(csv_reader_has_next(reader) == 1);

    // Read second record
    CSVRecord *record2 = csv_reader_next_record(reader);
    assert(record2 != NULL);
    assert(csv_reader_has_next(reader) == 0);

    // No more records
    CSVRecord *record3 = csv_reader_next_record(reader);
    assert(record3 == NULL);
    assert(csv_reader_has_next(reader) == 0);

    csv_reader_free(reader);
    arena_destroy(&arena);
    remove("test_has_next.csv");
    printf("✓ csv_reader_has_next test passed\n");
}

void test_csv_reader_seek() {
    printf("Testing csv_reader_seek...\n");
    const char *test_content = "Name,Age\nAlice,25\nBob,30\nCharlie,35\nDavid,40\n";
    create_test_csv_file("test_seek.csv", test_content);

    Arena arena;
    assert(arena_create(&arena, 4096) == ARENA_OK);
    CSVConfig *config = csv_config_create(&arena);
    csv_config_set_path(config, "test_seek.csv");
    csv_config_set_has_header(config, true);

    CSVReader *reader = csv_reader_init_standalone(config);
    assert(reader != NULL);

    // Seek to position 2 (3rd data record)
    int seek_result = csv_reader_seek(reader, 2);
    assert(seek_result == 1);

    // Should now read Charlie
    CSVRecord *record = csv_reader_next_record(reader);
    assert(record != NULL);
    assert(strcmp(record->fields[0], "Charlie") == 0);

    // Test seeking beyond available records
    int invalid_seek = csv_reader_seek(reader, 100);
    assert(invalid_seek == 0);

    csv_reader_free(reader);
    arena_destroy(&arena);
    remove("test_seek.csv");
    printf("✓ csv_reader_seek test passed\n");
}

void test_csv_reader_position() {
    printf("Testing csv_reader_get_position...\n");
    const char *test_content = "Name,Age\nAlice,25\nBob,30\nCharlie,35\n";
    create_test_csv_file("test_position.csv", test_content);

    Arena arena;
    assert(arena_create(&arena, 4096) == ARENA_OK);
    CSVConfig *config = csv_config_create(&arena);
    csv_config_set_path(config, "test_position.csv");
    csv_config_set_has_header(config, true);

    CSVReader *reader = csv_reader_init_standalone(config);
    assert(reader != NULL);

    // Initial position should be 1 (after header)
    assert(csv_reader_get_position(reader) == 1);

    // Read first record
    CSVRecord *record1 = csv_reader_next_record(reader);
    assert(record1 != NULL);
    assert(csv_reader_get_position(reader) == 2);

    // Read second record
    CSVRecord *record2 = csv_reader_next_record(reader);
    assert(record2 != NULL);
    assert(csv_reader_get_position(reader) == 3);

    csv_reader_free(reader);
    arena_destroy(&arena);
    remove("test_position.csv");
    printf("✓ csv_reader_get_position test passed\n");
}

void test_csv_reader_set_config() {
    printf("Testing csv_reader_set_config...\n");
    const char *test_content = "Name,Age\nAlice,25\nBob,30\n";
    create_test_csv_file("test_set_config.csv", test_content);
    
    Arena arena;
    assert(arena_create(&arena, 4096) == ARENA_OK);
    
    CSVConfig *config1 = csv_config_create(&arena);
    csv_config_set_delimiter(config1, ',');
    csv_config_set_path(config1, "test_set_config.csv");
    csv_config_set_has_header(config1, true);
    
    CSVConfig *config2 = csv_config_create(&arena);
    csv_config_set_delimiter(config2, ';');
    csv_config_set_path(config2, "test_set_config.csv");
    csv_config_set_has_header(config2, true);

    CSVReader *reader = csv_reader_init_standalone(config1);
    assert(reader != NULL);
    assert(reader->config->delimiter == ',');

    // For standalone mode, we can't change config after init
    // So we'll test creating a new reader with different config
    csv_reader_free(reader);
    
    reader = csv_reader_init_standalone(config2);
    assert(reader != NULL);
    assert(reader->config->delimiter == ';');

    csv_reader_free(reader);
    arena_destroy(&arena);
    remove("test_set_config.csv");
    printf("✓ csv_reader_set_config test passed\n");
}

void test_csv_reader_null_safety() {
    printf("Testing csv_reader null safety...\n");
    
    // Test functions with NULL reader
    int header_count = 0;
    assert(csv_reader_get_headers(NULL, &header_count) == NULL);
    assert(csv_reader_get_headers(NULL, NULL) == NULL);
    
    csv_reader_rewind(NULL); // Should not crash
    
    assert(csv_reader_get_record_count(NULL) == -1);
    assert(csv_reader_get_position(NULL) == -1);
    assert(csv_reader_seek(NULL, 0) == 0);
    assert(csv_reader_has_next(NULL) == 0);
    
    csv_reader_free(NULL); // Should not crash
    
    printf("✓ csv_reader null safety test passed\n");
}

void test_csv_reader_get_record_count() {
    printf("Testing csv_reader_get_record_count...\n");
    
    // Test 1: CSV with header
    const char *test_content_with_header = "Name,Age,City\nAlice,25,New York\nBob,30,London\nCharlie,35,Paris\n";
    create_test_csv_file("test_count_header.csv", test_content_with_header);

    Arena arena;
    assert(arena_create(&arena, 4096) == ARENA_OK);
    CSVConfig *config = csv_config_create(&arena);
    csv_config_set_path(config, "test_count_header.csv");
    csv_config_set_has_header(config, true);

    CSVReader *reader = csv_reader_init_standalone(config);
    assert(reader != NULL);

    long count = csv_reader_get_record_count(reader);
    assert(count == 3); // Should count 3 data records, excluding header

    csv_reader_free(reader);
    arena_destroy(&arena);
    remove("test_count_header.csv");
    
    // Test 2: CSV without header
    const char *test_content_no_header = "Alice,25,New York\nBob,30,London\nCharlie,35,Paris\n";
    create_test_csv_file("test_count_no_header.csv", test_content_no_header);

    assert(arena_create(&arena, 4096) == ARENA_OK);
    config = csv_config_create(&arena);
    csv_config_set_path(config, "test_count_no_header.csv");
    csv_config_set_has_header(config, false);

    reader = csv_reader_init_standalone(config);
    assert(reader != NULL);

    count = csv_reader_get_record_count(reader);
    assert(count == 3); // Should count 3 records, no header to skip

    csv_reader_free(reader);
    arena_destroy(&arena);
    remove("test_count_no_header.csv");
    
    // Test 3: Empty file
    create_test_csv_file("test_count_empty.csv", "");

    assert(arena_create(&arena, 4096) == ARENA_OK);
    config = csv_config_create(&arena);
    csv_config_set_path(config, "test_count_empty.csv");
    csv_config_set_has_header(config, false);

    reader = csv_reader_init_standalone(config);
    assert(reader != NULL);

    count = csv_reader_get_record_count(reader);
    assert(count == 0); // Empty file should return 0

    csv_reader_free(reader);
    arena_destroy(&arena);
    remove("test_count_empty.csv");
    
    // Test 4: CSV with empty lines (skip empty lines enabled)
    const char *test_content_empty_lines = "Name,Age\nAlice,25\n\nBob,30\n\n\nCharlie,35\n";
    create_test_csv_file("test_count_empty_lines.csv", test_content_empty_lines);

    assert(arena_create(&arena, 4096) == ARENA_OK);
    config = csv_config_create(&arena);
    csv_config_set_path(config, "test_count_empty_lines.csv");
    csv_config_set_has_header(config, true);
    csv_config_set_skip_empty_lines(config, true);

    reader = csv_reader_init_standalone(config);
    assert(reader != NULL);

    count = csv_reader_get_record_count(reader);
    assert(count == 3); // Should skip empty lines and count 3 data records

    csv_reader_free(reader);
    arena_destroy(&arena);
    remove("test_count_empty_lines.csv");
    
    printf("✓ csv_reader_get_record_count test passed\n");
}

int main() {
    printf("Running CSV Reader tests...\n\n");
    test_csv_reader_optimized();
    test_csv_reader_get_headers();
    test_csv_reader_rewind();
    test_csv_reader_has_next();
    test_csv_reader_seek();
    test_csv_reader_position();
    test_csv_reader_set_config();
    test_csv_reader_get_record_count();
    test_csv_reader_null_safety();
    printf("\n✅ All CSV Reader tests passed!\n");
    return 0;
} 