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
    assert(path == NULL || strlen(path) == 0);
    arena_destroy(&arena);
    printf("✓ csv_config defaults passed\n");
}

int main() {
    printf("Running CSVConfig tests...\n\n");
    test_csv_config_create();
    test_csv_config_set_get();
    test_csv_config_copy();
    test_csv_config_defaults();
    printf("\n✅ All CSVConfig tests passed!\n");
    return 0;
} 