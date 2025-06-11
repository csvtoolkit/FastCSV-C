#include "csv_reader.h"

// Helper function to initialize reader properties
static void csv_reader_init_properties(CSVReader *reader) {
    reader->position = -1;
    reader->cached_headers = NULL;
    reader->cached_header_count = 0;
    reader->headers_loaded = false;
    reader->record_count = -1;
    reader->config = NULL;
}

// Helper function to load headers
static int csv_reader_load_headers(CSVReader *reader) {
    char *header_line;
    
    if (!reader || !reader->config || !reader->config->hasHeader) {
        return 0;
    }
    
    if (reader->headers_loaded) {
        return 1; // Already loaded
    }
    
    // Save current position
    long saved_pos = ftell(reader->file);
    
    // Go to beginning and skip offset
    fseek(reader->file, 0, SEEK_SET);
    for (int i = 0; i < reader->config->offset; i++) {
        header_line = read_full_record(reader->file, &reader->arena);
        if (!header_line) break;
        arena_reset(&reader->arena);
    }
    
    header_line = read_full_record(reader->file, &reader->arena);
    if (header_line) {
        reader->cached_headers = malloc(MAX_FIELDS * sizeof(char*));
        if (!reader->cached_headers) {
            fseek(reader->file, saved_pos, SEEK_SET);
            return 0;
        }
        reader->cached_header_count = parse_headers(header_line, reader->cached_headers, MAX_FIELDS, reader->config);
        arena_reset(&reader->arena);
        reader->headers_loaded = true;
        
        // Set position correctly
        if (reader->config->hasHeader) {
            reader->position = -1; // Before first record
        } else {
            reader->position = 0;
        }
    } else {
        fseek(reader->file, saved_pos, SEEK_SET);
        return 0;
    }
    
    // Restore position
    fseek(reader->file, saved_pos, SEEK_SET);
    return 1;
}

CSVReader* csv_reader_init_with_config(const CSVConfig *config) {
    CSVReader *reader;
    
    if (!config || strlen(config->path) == 0) {
        return NULL;
    }
    
    reader = malloc(sizeof(CSVReader));
    if (!reader) {
        return NULL;
    }
    
    // Initialize properties
    csv_reader_init_properties(reader);  // This already sets position to -1
    
    // Copy config
    reader->config = csv_config_copy(config);
    if (!reader->config) {
        free(reader);
        return NULL;
    }
    
    reader->file = fopen(reader->config->path, "r");
    if (!reader->file) {
        csv_config_free(reader->config);
        free(reader);
        return NULL;
    }
    
    reader->arena = arena_create(ARENA_SIZE);
    if (!reader->arena.memory) {
        fclose(reader->file);
        csv_config_free(reader->config);
        free(reader);
        return NULL;
    }
    
    // Skip offset lines
    for (int i = 0; i < reader->config->offset; i++) {
        char *line = read_full_record(reader->file, &reader->arena);
        if (!line) break;
        arena_reset(&reader->arena);
    }
    
    // Load headers if needed and position file correctly
    if (reader->config->hasHeader) {
        // Read and parse headers
        char *header_line = read_full_record(reader->file, &reader->arena);
        if (header_line) {
            reader->cached_headers = malloc(MAX_FIELDS * sizeof(char*));
            if (reader->cached_headers) {
                reader->cached_header_count = parse_headers(header_line, reader->cached_headers, MAX_FIELDS, reader->config);
                reader->headers_loaded = true;
            }
            arena_reset(&reader->arena);
        }
        reader->position = -1;  // Start at -1 when headers are present
    } else {
        reader->position = -1;  // Start at -1 for consistency
    }
    
    // Legacy headers (for backward compatibility)
    reader->headers = NULL;
    reader->header_count = 0;
    if (reader->config->hasHeader && reader->headers_loaded) {
        reader->headers = malloc(reader->cached_header_count * sizeof(char*));
        if (reader->headers) {
            for (int i = 0; i < reader->cached_header_count; i++) {
                reader->headers[i] = strdup(reader->cached_headers[i]);
            }
            reader->header_count = reader->cached_header_count;
        }
    }
    
    return reader;
}

