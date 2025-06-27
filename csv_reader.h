#ifndef CSV_READER_H
#define CSV_READER_H

#include <stdio.h>
#include "csv_config.h"
#include "arena.h"

typedef struct {
    char **fields;
    size_t field_count;
} CSVRecord;

typedef struct {
    FILE *file;
    CSVConfig *config;
    Arena *persistent_arena;
    Arena *temp_arena;
    bool headers_loaded;
    int cached_header_count;
    char **cached_headers;
    long line_number;
    CSVRecord *current_record;
    bool owns_arenas;
} CSVReader;

CSVReader* csv_reader_init_with_config(Arena *persistent_arena, Arena *temp_arena, CSVConfig *config);
CSVReader* csv_reader_init_standalone(CSVConfig *config);
void csv_reader_free(CSVReader *reader);
CSVRecord* csv_reader_next_record(CSVReader *reader);


void csv_reader_rewind(CSVReader *reader);
int csv_reader_set_config(CSVReader *reader, Arena *persistent_arena, Arena *temp_arena, const CSVConfig *config);
long csv_reader_get_record_count(CSVReader *reader);
long csv_reader_get_position(CSVReader *reader);
char** csv_reader_get_headers(CSVReader *reader, int *header_count);
int csv_reader_seek(CSVReader *reader, long position);
int csv_reader_has_next(CSVReader *reader);

#endif 
