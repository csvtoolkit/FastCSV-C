#include "csv_config.h"

CSVConfig* csv_config_create(Arena *arena) {
    if (!arena) {
        return NULL;
    }
    
    void *ptr;
    ArenaResult result = arena_alloc(arena, sizeof(CSVConfig), &ptr);
    if (result != ARENA_OK) {
        return NULL;
    }
    CSVConfig *config = (CSVConfig*)ptr;
    
    config->delimiter = ',';
    config->enclosure = '"';
    config->escape = '"';
    config->path[0] = '\0';
    config->offset = 0;
    config->hasHeader = true;
    config->limit = 0;
    
    return config;
}

void csv_config_free(CSVConfig *config) {

    (void)config;
}

CSVConfig* csv_config_copy(Arena *arena, const CSVConfig *config) {
    if (!config || !arena) {
        return NULL;
    }
    
    void *ptr;
    ArenaResult result = arena_alloc(arena, sizeof(CSVConfig), &ptr);
    if (result != ARENA_OK) {
        return NULL;
    }
    CSVConfig *copy = (CSVConfig*)ptr;
    
    *copy = *config;
    return copy;
}


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

int csv_config_get_limit(const CSVConfig *config) {
    return config ? config->limit : 0;
}

bool csv_config_has_header(const CSVConfig *config) {
    return config ? config->hasHeader : true;
}


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
    if (config) {
        if (path) {
            strncpy(config->path, path, MAX_PATH_LENGTH - 1);
            config->path[MAX_PATH_LENGTH - 1] = '\0';
        } else {
            config->path[0] = '\0';
        }
    }
}

void csv_config_set_offset(CSVConfig *config, int offset) {
    if (config) {
        config->offset = offset;
    }
}

void csv_config_set_limit(CSVConfig *config, int limit) {
    if (config) {
        config->limit = limit;
    }
}

void csv_config_set_has_header(CSVConfig *config, bool hasHeader) {
    if (config) {
        config->hasHeader = hasHeader;
    }
} 
