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

    CSVReader *reader = csv_reader_init_with_config(&arena, config);
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

int main() {
    test_csv_reader_optimized();
    printf("All CSV Reader tests passed!\n");
    return 0;
} 