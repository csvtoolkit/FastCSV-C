#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../arena.h"
#include "../csv_config.h"

void test_csv_config_create() {
    printf("Testing csv_config_create...\n");
    Arena arena;
    assert(arena_create(&arena, 4096) == ARENA_OK);
    CSVConfig *config = csv_config_create(&arena);
    assert(config != NULL);
    arena_destroy(&arena);
    printf("✓ csv_config_create passed\n");
}

void test_csv_config_set_get() {
    printf("Testing csv_config_set/get...\n");
    Arena arena;
    assert(arena_create(&arena, 4096) == ARENA_OK);
    CSVConfig *config = csv_config_create(&arena);
    csv_config_set_delimiter(config, ';');
    csv_config_set_enclosure(config, '\'');
    csv_config_set_escape(config, '\\');
    csv_config_set_path(config, "test.csv");
    assert(csv_config_get_delimiter(config) == ';');
    assert(csv_config_get_enclosure(config) == '\'');
    assert(csv_config_get_escape(config) == '\\');
    assert(strcmp(csv_config_get_path(config), "test.csv") == 0);
    arena_destroy(&arena);
    printf("✓ csv_config_set/get passed\n");
}

void test_csv_config_copy() {
    printf("Testing csv_config_copy...\n");
    Arena arena;
    assert(arena_create(&arena, 4096) == ARENA_OK);
    CSVConfig *original = csv_config_create(&arena);
    csv_config_set_delimiter(original, '|');
    csv_config_set_enclosure(original, '"');
    csv_config_set_escape(original, '/');
    csv_config_set_path(original, "copy.csv");
    CSVConfig *copy = csv_config_copy(&arena, original);
    assert(copy != NULL);
    assert(csv_config_get_delimiter(copy) == '|');
    assert(csv_config_get_enclosure(copy) == '"');
    assert(csv_config_get_escape(copy) == '/');
    assert(strcmp(csv_config_get_path(copy), "copy.csv") == 0);
    arena_destroy(&arena);
    printf("✓ csv_config_copy passed\n");
}

void test_csv_config_defaults() {
    printf("Testing csv_config defaults...\n");
    Arena arena;
    assert(arena_create(&arena, 4096) == ARENA_OK);
    CSVConfig *config = csv_config_create(&arena);
    assert(csv_config_get_delimiter(config) == ',');
    assert(csv_config_get_enclosure(config) == '"');
    assert(csv_config_get_escape(config) == '"');
    const char *path = csv_config_get_path(config);
    assert(path != NULL && strlen(path) == 0);
    assert(csv_config_has_header(config) == true);
    assert(csv_config_get_encoding(config) == CSV_ENCODING_UTF8);
    assert(csv_config_get_write_bom(config) == false);
    assert(csv_config_get_strict_mode(config) == false);
    assert(csv_config_get_skip_empty_lines(config) == false);
    assert(csv_config_get_trim_fields(config) == false);
    assert(csv_config_get_preserve_quotes(config) == false);
    arena_destroy(&arena);
    printf("✓ csv_config defaults passed\n");
}

void test_csv_config_encoding() {
    printf("Testing csv_config encoding functions...\n");
    Arena arena;
    assert(arena_create(&arena, 4096) == ARENA_OK);
    CSVConfig *config = csv_config_create(&arena);
    
    csv_config_set_encoding(config, CSV_ENCODING_UTF16LE);
    assert(csv_config_get_encoding(config) == CSV_ENCODING_UTF16LE);
    
    csv_config_set_encoding(config, CSV_ENCODING_UTF16BE);
    assert(csv_config_get_encoding(config) == CSV_ENCODING_UTF16BE);
    
    csv_config_set_encoding(config, CSV_ENCODING_UTF32LE);
    assert(csv_config_get_encoding(config) == CSV_ENCODING_UTF32LE);
    
    csv_config_set_encoding(config, CSV_ENCODING_UTF32BE);
    assert(csv_config_get_encoding(config) == CSV_ENCODING_UTF32BE);
    
    csv_config_set_encoding(config, CSV_ENCODING_ASCII);
    assert(csv_config_get_encoding(config) == CSV_ENCODING_ASCII);
    
    csv_config_set_encoding(config, CSV_ENCODING_LATIN1);
    assert(csv_config_get_encoding(config) == CSV_ENCODING_LATIN1);
    
    arena_destroy(&arena);
    printf("✓ csv_config encoding functions passed\n");
}

