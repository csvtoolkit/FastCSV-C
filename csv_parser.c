#include "csv_parser.h"
#include "arena.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

static void init_field_array(FieldArray *arr, Arena *arena, size_t initial_capacity) {
    void *ptr;
    ArenaResult result = arena_alloc(arena, sizeof(char*) * initial_capacity, &ptr);
    if (result != ARENA_OK) {
        arr->fields = NULL;
        arr->count = 0;
        arr->capacity = 0;
        return;
    }
    arr->fields = (char**)ptr;
    arr->count = 0;
    arr->capacity = initial_capacity;
}

static bool grow_field_array(FieldArray *arr, Arena *arena) {
    size_t new_capacity = arr->capacity * 2;
    void *ptr;
    ArenaResult result = arena_alloc(arena, sizeof(char*) * new_capacity, &ptr);
    if (result != ARENA_OK) {
        return false;
    }
    char **new_fields = (char**)ptr;
    memcpy(new_fields, arr->fields, sizeof(char*) * arr->count);
    arr->fields = new_fields;
    arr->capacity = new_capacity;
    return true;
}

static bool add_field(FieldArray *arr, const char *start, size_t len, Arena *arena) {
    if (arr->count >= arr->capacity) {
        if (!grow_field_array(arr, arena)) {
            return false;
        }
    }

    while (len > 0 && (start[len-1] == ' ' || start[len-1] == '\t')) {
        len--;
    }

    void *ptr;
    ArenaResult result = arena_alloc(arena, len + 1, &ptr);
    if (result != ARENA_OK) {
        return false;
    }
    char *field = (char*)ptr;
    memcpy(field, start, len);
    field[len] = '\0';
    arr->fields[arr->count++] = field;
    return true;
}

static bool add_quoted_field(FieldArray *arr, const char *start, size_t len, Arena *arena, char enclosure) {
    if (arr->count >= arr->capacity) {
        if (!grow_field_array(arr, arena)) {
            return false;
        }
    }

    void *ptr;
    ArenaResult result = arena_alloc(arena, len + 1, &ptr);
    if (result != ARENA_OK) {
        return false;
    }
    
    char *field = (char*)ptr;
    size_t write_pos = 0;
    
    for (size_t i = 0; i < len; i++) {
        if (start[i] == enclosure && i + 1 < len && start[i + 1] == enclosure) {
            field[write_pos++] = enclosure;
            i++;
        } else {
            field[write_pos++] = start[i];
        }
    }
    
    field[write_pos] = '\0';
    arr->fields[arr->count++] = field;
    return true;
}

