#include "csv_config.h"

CSVConfig* csv_config_create(Arena *arena) {
    void *ptr;
    ArenaResult result = arena_alloc(arena, sizeof(CSVConfig), &ptr);
    if (result != ARENA_OK) return NULL;
    
    CSVConfig *config = (CSVConfig*)ptr;
    memset(config, 0, sizeof(CSVConfig));
    
    config->delimiter = ',';
    config->enclosure = '"';
    config->escape = '"';
    config->hasHeader = true;
    config->encoding = CSV_ENCODING_UTF8;
    config->writeBOM = false;
    config->strictMode = false;
    config->skipEmptyLines = false;
    config->trimFields = false;
    config->preserveQuotes = false;
    config->autoFlush = true;  // Default to true for immediate visibility
    
    return config;
}

void csv_config_free(CSVConfig *config) {
    if (config) {
        memset(config, 0, sizeof(CSVConfig));
    }
}

CSVConfig* csv_config_copy(Arena *arena, const CSVConfig *config) {
    if (!config) return NULL;
    
    void *ptr;
    ArenaResult result = arena_alloc(arena, sizeof(CSVConfig), &ptr);
    if (result != ARENA_OK) return NULL;
    
    CSVConfig *copy = (CSVConfig*)ptr;
    memcpy(copy, config, sizeof(CSVConfig));
    return copy;
}

char csv_config_get_delimiter(const CSVConfig *config) {
    return config ? config->delimiter : ',';
}

char csv_config_get_enclosure(const CSVConfig *config) {
    return config ? config->enclosure : '"';
}

char csv_config_get_escape(const CSVConfig *config) {
    return config ? config->escape : '"';
}

const char* csv_config_get_path(const CSVConfig *config) {
    return config ? config->path : NULL;
}

int csv_config_get_offset(const CSVConfig *config) {
    return config ? config->offset : 0;
}

int csv_config_get_limit(const CSVConfig *config) {
    return config ? config->limit : 0;
}

bool csv_config_has_header(const CSVConfig *config) {
    return config ? config->hasHeader : false;
}

CSVEncoding csv_config_get_encoding(const CSVConfig *config) {
    return config ? config->encoding : CSV_ENCODING_UTF8;
}

bool csv_config_get_write_bom(const CSVConfig *config) {
    return config ? config->writeBOM : false;
}

bool csv_config_get_strict_mode(const CSVConfig *config) {
    return config ? config->strictMode : true;
}

bool csv_config_get_skip_empty_lines(const CSVConfig *config) {
    return config ? config->skipEmptyLines : false;
}

bool csv_config_get_trim_fields(const CSVConfig *config) {
    return config ? config->trimFields : false;
}

bool csv_config_get_preserve_quotes(const CSVConfig *config) {
    return config ? config->preserveQuotes : false;
}

bool csv_config_get_auto_flush(const CSVConfig *config) {
    return config ? config->autoFlush : true;  // Default to true for safety
}

void csv_config_set_delimiter(CSVConfig *config, char delimiter) {
    if (config) config->delimiter = delimiter;
}

void csv_config_set_enclosure(CSVConfig *config, char enclosure) {
    if (config) config->enclosure = enclosure;
}

void csv_config_set_escape(CSVConfig *config, char escape) {
    if (config) config->escape = escape;
}

void csv_config_set_path(CSVConfig *config, const char *path) {
    if (config && path) {
        strncpy(config->path, path, MAX_PATH_LENGTH - 1);
        config->path[MAX_PATH_LENGTH - 1] = '\0';
    }
}

void csv_config_set_offset(CSVConfig *config, int offset) {
    if (config) config->offset = offset;
}

void csv_config_set_limit(CSVConfig *config, int limit) {
    if (config) config->limit = limit;
}

void csv_config_set_has_header(CSVConfig *config, bool hasHeader) {
    if (config) config->hasHeader = hasHeader;
}

void csv_config_set_encoding(CSVConfig *config, CSVEncoding encoding) {
    if (config) config->encoding = encoding;
}

void csv_config_set_write_bom(CSVConfig *config, bool writeBOM) {
    if (config) config->writeBOM = writeBOM;
}

void csv_config_set_strict_mode(CSVConfig *config, bool strictMode) {
    if (config) config->strictMode = strictMode;
}

void csv_config_set_skip_empty_lines(CSVConfig *config, bool skipEmptyLines) {
    if (config) config->skipEmptyLines = skipEmptyLines;
}

void csv_config_set_trim_fields(CSVConfig *config, bool trimFields) {
    if (config) config->trimFields = trimFields;
}

void csv_config_set_preserve_quotes(CSVConfig *config, bool preserveQuotes) {
    if (config) config->preserveQuotes = preserveQuotes;
}

void csv_config_set_auto_flush(CSVConfig *config, bool autoFlush) {
    if (config) config->autoFlush = autoFlush;
} 
