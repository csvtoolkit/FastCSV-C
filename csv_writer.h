#ifndef CSV_WRITER_H
#define CSV_WRITER_H

#include "csv_config.h"

typedef struct {
    char **headers;
    int header_count;
    FILE *file;
    CSVConfig *config;
    // Cached config values for performance
    char delimiter;
    char enclosure;
    char escape;
} CSVWriter;

// CSV Writer functions
CSVWriter* csv_writer_init(CSVConfig *config, char **headers, int header_count);
int csv_writer_write_record(CSVWriter *writer, char **fields, int field_count);
int csv_writer_write_record_map(CSVWriter *writer, char **field_names, char **field_values, int field_count);
void csv_writer_free(CSVWriter *writer);
void write_field(FILE *file, const char *field, char delimiter, char enclosure, char escape);

#endif // CSV_WRITER_H 