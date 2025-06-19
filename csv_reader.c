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
            reader->line_number++;
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

char** csv_reader_get_headers(CSVReader *reader, int *header_count) {
    if (!reader || !header_count) {
        return NULL;
    }
    
    if (reader->headers_loaded) {
        *header_count = reader->cached_header_count;
        return reader->cached_headers;
    }
    
    *header_count = 0;
    return NULL;
}

void csv_reader_rewind(CSVReader *reader) {
    if (reader && reader->file) {
        rewind(reader->file);
        reader->line_number = 0;
        
        if (reader->config->hasHeader && reader->headers_loaded) {
            char *line = read_full_record(reader->file, reader->arena);
            if (line) {
                reader->line_number = 1;
            }
        }
    }
}

int csv_reader_set_config(CSVReader *reader, Arena *arena, const CSVConfig *config) {
    if (!reader || !config || !arena) {
        return 0;
    }
    
    reader->config = (CSVConfig*)config;
    reader->arena = arena;
    return 1;
}

long csv_reader_get_record_count(CSVReader *reader) {
    if (!reader || !reader->file) {
        return -1;
    }
    
    long current_pos = ftell(reader->file);
    if (current_pos == -1) {
        return -1;
    }
    
    rewind(reader->file);
    
    long record_count = 0;
    
    if (reader->config && reader->config->hasHeader) {
        char *header_line = read_full_record(reader->file, reader->arena);
        if (!header_line) {
            fseek(reader->file, current_pos, SEEK_SET);
            return 0;
        }
    }
    
    while (1) {
        char *line = read_full_record(reader->file, reader->arena);
        if (!line) {
            break;
        }
        
        if (reader->config && reader->config->skipEmptyLines) {
            bool is_empty = true;
            for (int i = 0; line[i] != '\0'; i++) {
                if (line[i] != ' ' && line[i] != '\t' && line[i] != '\r' && line[i] != '\n') {
                    is_empty = false;
                    break;
                }
            }
            if (is_empty) {
                continue;
            }
        }
        
        record_count++;
    }
    
    fseek(reader->file, current_pos, SEEK_SET);
    
    return record_count;
}

long csv_reader_get_position(CSVReader *reader) {
    if (!reader || !reader->file) {
        return -1;
    }
    
    return reader->line_number;
}

int csv_reader_seek(CSVReader *reader, long position) {
    if (!reader || !reader->file || position < 0) {
        return 0;
    }
    
    csv_reader_rewind(reader);
    
    for (long i = 0; i < position; i++) {
        char *line = read_full_record(reader->file, reader->arena);
        if (!line) {
            return 0;
        }
        reader->line_number++;
    }
    
    return 1;
}

int csv_reader_has_next(CSVReader *reader) {
    if (!reader || !reader->file) {
        return 0;
    }
    
    long current_pos = ftell(reader->file);
    if (current_pos == -1) {
        return 0;
    }
    
    int c = fgetc(reader->file);
    fseek(reader->file, current_pos, SEEK_SET);
    
    return c != EOF;
} 