#ifndef CSV_CONFIG_H
#define CSV_CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "arena.h"

#define MAX_LINE_LENGTH 4096
#define MAX_FIELDS 32
#define MAX_PATH_LENGTH 1024
#define MAX_ENCODING_LENGTH 32

typedef enum {
    CSV_ENCODING_UTF8,
    CSV_ENCODING_UTF16LE,
    CSV_ENCODING_UTF16BE,
    CSV_ENCODING_UTF32LE,
    CSV_ENCODING_UTF32BE,
    CSV_ENCODING_ASCII,
    CSV_ENCODING_LATIN1
} CSVEncoding;

typedef struct {
    char delimiter;
    char enclosure;
    char escape;
    char path[MAX_PATH_LENGTH];
    int offset;
    bool hasHeader;
    char limit;
    CSVEncoding encoding;
    bool writeBOM;
    bool strictMode;
    bool skipEmptyLines;
    bool trimFields;
    bool preserveQuotes;
    bool autoFlush;
} CSVConfig;

CSVConfig* csv_config_create(Arena *arena);
void csv_config_free(CSVConfig *config);
CSVConfig* csv_config_copy(Arena *arena, const CSVConfig *config);

char csv_config_get_delimiter(const CSVConfig *config);
char csv_config_get_enclosure(const CSVConfig *config);
char csv_config_get_escape(const CSVConfig *config);
const char* csv_config_get_path(const CSVConfig *config);
int csv_config_get_offset(const CSVConfig *config);
int csv_config_get_limit(const CSVConfig *config);
bool csv_config_has_header(const CSVConfig *config);
CSVEncoding csv_config_get_encoding(const CSVConfig *config);
bool csv_config_get_write_bom(const CSVConfig *config);
bool csv_config_get_strict_mode(const CSVConfig *config);
bool csv_config_get_skip_empty_lines(const CSVConfig *config);
bool csv_config_get_trim_fields(const CSVConfig *config);
bool csv_config_get_preserve_quotes(const CSVConfig *config);
bool csv_config_get_auto_flush(const CSVConfig *config);

void csv_config_set_delimiter(CSVConfig *config, char delimiter);
void csv_config_set_enclosure(CSVConfig *config, char enclosure);
void csv_config_set_escape(CSVConfig *config, char escape);
void csv_config_set_path(CSVConfig *config, const char *path);
void csv_config_set_offset(CSVConfig *config, int offset);
void csv_config_set_limit(CSVConfig *config, int limit);
void csv_config_set_has_header(CSVConfig *config, bool hasHeader);
void csv_config_set_encoding(CSVConfig *config, CSVEncoding encoding);
void csv_config_set_write_bom(CSVConfig *config, bool writeBOM);
void csv_config_set_strict_mode(CSVConfig *config, bool strictMode);
void csv_config_set_skip_empty_lines(CSVConfig *config, bool skipEmptyLines);
void csv_config_set_trim_fields(CSVConfig *config, bool trimFields);
void csv_config_set_preserve_quotes(CSVConfig *config, bool preserveQuotes);
void csv_config_set_auto_flush(CSVConfig *config, bool autoFlush);

#endif 
