#include "csv_writer.h"
#include <string.h>

CSVWriter* csv_writer_init(CSVConfig* config, char** headers, int header_count) {
    CSVWriter* writer;
    int i;
    
    if (!config || !csv_config_get_path(config)) {
        return NULL;
    }
    
    writer = malloc(sizeof(CSVWriter));
    if (!writer) {
        return NULL;
    }
    
    writer->file = fopen(csv_config_get_path(config), "w");
    if (!writer->file) {
        free(writer);
        return NULL;
    }
    
    // Copy the config
    writer->config = csv_config_copy(config);
    if (!writer->config) {
        fclose(writer->file);
        free(writer);
        return NULL;
    }
    
    // Cache config values for performance
    writer->delimiter = csv_config_get_delimiter(writer->config);
    writer->enclosure = csv_config_get_enclosure(writer->config);
    writer->escape = csv_config_get_escape(writer->config);
    
    writer->headers = malloc(header_count * sizeof(char*));
    if (!writer->headers) {
        csv_config_free(writer->config);
        fclose(writer->file);
        free(writer);
        return NULL;
    }
    
    writer->header_count = header_count;
    for (i = 0; i < header_count; i++) {
        writer->headers[i] = malloc(strlen(headers[i]) + 1);
        if (!writer->headers[i]) {
            int j;
            for (j = 0; j < i; j++) {
                free(writer->headers[j]);
            }
            free(writer->headers);
            csv_config_free(writer->config);
            fclose(writer->file);
            free(writer);
            return NULL;
        }
        strcpy(writer->headers[i], headers[i]);
    }
    
    // Only write headers if we have any - support writing without headers
    if (header_count > 0) {
        for (i = 0; i < header_count; i++) {
            if (i > 0) {
                fputc(writer->delimiter, writer->file);
            }
            write_field(writer->file, headers[i], writer->delimiter, writer->enclosure, writer->escape);
        }
        fprintf(writer->file, "\r\n");
    }
    
    return writer;
}

void write_field(FILE* file, const char* field, char delimiter, char enclosure, char escape) {
    const char* p;
    int needs_quotes = 0;
    
    if (!field) {
        return;
    }
    
    // Optimized quote detection using strchr for common cases
    if (strchr(field, delimiter) || strchr(field, enclosure) || 
        strchr(field, '\r') || strchr(field, '\n')) {
        needs_quotes = 1;
    }
    
    if (needs_quotes) {
        fputc(enclosure, file);
        for (p = field; *p; p++) {
            if (*p == enclosure) {
                fputc(enclosure, file);
                fputc(enclosure, file);
            } else {
                fputc(*p, file);
            }
        }
        fputc(enclosure, file);
    } else {
        fputs(field, file);
    }
}

int csv_writer_write_record(CSVWriter* writer, char** fields, int field_count) {
    int fields_to_write;
    int i;
    
    if (!writer || !writer->file || !writer->config) {
        return -1;
    }
    
    // If no headers are set, write all provided fields
    // Otherwise, limit to header count for consistency
    if (writer->header_count == 0) {
        fields_to_write = field_count;
    } else {
        fields_to_write = (field_count < writer->header_count) ? field_count : writer->header_count;
    }
    
    // Use cached values instead of function calls
    for (i = 0; i < fields_to_write; i++) {
        if (i > 0) {
            fputc(writer->delimiter, writer->file);
        }
        write_field(writer->file, fields[i], writer->delimiter, writer->enclosure, writer->escape);
    }
    
    // Only fill remaining columns with empty fields if we have headers
    if (writer->header_count > 0) {
        for (i = fields_to_write; i < writer->header_count; i++) {
            if (i > 0) {
                fputc(writer->delimiter, writer->file);
            }
        }
    }
    
    fprintf(writer->file, "\r\n");
    // Removed fflush() - let OS handle buffering for better performance
    
    return 0;
}

int csv_writer_write_record_map(CSVWriter* writer, char** field_names, char** field_values, int field_count) {
    char* ordered_fields[MAX_FIELDS];
    int i, j;
    
    if (!writer || !writer->file || !writer->config) {
        return -1;
    }
    
    // Initialize ordered fields
    for (i = 0; i < MAX_FIELDS; i++) {
        ordered_fields[i] = NULL;
    }
    
    // Map field names to positions
    for (i = 0; i < field_count; i++) {
        for (j = 0; j < writer->header_count; j++) {
            if (strcmp(field_names[i], writer->headers[j]) == 0) {
                ordered_fields[j] = field_values[i];
                break;
            }
        }
    }
    
    // Write record using cached values
    for (i = 0; i < writer->header_count; i++) {
        if (i > 0) {
            fputc(writer->delimiter, writer->file);
        }
        write_field(writer->file, ordered_fields[i], writer->delimiter, writer->enclosure, writer->escape);
    }
    
    fprintf(writer->file, "\r\n");
    // Removed fflush() - let OS handle buffering for better performance
    
    return 0;
}

void csv_writer_free(CSVWriter* writer) {
    int i;
    
    if (writer) {
        if (writer->file) {
            fflush(writer->file);  // Only flush when closing
            fclose(writer->file);
        }
        if (writer->headers) {
            for (i = 0; i < writer->header_count; i++) {
                free(writer->headers[i]);
            }
            free(writer->headers);
        }
        if (writer->config) {
            csv_config_free(writer->config);
        }
        free(writer);
    }
} 