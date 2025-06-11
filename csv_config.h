#ifndef CSV_CONFIG_H
#define CSV_CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LINE_LENGTH 4096
#define MAX_FIELDS 32
#define ARENA_SIZE (1024 * 1024)  // 1MB arena
#define MAX_PATH_LENGTH 1024

typedef struct {
    char *memory;
    char *current;
    char *end;
} Arena;

typedef enum {
    FIELD_START,
    UNQUOTED_FIELD,
    QUOTED_FIELD,
    QUOTE_IN_QUOTED_FIELD,
    FIELD_END
} ParseState;

typedef struct {
    char delimiter;
    char enclosure;
    char escape;
    char path[MAX_PATH_LENGTH];
    int offset;
    bool hasHeader;
} CSVConfig;

// CSV Config functions
CSVConfig* csv_config_create();
void csv_config_free(CSVConfig *config);
CSVConfig* csv_config_copy(const CSVConfig *config);

// Getters
char csv_config_get_delimiter(const CSVConfig *config);
char csv_config_get_enclosure(const CSVConfig *config);
char csv_config_get_escape(const CSVConfig *config);
const char* csv_config_get_path(const CSVConfig *config);
int csv_config_get_offset(const CSVConfig *config);
bool csv_config_has_header(const CSVConfig *config);

// Setters
void csv_config_set_delimiter(CSVConfig *config, char delimiter);
void csv_config_set_enclosure(CSVConfig *config, char enclosure);
void csv_config_set_escape(CSVConfig *config, char escape);
void csv_config_set_path(CSVConfig *config, const char *path);
void csv_config_set_offset(CSVConfig *config, int offset);
void csv_config_set_has_header(CSVConfig *config, bool hasHeader);

// Arena functions
Arena arena_create(size_t size);
void arena_reset(Arena *arena);
void arena_destroy(Arena *arena);
void* arena_alloc(Arena *arena, size_t size);

// Utility functions
char* trim_whitespace(char *str);
char* arena_strdup(Arena *arena, const char *str);
int parse_csv_line(const char *line, char **fields, int max_fields, Arena *arena, const CSVConfig *config);
int parse_headers(const char *line, char **fields, int max_fields, const CSVConfig *config);
char* read_full_record(FILE *file, Arena *arena);

#endif // CSV_CONFIG_H 