CSVParseResult csv_parse_line_inplace(const char *line, Arena *arena, const CSVConfig *config, int line_number) {
    CSVParseResult result = {0};
    result.success = true;
    result.error = NULL;
    result.error_line = line_number;
    result.error_column = 0;

    if (!line || !arena || !config) {
        result.success = false;
        result.error = "Invalid arguments";
        return result;
    }

    init_field_array(&result.fields, arena, 16);
    if (!result.fields.fields) {
        result.success = false;
        result.error = "Failed to allocate field array";
        return result;
    }

    size_t len = strlen(line);
    ParseState state = FIELD_START;
    const char *field_start = line;
    size_t field_len = 0;
    size_t pos = 0;

    while (pos < len) {
        char c = line[pos];
        
        switch (state) {
            case FIELD_START:
                if (c == config->enclosure) {
                    state = QUOTED_FIELD;
                    field_start = &line[pos + 1];
                    field_len = 0;
                } else if (c == config->delimiter) {
                    if (!add_field(&result.fields, "", 0, arena)) {
                        result.success = false;
                        result.error = "Memory allocation failed";
                        result.error_column = pos;
                        return result;
                    }
                    field_start = &line[pos + 1];
                    field_len = 0;
                } else {
                    state = UNQUOTED_FIELD;
                    field_start = &line[pos];
                    field_len = 1;
                }
                break;

            case UNQUOTED_FIELD:
                if (c == config->delimiter) {
                    if (!add_field(&result.fields, field_start, field_len, arena)) {
                        result.success = false;
                        result.error = "Memory allocation failed";
                        result.error_column = pos;
                        return result;
                    }
                    state = FIELD_START;
                    field_start = &line[pos + 1];
                    field_len = 0;
                } else {
                    field_len++;
                }
                break;

            case QUOTED_FIELD:
                if (c == config->enclosure) {
                    if (pos + 1 < len && line[pos + 1] == config->enclosure) {
                        field_len += 2;
                        pos++;
                    } else {
                        state = FIELD_END;
                    }
                } else {
                    field_len++;
                }
                break;

            case FIELD_END:
                if (c == config->delimiter) {
                    if (!add_quoted_field(&result.fields, field_start, field_len, arena, config->enclosure)) {
                        result.success = false;
                        result.error = "Memory allocation failed";
                        result.error_column = pos;
                        return result;
                    }
                    state = FIELD_START;
                    field_start = &line[pos + 1];
                    field_len = 0;
                } else if (c != ' ' && c != '\t' && c != '\r' && c != '\n') {
                    result.success = false;
                    result.error = "Expected delimiter after quoted field";
                    result.error_column = pos;
                    return result;
                }
                break;

            default:
                result.success = false;
                result.error = "Invalid parser state";
                result.error_column = pos;
                return result;
        }
        pos++;
    }

    if (state == QUOTED_FIELD) {
        result.success = false;
        result.error = "Unclosed quote";
        result.error_column = pos;
        return result;
    }

    if (field_len > 0 || state == FIELD_START) {
        if (state == FIELD_END) {
            if (!add_quoted_field(&result.fields, field_start, field_len, arena, config->enclosure)) {
                result.success = false;
                result.error = "Memory allocation failed";
                return result;
            }
        } else {
            if (!add_field(&result.fields, field_start, field_len, arena)) {
                result.success = false;
                result.error = "Memory allocation failed";
                return result;
            }
        }
    }

    return result;
}

char* read_full_record(FILE *file, Arena *arena) {
    if (!file || !arena) {
        return NULL;
    }

    size_t record_capacity = 1024;
    void *arena_ptr;
    ArenaResult result = arena_alloc(arena, record_capacity, &arena_ptr);
    if (result != ARENA_OK) {
        return NULL;
    }
    
    char *record = (char*)arena_ptr;
    size_t record_len = 0;
    bool in_quotes = false;
    int c;

    while ((c = fgetc(file)) != EOF) {
        if (record_len >= record_capacity - 1) {
            size_t new_capacity = record_capacity * 2;
            void *new_ptr;
            ArenaResult grow_result = arena_alloc(arena, new_capacity, &new_ptr);
            if (grow_result != ARENA_OK) {
                return NULL;
            }
            char *new_record = (char*)new_ptr;
            memcpy(new_record, record, record_len);
            record = new_record;
            record_capacity = new_capacity;
        }

        if (c == '"') {
            if (in_quotes) {
                int next_c = fgetc(file);
                if (next_c == '"') {
                    record[record_len++] = '"';
                    record[record_len++] = '"';
                } else {
                    record[record_len++] = '"';
                    in_quotes = false;
                    if (next_c != EOF) {
                        ungetc(next_c, file);
                    }
                }
            } else {
                in_quotes = true;
                record[record_len++] = '"';
            }
        } else if (c == '\n' || c == '\r') {
            if (!in_quotes) {
                if (c == '\r') {
                    int next_c = fgetc(file);
                    if (next_c != '\n' && next_c != EOF) {
                        ungetc(next_c, file);
                    }
                }
                break;
            } else {
                record[record_len++] = c;
            }
        } else {
            record[record_len++] = c;
        }
    }

    if (record_len == 0 && c == EOF) {
        return NULL;
    }

    record[record_len] = '\0';
    
    return record;
} 