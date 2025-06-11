#include "csv_config.h"

// CSV Config functions
CSVConfig* csv_config_create() {
    CSVConfig *config = malloc(sizeof(CSVConfig));
    if (!config) {
        return NULL;
    }
    
    // Set default values
    config->delimiter = ',';
    config->enclosure = '"';
    config->escape = '\\';
    config->path[0] = '\0';
    config->offset = 0;
    config->hasHeader = true;
    
    return config;
}

void csv_config_free(CSVConfig *config) {
    if (config) {
        free(config);
    }
}

CSVConfig* csv_config_copy(const CSVConfig *config) {
    if (!config) {
        return NULL;
    }
    
    CSVConfig *copy = malloc(sizeof(CSVConfig));
    if (!copy) {
        return NULL;
    }
    
    memcpy(copy, config, sizeof(CSVConfig));
    return copy;
}

// Getters
char csv_config_get_delimiter(const CSVConfig *config) {
    return config ? config->delimiter : ',';
}

char csv_config_get_enclosure(const CSVConfig *config) {
    return config ? config->enclosure : '"';
}

char csv_config_get_escape(const CSVConfig *config) {
    return config ? config->escape : '\\';
}

const char* csv_config_get_path(const CSVConfig *config) {
    return config ? config->path : "";
}

int csv_config_get_offset(const CSVConfig *config) {
    return config ? config->offset : 0;
}

bool csv_config_has_header(const CSVConfig *config) {
    return config ? config->hasHeader : true;
}

// Setters
void csv_config_set_delimiter(CSVConfig *config, char delimiter) {
    if (config) {
        config->delimiter = delimiter;
    }
}

void csv_config_set_enclosure(CSVConfig *config, char enclosure) {
    if (config) {
        config->enclosure = enclosure;
    }
}

void csv_config_set_escape(CSVConfig *config, char escape) {
    if (config) {
        config->escape = escape;
    }
}

void csv_config_set_path(CSVConfig *config, const char *path) {
    if (config && path) {
        strncpy(config->path, path, MAX_PATH_LENGTH - 1);
        config->path[MAX_PATH_LENGTH - 1] = '\0';
    }
}

void csv_config_set_offset(CSVConfig *config, int offset) {
    if (config) {
        config->offset = offset;
    }
}

void csv_config_set_has_header(CSVConfig *config, bool hasHeader) {
    if (config) {
        config->hasHeader = hasHeader;
    }
}

// Arena functions
Arena arena_create(size_t size) {
    Arena arena;
    arena.memory = malloc(size);
    arena.current = arena.memory;
    arena.end = arena.memory + size;
    return arena;
}

void arena_reset(Arena *arena) {
    arena->current = arena->memory;
}

void arena_destroy(Arena *arena) {
    free(arena->memory);
}

void* arena_alloc(Arena *arena, size_t size) {
    if (arena->current + size > arena->end) {
        return NULL;
    }
    void *ptr = arena->current;
    arena->current += size;
    return ptr;
}

char* trim_whitespace(char *str) {
    char *end;
    
    while (*str == ' ' || *str == '\t' || *str == '\r') {
        str++;
    }
    
    if (*str == '\0') {
        return str;
    }
    
    end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n')) {
        end--;
    }
    end[1] = '\0';
    
    return str;
}

char* arena_strdup(Arena *arena, const char *str) {
    size_t len = strlen(str);
    char *copy = arena_alloc(arena, len + 1);
    if (copy) {
        memcpy(copy, str, len);
        copy[len] = '\0';
    }
    return copy;
}

char* read_full_record(FILE *file, Arena *arena) {
    char *buffer;
    size_t pos;
    int in_quotes;
    int ch;
    
    buffer = arena_alloc(arena, MAX_LINE_LENGTH);
    if (!buffer) {
        return NULL;
    }
    
    pos = 0;
    in_quotes = 0;
    
    while ((ch = fgetc(file)) != EOF && pos < MAX_LINE_LENGTH - 1) {
        buffer[pos++] = ch;
        
        if (ch == '"') {
            in_quotes = !in_quotes;
        } else if (ch == '\n' && !in_quotes) {
            break;
        }
    }
    
    if (pos == 0 && ch == EOF) {
        return NULL;
    }
    
    if (pos > 0 && buffer[pos-1] == '\n') {
        pos--;
    }
    if (pos > 0 && buffer[pos-1] == '\r') {
        pos--;
    }
    buffer[pos] = '\0';
    
    return buffer;
}