CSVRecord* csv_reader_next_record(CSVReader *reader) {
    char *line;
    CSVRecord *record;
    
    if (!reader || !reader->file) {
        return NULL;
    }
    
    // Reset arena before processing new record
    arena_reset(&reader->arena);
    
    line = read_full_record(reader->file, &reader->arena);
    if (!line) {
        return NULL;
    }

    if (strlen(line) == 0) {
        return csv_reader_next_record(reader);
    }

    record = arena_alloc(&reader->arena, sizeof(CSVRecord));
    if (!record) {
        return NULL;
    }

    record->fields = arena_alloc(&reader->arena, MAX_FIELDS * sizeof(char*));
    if (!record->fields) {
        return NULL;
    }

    record->field_count = parse_csv_line(line, record->fields, MAX_FIELDS, &reader->arena, reader->config);
    if (record->field_count < 0) {
        return NULL;
    }
    
    // Update position
    reader->position++;

    return record;
}

void csv_reader_rewind(CSVReader *reader) {
    if (!reader || !reader->file || !reader->config) {
        return;
    }
    
    // Go to beginning
    fseek(reader->file, 0, SEEK_SET);
    
    // Skip offset lines
    arena_reset(&reader->arena);
    for (int i = 0; i < reader->config->offset; i++) {
        char *line = read_full_record(reader->file, &reader->arena);
        if (!line) break;
        arena_reset(&reader->arena);
    }
    
    // Skip header line if hasHeader is true
    if (reader->config->hasHeader) {
        char *header_line = read_full_record(reader->file, &reader->arena);
        arena_reset(&reader->arena);
    }
    
    // Always start at -1 for consistency
    reader->position = -1;
}

int csv_reader_set_config(CSVReader *reader, const CSVConfig *config) {
    if (!reader || !config) {
        return 0;
    }
    
    // Close current file
    if (reader->file) {
        fclose(reader->file);
        reader->file = NULL;
    }
    
    // Free cached headers
    if (reader->cached_headers) {
        for (int i = 0; i < reader->cached_header_count; i++) {
            free(reader->cached_headers[i]);
        }
        free(reader->cached_headers);
        reader->cached_headers = NULL;
        reader->cached_header_count = 0;
    }
    
    // Free legacy headers
    if (reader->headers) {
        for (int i = 0; i < reader->header_count; i++) {
            free(reader->headers[i]);
        }
        free(reader->headers);
        reader->headers = NULL;
        reader->header_count = 0;
    }
    
    // Free old config
    if (reader->config) {
        csv_config_free(reader->config);
        reader->config = NULL;
    }
    
    // Copy new config
    reader->config = csv_config_copy(config);
    if (!reader->config) {
        return 0;
    }
    
    // Reset properties (but keep config)
    CSVConfig *temp_config = reader->config;
    csv_reader_init_properties(reader);
    reader->config = temp_config;
    
    // Open new file
    reader->file = fopen(reader->config->path, "r");
    if (!reader->file) {
        csv_config_free(reader->config);
        reader->config = NULL;
        return 0;
    }
    
    // Reset arena
    arena_reset(&reader->arena);
    
    // Skip offset lines
    for (int i = 0; i < reader->config->offset; i++) {
        char *line = read_full_record(reader->file, &reader->arena);
        if (!line) break;
        arena_reset(&reader->arena);
    }
    
    // Load headers if needed and position file correctly
    if (reader->config->hasHeader) {
        // Read and parse headers
        char *header_line = read_full_record(reader->file, &reader->arena);
        if (header_line) {
            reader->cached_headers = malloc(MAX_FIELDS * sizeof(char*));
            if (reader->cached_headers) {
                reader->cached_header_count = parse_headers(header_line, reader->cached_headers, MAX_FIELDS, reader->config);
                reader->headers_loaded = true;
            }
            arena_reset(&reader->arena);
        }
        reader->position = -1; // Before first record (rewind sets to -1)
    } else {
        reader->position = -1; // FIXED: Consistent rewind position
    }
    
    // Update legacy headers
    if (reader->config->hasHeader && reader->headers_loaded) {
        reader->headers = malloc(reader->cached_header_count * sizeof(char*));
        if (reader->headers) {
            for (int i = 0; i < reader->cached_header_count; i++) {
                reader->headers[i] = strdup(reader->cached_headers[i]);
            }
            reader->header_count = reader->cached_header_count;
        }
    }
    
    return 1;
}

