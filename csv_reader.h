#ifndef CSV_READER_H
#define CSV_READER_H

#include "csv_config.h"

typedef struct {
    char **headers;
    int header_count;
    FILE *file;
    Arena arena;
    
    // New properties
    long position;          // Current position relative to records (-1 if hasHeaders and before first record)
    char **cached_headers;  // Cached headers if hasHeaders is true
    int cached_header_count;
    bool headers_loaded;    // Flag to know if headers are cached
    long record_count;      // Cached record count (-1 if not calculated yet)
    CSVConfig *config;      // Configuration object
} CSVReader;

typedef struct {
    char **fields;
    int field_count;
} CSVRecord;

// CSV Reader functions
CSVReader* csv_reader_init_with_config(const CSVConfig *config);
CSVRecord* csv_reader_next_record(CSVReader *reader);
void csv_reader_free(CSVReader *reader);

// New functions
void csv_reader_rewind(CSVReader *reader);
int csv_reader_set_config(CSVReader *reader, const CSVConfig *config);
long csv_reader_get_record_count(CSVReader *reader);
long csv_reader_get_position(CSVReader *reader);
char** csv_reader_get_headers(CSVReader *reader, int *header_count);
int csv_reader_seek(CSVReader *reader, long position);
int csv_reader_has_next(CSVReader *reader);

#endif // CSV_READER_H 