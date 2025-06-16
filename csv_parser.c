#include "csv_parser.h"
#include "arena.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

static void add_field(FieldArray *arr, const char *start, size_t len, Arena *arena) {
    if (arr->count >= arr->capacity) {
        if (!grow_field_array(arr, arena)) {
            return;
        }
    }

    void *ptr;
    ArenaResult result = arena_alloc(arena, len + 1, &ptr);
    if (result != ARENA_OK) {
        return;
    }
    char *field = (char*)ptr;
    memcpy(field, start, len);
    field[len] = '\0';
    arr->fields[arr->count++] = field;
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
                    add_field(&result.fields, "", 0, arena);
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
                    add_field(&result.fields, field_start, field_len, arena);
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
                        field_len++;
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
                    add_field(&result.fields, field_start, field_len, arena);
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
        add_field(&result.fields, field_start, field_len, arena);
    }

    return result;
}

char* read_full_record(FILE *file, Arena *arena) {
    if (!file || !arena) {
        return NULL;
    }

    char buffer[4096];
    if (!fgets(buffer, sizeof(buffer), file)) {
        return NULL;
    }

    size_t len = strlen(buffer);
    if (len > 0 && (buffer[len-1] == '\n' || buffer[len-1] == '\r')) {
        buffer[len-1] = '\0';
        len--;
    }
    if (len > 0 && buffer[len-1] == '\r') {
        buffer[len-1] = '\0';
        len--;
    }

    void *ptr;
    ArenaResult result = arena_alloc(arena, len + 1, &ptr);
    if (result != ARENA_OK) {
        return NULL;
    }

    char *line = (char*)ptr;
    strcpy(line, buffer);
    return line;
} 