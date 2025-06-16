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



typedef struct {
    char delimiter;
    char enclosure;
    char escape;
    char path[MAX_PATH_LENGTH];
    int offset;
    bool hasHeader;
    char limit;
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


void csv_config_set_delimiter(CSVConfig *config, char delimiter);
void csv_config_set_enclosure(CSVConfig *config, char enclosure);
void csv_config_set_escape(CSVConfig *config, char escape);
void csv_config_set_path(CSVConfig *config, const char *path);
void csv_config_set_offset(CSVConfig *config, int offset);
void csv_config_set_limit(CSVConfig *config, int limit);
void csv_config_set_has_header(CSVConfig *config, bool hasHeader);

#endif 