void test_csv_config_boolean_flags() {
    printf("Testing csv_config boolean flags...\n");
    Arena arena;
    assert(arena_create(&arena, 4096) == ARENA_OK);
    CSVConfig *config = csv_config_create(&arena);
    
    csv_config_set_write_bom(config, true);
    assert(csv_config_get_write_bom(config) == true);
    csv_config_set_write_bom(config, false);
    assert(csv_config_get_write_bom(config) == false);
    
    csv_config_set_strict_mode(config, true);
    assert(csv_config_get_strict_mode(config) == true);
    csv_config_set_strict_mode(config, false);
    assert(csv_config_get_strict_mode(config) == false);
    
    csv_config_set_skip_empty_lines(config, true);
    assert(csv_config_get_skip_empty_lines(config) == true);
    csv_config_set_skip_empty_lines(config, false);
    assert(csv_config_get_skip_empty_lines(config) == false);
    
    csv_config_set_trim_fields(config, true);
    assert(csv_config_get_trim_fields(config) == true);
    csv_config_set_trim_fields(config, false);
    assert(csv_config_get_trim_fields(config) == false);
    
    csv_config_set_preserve_quotes(config, true);
    assert(csv_config_get_preserve_quotes(config) == true);
    csv_config_set_preserve_quotes(config, false);
    assert(csv_config_get_preserve_quotes(config) == false);
    
    arena_destroy(&arena);
    printf("✓ csv_config boolean flags passed\n");
}

void test_csv_config_null_safety() {
    printf("Testing csv_config null safety...\n");
    
    assert(csv_config_get_delimiter(NULL) == ',');
    assert(csv_config_get_enclosure(NULL) == '"');
    assert(csv_config_get_escape(NULL) == '"');
    assert(csv_config_get_path(NULL) == NULL);
    assert(csv_config_get_offset(NULL) == 0);
    assert(csv_config_get_limit(NULL) == 0);
    assert(csv_config_has_header(NULL) == false);
    assert(csv_config_get_encoding(NULL) == CSV_ENCODING_UTF8);
    assert(csv_config_get_write_bom(NULL) == false);
    assert(csv_config_get_strict_mode(NULL) == true);
    assert(csv_config_get_skip_empty_lines(NULL) == false);
    assert(csv_config_get_trim_fields(NULL) == false);
    assert(csv_config_get_preserve_quotes(NULL) == false);
    
    csv_config_set_delimiter(NULL, ';');
    csv_config_set_enclosure(NULL, '\'');
    csv_config_set_escape(NULL, '\\');
    csv_config_set_path(NULL, "test.csv");
    csv_config_set_offset(NULL, 10);
    csv_config_set_limit(NULL, 100);
    csv_config_set_has_header(NULL, true);
    csv_config_set_encoding(NULL, CSV_ENCODING_UTF16LE);
    csv_config_set_write_bom(NULL, true);
    csv_config_set_strict_mode(NULL, true);
    csv_config_set_skip_empty_lines(NULL, true);
    csv_config_set_trim_fields(NULL, true);
    csv_config_set_preserve_quotes(NULL, true);
    
    printf("✓ csv_config null safety passed\n");
}

int main() {
    printf("Running CSVConfig tests...\n\n");
    test_csv_config_create();
    test_csv_config_set_get();
    test_csv_config_copy();
    test_csv_config_defaults();
    test_csv_config_encoding();
    test_csv_config_boolean_flags();
    test_csv_config_null_safety();
    printf("\n✅ All CSVConfig tests passed!\n");
    return 0;
} 