#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "csv_reader.h"
#include "csv_parser.h"
#include "arena.h"

CSVReader* csv_reader_init_with_config(Arena *persistent_arena, Arena *temp_arena, CSVConfig *config) {
    void *ptr;
    ArenaResult result = arena_alloc(persistent_arena, sizeof(CSVReader), &ptr);
    if (result != ARENA_OK) {
        return NULL;
    }

    CSVReader *reader = (CSVReader*)ptr;
    reader->file = fopen(config->path, "r");
    if (!reader->file) {
        return NULL;
    }

    reader->persistent_arena = persistent_arena;
    reader->temp_arena = temp_arena;
    reader->config = config;
    reader->headers_loaded = false;
    reader->cached_header_count = 0;
    reader->cached_headers = NULL;
    reader->line_number = 0;
    reader->current_record = NULL;
    reader->owns_arenas = false;

    if (config->hasHeader) {
        char *line = read_full_record(reader->file, reader->persistent_arena);
        if (line) {
            reader->line_number++;
            CSVParseResult result = csv_parse_line_inplace(line, reader->persistent_arena, config, reader->line_number);
            if (result.success) {
                reader->cached_headers = result.fields.fields;
                reader->cached_header_count = result.fields.count;
                reader->headers_loaded = true;
            }
        }
    }

    return reader;
}

CSVReader* csv_reader_init_standalone(CSVConfig *config) {
    if (!config) {
        return NULL;
    }
    
    Arena *persistent_arena = malloc(sizeof(Arena));
    Arena *temp_arena = malloc(sizeof(Arena));
    
    if (!persistent_arena || !temp_arena) {
        if (persistent_arena) free(persistent_arena);
        if (temp_arena) free(temp_arena);
        return NULL;
    }
    
    ArenaResult p_result = arena_create(persistent_arena, 1024 * 1024);
    ArenaResult t_result = arena_create(temp_arena, 1024 * 1024);
    
    if (p_result != ARENA_OK || t_result != ARENA_OK) {
        if (p_result == ARENA_OK) arena_destroy(persistent_arena);
        if (t_result == ARENA_OK) arena_destroy(temp_arena);
        free(persistent_arena);
        free(temp_arena);
        return NULL;
    }
    
    CSVReader *reader = malloc(sizeof(CSVReader));
    if (!reader) {
        arena_destroy(persistent_arena);
        arena_destroy(temp_arena);
        free(persistent_arena);
        free(temp_arena);
        return NULL;
    }

    reader->file = fopen(config->path, "r");
    if (!reader->file) {
        arena_destroy(persistent_arena);
        arena_destroy(temp_arena);
        free(persistent_arena);
        free(temp_arena);
        free(reader);
        return NULL;
    }

    reader->persistent_arena = persistent_arena;
    reader->temp_arena = temp_arena;
    reader->config = config;
    reader->headers_loaded = false;
    reader->cached_header_count = 0;
    reader->cached_headers = NULL;
    reader->line_number = 0;
    reader->current_record = NULL;
    reader->owns_arenas = true;

    if (config->hasHeader) {
        char *line = read_full_record(reader->file, reader->persistent_arena);
        if (line) {
            reader->line_number++;
            CSVParseResult result = csv_parse_line_inplace(line, reader->persistent_arena, config, reader->line_number);
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

    arena_reset(reader->temp_arena);

    char *line = read_full_record(reader->file, reader->temp_arena);
    if (!line) {
        return NULL;
    }

    reader->line_number++;
    CSVParseResult result = csv_parse_line_inplace(line, reader->temp_arena, reader->config, reader->line_number);
    if (!result.success) {
        return NULL;
    }

    void *ptr;
    ArenaResult arena_result = arena_alloc(reader->temp_arena, sizeof(CSVRecord), &ptr);
    if (arena_result != ARENA_OK) {
        return NULL;
    }

    CSVRecord *record = (CSVRecord*)ptr;
    record->fields = result.fields.fields;
    record->field_count = result.fields.count;
    reader->current_record = record;

    return record;
}

void csv_reader_free(CSVReader *reader) {
    if (reader) {
        if (reader->file) {
            fclose(reader->file);
            reader->file = NULL;
        }
        
        if (reader->owns_arenas) {
            if (reader->persistent_arena) {
                arena_destroy(reader->persistent_arena);
                free(reader->persistent_arena);
                reader->persistent_arena = NULL;
            }
            if (reader->temp_arena) {
                arena_destroy(reader->temp_arena);
                free(reader->temp_arena);
                reader->temp_arena = NULL;
            }
            free(reader);
        } else {
            reader->persistent_arena = NULL;
            reader->temp_arena = NULL;
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
            char *line = read_full_record(reader->file, reader->persistent_arena);
            if (line) {
                reader->line_number = 1;
            }
        }
    }
}

int csv_reader_set_config(CSVReader *reader, Arena *persistent_arena, Arena *temp_arena, const CSVConfig *config) {
    if (!reader || !config || !persistent_arena || !temp_arena) {
        return 0;
    }
    
    reader->config = (CSVConfig*)config;
    reader->persistent_arena = persistent_arena;
    reader->temp_arena = temp_arena;
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
        char *header_line = read_full_record(reader->file, reader->persistent_arena);
        if (!header_line) {
            fseek(reader->file, current_pos, SEEK_SET);
            return 0;
        }
    }
    
    while (1) {
        char *line = read_full_record(reader->file, reader->persistent_arena);
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
        arena_reset(reader->temp_arena);
        char *line = read_full_record(reader->file, reader->temp_arena);
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