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

void test_csv_parser_escaped_quotes() {
    printf("Testing CSV parser with escaped quotes...\n");
    Arena arena;
    assert(arena_create(&arena, 4096) == ARENA_OK);
    CSVConfig *config = csv_config_create(&arena);
    
    // Test RFC 4180 style double quote escaping
    CSVParseResult result1 = csv_parse_line_inplace("\"Say \"\"Hello\"\" World\",normal", &arena, config, 1);
    assert(result1.success == true);
    assert(result1.fields.count == 2);
    assert(strcmp(result1.fields.fields[0], "Say \"Hello\" World") == 0);
    assert(strcmp(result1.fields.fields[1], "normal") == 0);
    
    // Test multiple escaped quotes
    CSVParseResult result2 = csv_parse_line_inplace("\"\"\"quoted\"\"\",\"test\"", &arena, config, 2);
    assert(result2.success == true);
    assert(result2.fields.count == 2);
    assert(strcmp(result2.fields.fields[0], "\"quoted\"") == 0);
    assert(strcmp(result2.fields.fields[1], "test") == 0);
    
    arena_destroy(&arena);
    printf("✓ CSV parser escaped quotes test passed\n");
}

void test_csv_parser_whitespace_trimming() {
    printf("Testing CSV parser whitespace trimming...\n");
    Arena arena;
    assert(arena_create(&arena, 4096) == ARENA_OK);
    CSVConfig *config = csv_config_create(&arena);
    
    // Test trailing whitespace trimming (parser only trims trailing, not leading)
    CSVParseResult result1 = csv_parse_line_inplace("  field1  ,  field2  ,  field3  ", &arena, config, 1);
    assert(result1.success == true);
    assert(result1.fields.count == 3);
    assert(strcmp(result1.fields.fields[0], "  field1") == 0);  // Leading spaces preserved
    assert(strcmp(result1.fields.fields[1], "  field2") == 0);  // Leading spaces preserved
    assert(strcmp(result1.fields.fields[2], "  field3") == 0);  // Leading spaces preserved
    
    // Test with quoted fields (should not trim inside quotes)
    CSVParseResult result2 = csv_parse_line_inplace("\"  field1  \",  field2  ", &arena, config, 2);
    assert(result2.success == true);
    assert(result2.fields.count == 2);
    assert(strcmp(result2.fields.fields[0], "  field1  ") == 0);
    assert(strcmp(result2.fields.fields[1], "  field2") == 0);
    
    // Test pure trailing whitespace trimming
    CSVParseResult result3 = csv_parse_line_inplace("field1   ,field2\t\t,field3 ", &arena, config, 3);
    assert(result3.success == true);
    assert(result3.fields.count == 3);
    assert(strcmp(result3.fields.fields[0], "field1") == 0);  // Trailing spaces trimmed
    assert(strcmp(result3.fields.fields[1], "field2") == 0);  // Trailing tabs trimmed
    assert(strcmp(result3.fields.fields[2], "field3") == 0);  // Trailing space trimmed
    
    arena_destroy(&arena);
    printf("✓ CSV parser whitespace trimming test passed\n");
}

void test_csv_parser_empty_fields() {
    printf("Testing CSV parser with empty fields...\n");
    Arena arena;
    assert(arena_create(&arena, 4096) == ARENA_OK);
    CSVConfig *config = csv_config_create(&arena);
    
    // Test empty fields
    CSVParseResult result1 = csv_parse_line_inplace("a,,c", &arena, config, 1);
    assert(result1.success == true);
    assert(result1.fields.count == 3);
    assert(strcmp(result1.fields.fields[0], "a") == 0);
    assert(strcmp(result1.fields.fields[1], "") == 0);
    assert(strcmp(result1.fields.fields[2], "c") == 0);
    
    // Test all empty fields
    CSVParseResult result2 = csv_parse_line_inplace(",,", &arena, config, 2);
    assert(result2.success == true);
    assert(result2.fields.count == 3);
    assert(strcmp(result2.fields.fields[0], "") == 0);
    assert(strcmp(result2.fields.fields[1], "") == 0);
    assert(strcmp(result2.fields.fields[2], "") == 0);
    
    // Test quoted empty field
    CSVParseResult result3 = csv_parse_line_inplace("a,\"\",c", &arena, config, 3);
    assert(result3.success == true);
    assert(result3.fields.count == 3);
    assert(strcmp(result3.fields.fields[0], "a") == 0);
    assert(strcmp(result3.fields.fields[1], "") == 0);
    assert(strcmp(result3.fields.fields[2], "c") == 0);
    
    arena_destroy(&arena);
    printf("✓ CSV parser empty fields test passed\n");
}

