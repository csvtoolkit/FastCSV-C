#include "csv_writer.h"
#include "csv_utils.h"
#include <string.h>

const char* csv_writer_error_string(CSVWriterResult result) {
    switch (result) {
        case CSV_WRITER_OK: return "Success";
        case CSV_WRITER_ERROR_NULL_POINTER: return "Null pointer error";
        case CSV_WRITER_ERROR_MEMORY_ALLOCATION: return "Memory allocation failed";
        case CSV_WRITER_ERROR_FILE_OPEN: return "Failed to open file";
        case CSV_WRITER_ERROR_FILE_WRITE: return "Failed to write to file";
        case CSV_WRITER_ERROR_INVALID_FIELD_COUNT: return "Invalid field count";
        case CSV_WRITER_ERROR_FIELD_NOT_FOUND: return "Field not found";
        case CSV_WRITER_ERROR_BUFFER_OVERFLOW: return "Buffer overflow";
        default: return "Unknown error";
    }
}

static CSVWriterResult validate_writer_params(CSVWriter **writer, CSVConfig *config, Arena *arena) {
    if (!writer) return CSV_WRITER_ERROR_NULL_POINTER;
    if (!config) return CSV_WRITER_ERROR_NULL_POINTER;
    if (!arena) return CSV_WRITER_ERROR_NULL_POINTER;
    
    const char *path = csv_config_get_path(config);
    if (!path || path[0] == '\0') return CSV_WRITER_ERROR_NULL_POINTER;
    
    return CSV_WRITER_OK;
}

static CSVWriterResult allocate_writer(CSVWriter **writer, Arena *arena) {
    void *ptr;
    ArenaResult result = arena_alloc(arena, sizeof(CSVWriter), &ptr);
    if (result != ARENA_OK) return CSV_WRITER_ERROR_MEMORY_ALLOCATION;
    
    *writer = (CSVWriter*)ptr;
    memset(*writer, 0, sizeof(CSVWriter));
    (*writer)->arena = arena;
    return CSV_WRITER_OK;
}

static CSVWriterResult copy_headers_to_arena(CSVWriter *writer, char **headers, int header_count) {
    if (header_count <= 0) {
        writer->headers = NULL;
        writer->header_count = 0;
        return CSV_WRITER_OK;
    }
    
    void *ptr;
    ArenaResult result = arena_alloc(writer->arena, header_count * sizeof(char*), &ptr);
    if (result != ARENA_OK) return CSV_WRITER_ERROR_MEMORY_ALLOCATION;
    
    writer->headers = (char**)ptr;
    
    for (int i = 0; i < header_count; i++) {
        if (!headers[i]) {
            writer->headers[i] = arena_strdup(writer->arena, "");
        } else {
            writer->headers[i] = arena_strdup(writer->arena, headers[i]);
        }
        if (!writer->headers[i]) return CSV_WRITER_ERROR_MEMORY_ALLOCATION;
    }
    
    writer->header_count = header_count;
    return CSV_WRITER_OK;
}

CSVWriterResult csv_writer_init(CSVWriter **writer, CSVConfig *config, char **headers, int header_count, Arena *arena) {
    CSVWriterResult result = validate_writer_params(writer, config, arena);
    if (result != CSV_WRITER_OK) return result;
    
    result = allocate_writer(writer, arena);
    if (result != CSV_WRITER_OK) return result;
    
    const char *path = csv_config_get_path(config);
    (*writer)->file = fopen(path, "w");
    if (!(*writer)->file) return CSV_WRITER_ERROR_FILE_OPEN;
    
    (*writer)->owns_file = true;
    (*writer)->config = csv_config_copy(arena, config);
    if (!(*writer)->config) {
        fclose((*writer)->file);
        return CSV_WRITER_ERROR_MEMORY_ALLOCATION;
    }
    (*writer)->owns_config = true;
    
    (*writer)->delimiter = csv_config_get_delimiter((*writer)->config);
    (*writer)->enclosure = csv_config_get_enclosure((*writer)->config);
    (*writer)->escape = csv_config_get_escape((*writer)->config);
    
    result = copy_headers_to_arena(*writer, headers, header_count);
    if (result != CSV_WRITER_OK) {
        if ((*writer)->owns_config) csv_config_free((*writer)->config);
        fclose((*writer)->file);
        return result;
    }
    
    if (header_count > 0) {
        result = write_headers(*writer, headers, header_count);
        if (result != CSV_WRITER_OK) {
            if ((*writer)->owns_config) csv_config_free((*writer)->config);
            fclose((*writer)->file);
            return result;
        }
    }
    
    return CSV_WRITER_OK;
}

CSVWriterResult csv_writer_init_with_file(CSVWriter **writer, FILE *file, CSVConfig *config, char **headers, int header_count, Arena *arena) {
    if (!writer || !file || !config || !arena) return CSV_WRITER_ERROR_NULL_POINTER;
    
    CSVWriterResult result = allocate_writer(writer, arena);
    if (result != CSV_WRITER_OK) return result;
    
    (*writer)->file = file;
    (*writer)->owns_file = false;
    (*writer)->config = config;
    (*writer)->owns_config = false;
    
    (*writer)->delimiter = csv_config_get_delimiter((*writer)->config);
    (*writer)->enclosure = csv_config_get_enclosure((*writer)->config);
    (*writer)->escape = csv_config_get_escape((*writer)->config);
    
    result = copy_headers_to_arena(*writer, headers, header_count);
    if (result != CSV_WRITER_OK) return result;
    
    if (header_count > 0) {
        result = write_headers(*writer, headers, header_count);
        if (result != CSV_WRITER_OK) return result;
    }
    
    return CSV_WRITER_OK;
}

