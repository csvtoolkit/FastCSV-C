#ifndef CSV_WRITER_H
#define CSV_WRITER_H

#include "csv_config.h"
#include "arena.h"
#include <stdio.h>
#include <stdbool.h>

typedef enum {
    CSV_WRITER_OK = 0,
    CSV_WRITER_ERROR_NULL_POINTER,
    CSV_WRITER_ERROR_MEMORY_ALLOCATION,
    CSV_WRITER_ERROR_FILE_OPEN,
    CSV_WRITER_ERROR_FILE_WRITE,
    CSV_WRITER_ERROR_INVALID_FIELD_COUNT,
    CSV_WRITER_ERROR_FIELD_NOT_FOUND,
    CSV_WRITER_ERROR_BUFFER_OVERFLOW
} CSVWriterResult;

typedef struct {
    char **headers;
    int header_count;
    FILE *file;
    CSVConfig *config;
    Arena *arena;
    char delimiter;
    char enclosure;
    char escape;
    bool owns_file;
    bool owns_config;
} CSVWriter;

typedef struct {
    const char *field;
    char delimiter;
    char enclosure;
    char escape;
    bool needs_quoting;
} FieldWriteOptions;

CSVWriterResult csv_writer_init(CSVWriter **writer, CSVConfig *config, char **headers, int header_count, Arena *arena);
CSVWriterResult csv_writer_init_with_file(CSVWriter **writer, FILE *file, CSVConfig *config, char **headers, int header_count, Arena *arena);
CSVWriterResult csv_writer_write_record(CSVWriter *writer, char **fields, int field_count);
CSVWriterResult csv_writer_write_record_map(CSVWriter *writer, char **field_names, char **field_values, int field_count);
CSVWriterResult csv_writer_flush(CSVWriter *writer);
void csv_writer_free(CSVWriter *writer);

CSVWriterResult write_field(FILE *file, const FieldWriteOptions *options);
CSVWriterResult write_headers(CSVWriter *writer, char **headers, int header_count);
bool field_needs_quoting(const char *field, char delimiter, char enclosure);

const char* csv_writer_error_string(CSVWriterResult result);

#endif 
