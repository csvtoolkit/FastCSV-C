#ifndef CSV_PARSER_H
#define CSV_PARSER_H

#include "csv_config.h"
#include "arena.h"
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>

#define MAX_LINE_LENGTH 4096

typedef enum {
    FIELD_START,
    UNQUOTED_FIELD,
    QUOTED_FIELD,
    QUOTE_IN_QUOTED_FIELD,
    FIELD_END
} ParseState;

typedef enum {
    CSV_PARSER_OK = 0,
    CSV_PARSER_ERROR_NULL_POINTER,
    CSV_PARSER_ERROR_MEMORY_ALLOCATION,
    CSV_PARSER_ERROR_BUFFER_OVERFLOW,
    CSV_PARSER_ERROR_INVALID_INPUT,
    CSV_PARSER_ERROR_MALFORMED_CSV
} CSVParserResult;

typedef struct {
    char **fields;
    size_t count;
    size_t capacity;
} FieldArray;

typedef struct {
    char *line;
    size_t pos;
    size_t len;
    ParseState state;
    bool in_quotes;
    char delimiter;
    char enclosure;
    char escape;
    int line_number;
    Arena *arena;
} ParseContext;

typedef struct {
    FieldArray fields;
    bool success;
    const char *error;
    int error_line;
    int error_column;
} CSVParseResult;

typedef struct {
    CSVConfig *config;
    Arena *arena;
    ParseContext parse_ctx;
} CSVParser;

char* read_full_record(FILE *file, Arena *arena);
int parse_csv_line(const char *line, char **fields, int max_fields, Arena *arena, const CSVConfig *config);
int parse_headers(const char *line, char **fields, int max_fields, Arena *arena, const CSVConfig *config);

CSVParserResult csv_parser_count_fields_in_line(const char *line, const ParseContext *ctx, int *field_count);
CSVParserResult csv_parser_split_line_generic(const char *line, FieldArray *fields, const ParseContext *ctx);

CSVParser* csv_parser_init(Arena *arena, CSVConfig *config);
void csv_parser_free(CSVParser *parser);
CSVParseResult csv_parse_line_inplace(const char *line, Arena *arena, const CSVConfig *config, int line_number);

#endif 