bool field_needs_quoting(const char *field, char delimiter, char enclosure) {
    if (!field) return false;
    
    return strchr(field, delimiter) != NULL ||
           strchr(field, enclosure) != NULL ||
           strchr(field, '\r') != NULL ||
           strchr(field, '\n') != NULL;
}

CSVWriterResult write_field(FILE *file, const FieldWriteOptions *options) {
    if (!file || !options) return CSV_WRITER_ERROR_NULL_POINTER;
    
    const char *field = options->field ? options->field : "";
    
    if (options->needs_quoting || field_needs_quoting(field, options->delimiter, options->enclosure)) {
        if (fputc(options->enclosure, file) == EOF) return CSV_WRITER_ERROR_FILE_WRITE;
        
        for (const char *p = field; *p; p++) {
            if (*p == options->enclosure) {
                if (fputc(options->enclosure, file) == EOF) return CSV_WRITER_ERROR_FILE_WRITE;
                if (fputc(options->enclosure, file) == EOF) return CSV_WRITER_ERROR_FILE_WRITE;
            } else {
                if (fputc(*p, file) == EOF) return CSV_WRITER_ERROR_FILE_WRITE;
            }
        }
        
        if (fputc(options->enclosure, file) == EOF) return CSV_WRITER_ERROR_FILE_WRITE;
    } else {
        if (fputs(field, file) == EOF) return CSV_WRITER_ERROR_FILE_WRITE;
    }
    
    return CSV_WRITER_OK;
}

CSVWriterResult write_headers(CSVWriter *writer, char **headers, int header_count) {
    if (!writer || !writer->file) return CSV_WRITER_ERROR_NULL_POINTER;
    if (header_count <= 0) return CSV_WRITER_OK;
    
    for (int i = 0; i < header_count; i++) {
        if (i > 0) {
            if (fputc(writer->delimiter, writer->file) == EOF) {
                return CSV_WRITER_ERROR_FILE_WRITE;
            }
        }
        
        FieldWriteOptions options = {
            .field = headers[i],
            .delimiter = writer->delimiter,
            .enclosure = writer->enclosure,
            .escape = writer->escape,
            .needs_quoting = false
        };
        
                CSVWriterResult result = write_field(writer->file, &options);
        if (result != CSV_WRITER_OK) return result;
    }

    if (fprintf(writer->file, "\r\n") < 0) return CSV_WRITER_ERROR_FILE_WRITE;
    return CSV_WRITER_OK;
}

CSVWriterResult csv_writer_write_record(CSVWriter *writer, char **fields, int field_count) {
    if (!writer || !writer->file) return CSV_WRITER_ERROR_NULL_POINTER;
    
    int fields_to_write = (writer->header_count > 0) ? 
        (field_count < writer->header_count ? field_count : writer->header_count) : 
        field_count;
    
    for (int i = 0; i < fields_to_write; i++) {
        if (i > 0) {
            if (fputc(writer->delimiter, writer->file) == EOF) {
                return CSV_WRITER_ERROR_FILE_WRITE;
            }
        }
        
        FieldWriteOptions options = {
            .field = (i < field_count) ? fields[i] : NULL,
            .delimiter = writer->delimiter,
            .enclosure = writer->enclosure,
            .escape = writer->escape,
            .needs_quoting = false
        };
        
                CSVWriterResult result = write_field(writer->file, &options);
        if (result != CSV_WRITER_OK) return result;
    }

    if (writer->header_count > 0) {
        for (int i = fields_to_write; i < writer->header_count; i++) {
            if (fputc(writer->delimiter, writer->file) == EOF) {
                return CSV_WRITER_ERROR_FILE_WRITE;
            }
            
        }
    }
    
    if (fprintf(writer->file, "\r\n") < 0) return CSV_WRITER_ERROR_FILE_WRITE;
    return CSV_WRITER_OK;
}

CSVWriterResult csv_writer_write_record_map(CSVWriter *writer, char **field_names, char **field_values, int field_count) {
    if (!writer || !writer->file) return CSV_WRITER_ERROR_NULL_POINTER;
    if (!field_names || !field_values) return CSV_WRITER_ERROR_NULL_POINTER;
    if (writer->header_count <= 0) return CSV_WRITER_ERROR_INVALID_FIELD_COUNT;
    
    if (writer->header_count > MAX_FIELDS) return CSV_WRITER_ERROR_BUFFER_OVERFLOW;
    
    char *ordered_fields[MAX_FIELDS];
    
    for (int i = 0; i < writer->header_count; i++) {
        ordered_fields[i] = NULL;
    }
    
    for (int i = 0; i < field_count; i++) {
        if (!field_names[i]) continue;
        
        for (int j = 0; j < writer->header_count; j++) {
            if (writer->headers[j] && strcmp(field_names[i], writer->headers[j]) == 0) {
                ordered_fields[j] = field_values[i];
                break;
            }
        }
    }
    
    return csv_writer_write_record(writer, ordered_fields, writer->header_count);
}

CSVWriterResult csv_writer_flush(CSVWriter *writer) {
    if (!writer || !writer->file) return CSV_WRITER_ERROR_NULL_POINTER;
    
    if (fflush(writer->file) != 0) return CSV_WRITER_ERROR_FILE_WRITE;
    return CSV_WRITER_OK;
}

void csv_writer_free(CSVWriter *writer) {
    if (!writer) return;
    
    if (writer->file && writer->owns_file) {
        fflush(writer->file);
        fclose(writer->file);
    }
    
    if (writer->config && writer->owns_config) {
        csv_config_free(writer->config);
    }
    

} 