void test_csv_parser_custom_delimiters() {
    printf("Testing CSV parser with custom delimiters...\n");
    Arena arena;
    assert(arena_create(&arena, 4096) == ARENA_OK);
    CSVConfig *config = csv_config_create(&arena);
    
    // Test semicolon delimiter
    csv_config_set_delimiter(config, ';');
    CSVParseResult result1 = csv_parse_line_inplace("a;b;c", &arena, config, 1);
    assert(result1.success == true);
    assert(result1.fields.count == 3);
    assert(strcmp(result1.fields.fields[0], "a") == 0);
    assert(strcmp(result1.fields.fields[1], "b") == 0);
    assert(strcmp(result1.fields.fields[2], "c") == 0);
    
    // Test pipe delimiter
    csv_config_set_delimiter(config, '|');
    CSVParseResult result2 = csv_parse_line_inplace("a|b|c", &arena, config, 2);
    assert(result2.success == true);
    assert(result2.fields.count == 3);
    assert(strcmp(result2.fields.fields[0], "a") == 0);
    assert(strcmp(result2.fields.fields[1], "b") == 0);
    assert(strcmp(result2.fields.fields[2], "c") == 0);
    
    arena_destroy(&arena);
    printf("✓ CSV parser custom delimiters test passed\n");
}

void test_read_full_record() {
    printf("Testing read_full_record function...\n");
    
    // Create a test file with multi-line content
    FILE *test_file = tmpfile();
    if (!test_file) {
        printf("Failed to create test file\n");
        return;
    }
    
    const char *test_content = "field1,\"field2\nwith newline\",field3\nsimple,line,here\n\"another\",\"multi\nline\nfield\",end\n";
    fputs(test_content, test_file);
    rewind(test_file);
    
    Arena arena;
    assert(arena_create(&arena, 4096) == ARENA_OK);
    
    // Read first record (should handle multi-line quoted field)
    char *record1 = read_full_record(test_file, &arena);
    assert(record1 != NULL);
    assert(strstr(record1, "field2\nwith newline") != NULL);
    
    // Read second record (simple line)
    char *record2 = read_full_record(test_file, &arena);
    assert(record2 != NULL);
    assert(strcmp(record2, "simple,line,here") == 0);
    
    // Read third record (multi-line)
    char *record3 = read_full_record(test_file, &arena);
    assert(record3 != NULL);
    assert(strstr(record3, "multi\nline\nfield") != NULL);
    
    // No more records
    char *record4 = read_full_record(test_file, &arena);
    assert(record4 == NULL);
    
    fclose(test_file);
    arena_destroy(&arena);
    printf("✓ read_full_record test passed\n");
}

void test_csv_parser_memory_allocation_errors() {
    printf("Testing CSV parser memory allocation error handling...\n");
    Arena arena;
    // Create a very small arena to trigger allocation failures
    assert(arena_create(&arena, 64) == ARENA_OK);
    CSVConfig *config = csv_config_create(&arena);
    
    // Try to parse a line that should trigger memory allocation failure
    const char *long_line = "very_long_field_that_might_cause_allocation_failure,another_field,and_another_field,yet_another_field";
    CSVParseResult result = csv_parse_line_inplace(long_line, &arena, config, 1);
    
    // The result might succeed or fail depending on arena size, but it shouldn't crash
    if (!result.success) {
        assert(result.error != NULL);
        printf("✓ Memory allocation error properly handled\n");
    } else {
        printf("✓ Parsing succeeded with small arena\n");
    }
    
    arena_destroy(&arena);
    printf("✓ CSV parser memory allocation error handling test passed\n");
}

int main() {
    printf("Running CSV Parser tests...\n\n");
    test_csv_parser_optimized();
    test_csv_parser_escaped_quotes();
    test_csv_parser_whitespace_trimming();
    test_csv_parser_empty_fields();
    test_csv_parser_custom_delimiters();
    test_read_full_record();
    test_csv_parser_memory_allocation_errors();
    printf("\n✅ All CSV Parser tests passed!\n");
    return 0;
} 