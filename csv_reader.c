#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "csv_reader.h"
#include "csv_parser.h"
#include "arena.h"

CSVReader* csv_reader_init_with_config(Arena *arena, CSVConfig *config) {
    void *ptr;
    ArenaResult result = arena_alloc(arena, sizeof(CSVReader), &ptr);
    if (result != ARENA_OK) {
        return NULL;
    }

    CSVReader *reader = (CSVReader*)ptr;
    reader->file = fopen(config->path, "r");
    if (!reader->file) {
        return NULL;
    }

    reader->arena = arena;
    reader->config = config;
    reader->headers_loaded = false;
    reader->cached_header_count = 0;
    reader->cached_headers = NULL;
    reader->line_number = 0;

    if (config->hasHeader) {
        char *line = read_full_record(reader->file, arena);
        if (line) {
            CSVParseResult result = csv_parse_line_inplace(line, arena, config, reader->line_number);
            if (result.success) {
                reader->cached_headers = result.fields.fields;
                reader->cached_header_count = result.fields.count;
                reader->headers_loaded = true;
            }
        }
    }

    return reader;
}

CSVRecord* csv_reader_next_record(CSVReader *reader) {
    if (!reader || !reader->file) {
        return NULL;
    }

    char *line = read_full_record(reader->file, reader->arena);
    if (!line) {
        return NULL;
    }

    reader->line_number++;
    CSVParseResult result = csv_parse_line_inplace(line, reader->arena, reader->config, reader->line_number);
    if (!result.success) {
        return NULL;
    }

    void *ptr;
    ArenaResult arena_result = arena_alloc(reader->arena, sizeof(CSVRecord), &ptr);
    if (arena_result != ARENA_OK) {
        return NULL;
    }

    CSVRecord *record = (CSVRecord*)ptr;
    record->fields = result.fields.fields;
    record->field_count = result.fields.count;
    return record;
}

void csv_reader_free(CSVReader *reader) {
    if (reader) {
        if (reader->file) {
            fclose(reader->file);
        }
    }
} 