int parse_csv_line(const char *line, char **fields, int max_fields, Arena *arena, const CSVConfig *config) {
    int field_count;
    ParseState state;
    char *field_buffer;
    size_t field_pos;
    const char *current;
    char ch;
    char delimiter, enclosure;
    
    if (!line || !fields || max_fields <= 0) {
        return -1;
    }
    
    delimiter = config ? config->delimiter : ',';
    enclosure = config ? config->enclosure : '"';
    
    field_buffer = arena_alloc(arena, strlen(line) + 1);
    if (!field_buffer) {
        return -1;
    }
    
    field_count = 0;
    state = FIELD_START;
    field_pos = 0;
    current = line;
    
    while (*current && field_count < max_fields) {
        ch = *current;
        
        switch (state) {
            case FIELD_START:
                field_pos = 0;
                if (ch == enclosure) {
                    state = QUOTED_FIELD;
                } else if (ch == delimiter) {
                    field_buffer[0] = '\0';
                    fields[field_count] = arena_strdup(arena, field_buffer);
                    if (!fields[field_count]) {
                        return -1;
                    }
                    field_count++;
                    state = FIELD_START;
                } else {
                    if (field_pos < strlen(line)) {
                        field_buffer[field_pos++] = ch;
                    }
                    state = UNQUOTED_FIELD;
                }
                break;
                
            case UNQUOTED_FIELD:
                if (ch == delimiter) {
                    field_buffer[field_pos] = '\0';
                    fields[field_count] = arena_strdup(arena, field_buffer);
                    if (!fields[field_count]) {
                        return -1;
                    }
                    field_count++;
                    state = FIELD_START;
                } else {
                    if (field_pos < strlen(line)) {
                        field_buffer[field_pos++] = ch;
                    }
                }
                break;
                
            case QUOTED_FIELD:
                if (ch == enclosure) {
                    state = QUOTE_IN_QUOTED_FIELD;
                } else {
                    if (field_pos < strlen(line)) {
                        field_buffer[field_pos++] = ch;
                    }
                }
                break;
                
            case QUOTE_IN_QUOTED_FIELD:
                if (ch == enclosure) {
                    if (field_pos < strlen(line)) {
                        field_buffer[field_pos++] = enclosure;
                    }
                    state = QUOTED_FIELD;
                } else if (ch == delimiter) {
                    field_buffer[field_pos] = '\0';
                    fields[field_count] = arena_strdup(arena, field_buffer);
                    if (!fields[field_count]) {
                        return -1;
                    }
                    field_count++;
                    state = FIELD_START;
                } else {
                    state = FIELD_END;
                }
                break;
                
            case FIELD_END:
                if (ch == delimiter) {
                    field_buffer[field_pos] = '\0';
                    fields[field_count] = arena_strdup(arena, field_buffer);
                    if (!fields[field_count]) {
                        return -1;
                    }
                    field_count++;
                    state = FIELD_START;
                }
                break;
        }
        current++;
    }
    
    if (field_count < max_fields) {
        field_buffer[field_pos] = '\0';
        fields[field_count] = arena_strdup(arena, field_buffer);
        if (!fields[field_count]) {
            return -1;
        }
        field_count++;
    }
    
    return field_count;
}

int parse_headers(const char *line, char **fields, int max_fields, const CSVConfig *config) {
    int field_count;
    ParseState state;
    char field_buffer[MAX_LINE_LENGTH];
    size_t field_pos;
    const char *current;
    char ch;
    char delimiter, enclosure;
    
    if (!line || !fields || max_fields <= 0) {
        return -1;
    }
    
    delimiter = config ? config->delimiter : ',';
    enclosure = config ? config->enclosure : '"';
    
    field_count = 0;
    state = FIELD_START;
    field_pos = 0;
    current = line;
    
    while (*current && field_count < max_fields) {
        ch = *current;
        
        switch (state) {
            case FIELD_START:
                field_pos = 0;
                if (ch == enclosure) {
                    state = QUOTED_FIELD;
                } else if (ch == delimiter) {
                    field_buffer[0] = '\0';
                    fields[field_count] = malloc(1);
                    if (!fields[field_count]) {
                        return -1;
                    }
                    fields[field_count][0] = '\0';
                    field_count++;
                    state = FIELD_START;
                } else {
                    if (field_pos < MAX_LINE_LENGTH - 1) {
                        field_buffer[field_pos++] = ch;
                    }
                    state = UNQUOTED_FIELD;
                }
                break;
                
            case UNQUOTED_FIELD:
                if (ch == delimiter) {
                    field_buffer[field_pos] = '\0';
                    fields[field_count] = malloc(strlen(field_buffer) + 1);
                    if (!fields[field_count]) {
                        return -1;
                    }
                    strcpy(fields[field_count], field_buffer);
                    field_count++;
                    state = FIELD_START;
                } else {
                    if (field_pos < MAX_LINE_LENGTH - 1) {
                        field_buffer[field_pos++] = ch;
                    }
                }
                break;
                
            case QUOTED_FIELD:
                if (ch == enclosure) {
                    state = QUOTE_IN_QUOTED_FIELD;
                } else {
                    if (field_pos < MAX_LINE_LENGTH - 1) {
                        field_buffer[field_pos++] = ch;
                    }
                }
                break;
                
            case QUOTE_IN_QUOTED_FIELD:
                if (ch == enclosure) {
                    if (field_pos < MAX_LINE_LENGTH - 1) {
                        field_buffer[field_pos++] = enclosure;
                    }
                    state = QUOTED_FIELD;
                } else if (ch == delimiter) {
                    field_buffer[field_pos] = '\0';
                    fields[field_count] = malloc(strlen(field_buffer) + 1);
                    if (!fields[field_count]) {
                        return -1;
                    }
                    strcpy(fields[field_count], field_buffer);
                    field_count++;
                    state = FIELD_START;
                } else {
                    state = FIELD_END;
                }
                break;
                
            case FIELD_END:
                if (ch == delimiter) {
                    field_buffer[field_pos] = '\0';
                    fields[field_count] = malloc(strlen(field_buffer) + 1);
                    if (!fields[field_count]) {
                        return -1;
                    }
                    strcpy(fields[field_count], field_buffer);
                    field_count++;
                    state = FIELD_START;
                }
                break;
        }
        current++;
    }
    
    if (field_count < max_fields) {
        field_buffer[field_pos] = '\0';
        fields[field_count] = malloc(strlen(field_buffer) + 1);
        if (!fields[field_count]) {
            return -1;
        }
        strcpy(fields[field_count], field_buffer);
        field_count++;
    }
    
    return field_count;
} 