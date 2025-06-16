#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../csv_parser.h"
#include "../csv_config.h"
#include "../arena.h"

void test_csv_parser_optimized() {
    printf("Testing optimized CSV parser...\n");
    Arena arena;
    assert(arena_create(&arena, 4096) == ARENA_OK);
    CSVConfig *config = csv_config_create(&arena);
    csv_config_set_delimiter(config, ',');
    csv_config_set_enclosure(config, '"');
    csv_config_set_escape(config, '\\');


    CSVParseResult result1 = csv_parse_line_inplace("a,b,c", &arena, config, 1);
    assert(result1.success == true);
    assert(result1.fields.count == 3);
    assert(strcmp(result1.fields.fields[0], "a") == 0);
    assert(strcmp(result1.fields.fields[1], "b") == 0);
    assert(strcmp(result1.fields.fields[2], "c") == 0);
    printf("✓ Simple line parsing test passed\n");


    CSVParseResult result2 = csv_parse_line_inplace("\"a,b\",\"c\"", &arena, config, 2);
    assert(result2.success == true);
    assert(result2.fields.count == 2);
    assert(strcmp(result2.fields.fields[0], "a,b") == 0);
    assert(strcmp(result2.fields.fields[1], "c") == 0);
    printf("✓ Quoted fields test passed\n");


    CSVParseResult result3 = csv_parse_line_inplace("\"a,b,c", &arena, config, 3);
    assert(result3.success == false);
    assert(result3.error != NULL);
    printf("✓ Error case test passed\n");

    arena_destroy(&arena);
    printf("✓ Optimized CSV parser test passed\n");
}

int main() {
    test_csv_parser_optimized();
    printf("All CSV Parser tests passed!\n");
    return 0;
} 