long csv_reader_get_record_count(CSVReader *reader) {
    if (!reader || !reader->file) {
        return -1;
    }
    
    // Return cached value if available
    if (reader->record_count >= 0) {
        return reader->record_count;
    }
    
    // Save current position
    long saved_pos = ftell(reader->file);
    
    // Go to beginning and count records
    fseek(reader->file, 0, SEEK_SET);
    
    // Skip offset lines
    arena_reset(&reader->arena);
    for (int i = 0; i < reader->config->offset; i++) {
        char *line = read_full_record(reader->file, &reader->arena);
        if (!line) break;
        arena_reset(&reader->arena);
    }
    
    // FIXED: Only skip header when hasHeader is true
    if (reader->config && reader->config->hasHeader) {
        char *header_line = read_full_record(reader->file, &reader->arena);
        arena_reset(&reader->arena);
    }
    
    // Count records
    long count = 0;
    char *line;
    while ((line = read_full_record(reader->file, &reader->arena)) != NULL) {
        if (strlen(line) > 0) {
            count++;
        }
        arena_reset(&reader->arena);
    }
    
    // Cache the result
    reader->record_count = count;
    
    // Restore position
    fseek(reader->file, saved_pos, SEEK_SET);
    
    return count;
}

long csv_reader_get_position(CSVReader *reader) {
    if (!reader) {
        return -1;
    }
    return reader->position;
}

char** csv_reader_get_headers(CSVReader *reader, int *header_count) {
    if (!reader || !header_count) {
        return NULL;
    }
    
    *header_count = 0;
    
    // FIXED: Return NULL when hasHeader is false (was causing bug)
    if (!reader->config || !reader->config->hasHeader) {
        return NULL;
    }
    
    if (!reader->headers_loaded) {
        csv_reader_load_headers(reader);
    }
    
    if (reader->headers_loaded && reader->cached_headers) {
        *header_count = reader->cached_header_count;
        return reader->cached_headers;
    }
    
    return NULL;
}

int csv_reader_seek(CSVReader *reader, long position) {
    if (!reader || !reader->file || position < 0) {
        return 0;
    }
    
    // Check if position is valid
    long record_count = csv_reader_get_record_count(reader);
    if (position >= record_count) {
        return 0;
    }
    
    // Go to beginning
    fseek(reader->file, 0, SEEK_SET);
    
    // Skip offset lines
    arena_reset(&reader->arena);
    for (int i = 0; i < reader->config->offset; i++) {
        char *line = read_full_record(reader->file, &reader->arena);
        if (!line) return 0;
        arena_reset(&reader->arena);
    }
    
    // Skip header if present
    if (reader->config->hasHeader) {
        char *header_line = read_full_record(reader->file, &reader->arena);
        if (!header_line) return 0;
        arena_reset(&reader->arena);
    }
    
    // Skip to desired position
    for (long i = 0; i < position; i++) {
        char *line = read_full_record(reader->file, &reader->arena);
        if (!line) return 0;
        arena_reset(&reader->arena);
    }
    
    reader->position = position;
    return 1;
}

int csv_reader_has_next(CSVReader *reader) {
    if (!reader || !reader->file) {
        return 0;
    }
    
    long current_pos = ftell(reader->file);
    char *next_line = read_full_record(reader->file, &reader->arena);
    fseek(reader->file, current_pos, SEEK_SET);
    arena_reset(&reader->arena);
    
    return next_line != NULL;
}

void csv_reader_free(CSVReader *reader) {
    int i;
    
    if (reader) {
        if (reader->file) {
            fclose(reader->file);
        }
        
        // Free legacy headers
        if (reader->headers) {
            for (i = 0; i < reader->header_count; i++) {
                free(reader->headers[i]);
            }
            free(reader->headers);
        }
        
        // Free cached headers
        if (reader->cached_headers) {
            for (i = 0; i < reader->cached_header_count; i++) {
                free(reader->cached_headers[i]);
            }
            free(reader->cached_headers);
        }
        
        // Free config
        if (reader->config) {
            csv_config_free(reader->config);
        }
        
        arena_destroy(&reader->arena);
        free(reader);
    }
} 