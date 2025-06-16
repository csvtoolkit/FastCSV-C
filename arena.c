#include "arena.h"
#include <stdlib.h>
#include <string.h>

const char* arena_error_string(ArenaResult result) {
    switch (result) {
        case ARENA_OK: return "Success";
        case ARENA_ERROR_NULL_POINTER: return "Null pointer error";
        case ARENA_ERROR_MEMORY_ALLOCATION: return "Memory allocation failed";
        case ARENA_ERROR_OUT_OF_MEMORY: return "Arena out of memory";
        case ARENA_ERROR_INVALID_SIZE: return "Invalid size";
        default: return "Unknown error";
    }
}

ArenaResult arena_create(Arena *arena, size_t size) {
    if (!arena) return ARENA_ERROR_NULL_POINTER;
    if (size == 0) return ARENA_ERROR_INVALID_SIZE;
    
    arena->memory = malloc(size);
    if (!arena->memory) return ARENA_ERROR_MEMORY_ALLOCATION;
    
    arena->current = arena->memory;
    arena->end = arena->memory + size;
    arena->total_size = size;
    arena->used_size = 0;
    arena->owns_memory = true;
    
    return ARENA_OK;
}

ArenaResult arena_create_with_buffer(Arena *arena, void *buffer, size_t size) {
    if (!arena || !buffer) return ARENA_ERROR_NULL_POINTER;
    if (size == 0) return ARENA_ERROR_INVALID_SIZE;
    
    arena->memory = (char*)buffer;
    arena->current = arena->memory;
    arena->end = arena->memory + size;
    arena->total_size = size;
    arena->used_size = 0;
    arena->owns_memory = false;
    
    return ARENA_OK;
}

void arena_reset(Arena *arena) {
    if (!arena || !arena->memory) return;
    
    arena->current = arena->memory;
    arena->used_size = 0;
}

void arena_destroy(Arena *arena) {
    if (!arena) return;
    
    if (arena->memory && arena->owns_memory) {
        free(arena->memory);
    }
    
    memset(arena, 0, sizeof(Arena));
}

ArenaResult arena_alloc(Arena *arena, size_t size, void **ptr) {
    if (!arena || !ptr) return ARENA_ERROR_NULL_POINTER;
    if (!arena->memory) return ARENA_ERROR_NULL_POINTER;
    if (size == 0) return ARENA_ERROR_INVALID_SIZE;
    
    size_t aligned_size = (size + 7) & ~7;
    
    if (arena->current + aligned_size > arena->end) {
        *ptr = NULL;
        return ARENA_ERROR_OUT_OF_MEMORY;
    }
    
    *ptr = arena->current;
    arena->current += aligned_size;
    arena->used_size += aligned_size;
    
    return ARENA_OK;
}

char* arena_strdup(Arena *arena, const char *str) {
    if (!arena || !str) return NULL;
    
    size_t len = strlen(str);
    void *ptr;
    ArenaResult result = arena_alloc(arena, len + 1, &ptr);
    if (result != ARENA_OK) return NULL;
    
    char *copy = (char*)ptr;
    memcpy(copy, str, len);
    copy[len] = '\0';
    return copy;
}

void* arena_realloc(Arena *arena, void *ptr, size_t old_size, size_t new_size) {
    if (!arena) return NULL;
    if (new_size == 0) return NULL;
    
    if (!ptr) {
        void *new_ptr;
        ArenaResult result = arena_alloc(arena, new_size, &new_ptr);
        return (result == ARENA_OK) ? new_ptr : NULL;
    }
    
    if (new_size <= old_size) {
        return ptr;
    }
    
    void *new_ptr;
    ArenaResult result = arena_alloc(arena, new_size, &new_ptr);
    if (result != ARENA_OK) return NULL;
    
    if (old_size > 0) {
        memcpy(new_ptr, ptr, old_size);
    }
    
    return new_ptr;
}

size_t arena_get_used_size(const Arena *arena) {
    if (!arena) return 0;
    return arena->used_size;
}

size_t arena_get_free_size(const Arena *arena) {
    if (!arena || !arena->memory) return 0;
    return arena->total_size - arena->used_size;
}

bool arena_can_allocate(const Arena *arena, size_t size) {
    if (!arena || !arena->memory) return false;
    
    size_t aligned_size = (size + 7) & ~7;
    return (arena->current + aligned_size <= arena->end);
}

ArenaRegion arena_begin_region(Arena *arena) {
    ArenaRegion region = {0};
    if (arena) {
        region.arena = arena;
        region.checkpoint = arena->current;
        region.used_at_checkpoint = arena->used_size;
    }
    return region;
}

void arena_end_region(ArenaRegion *region) {
    if (!region || !region->arena) return;
    
    region->arena->current = region->checkpoint;
    region->arena->used_size = region->used_at_checkpoint;
}

ArenaResult arena_restore_region(ArenaRegion *region) {
    if (!region || !region->arena) return ARENA_ERROR_NULL_POINTER;
    if (!region->checkpoint) return ARENA_ERROR_INVALID_SIZE;
    
    if (region->checkpoint < region->arena->memory || 
        region->checkpoint > region->arena->end) {
        return ARENA_ERROR_INVALID_SIZE;
    }
    
    region->arena->current = region->checkpoint;
    region->arena->used_size = region->used_at_checkpoint;
    return ARENA_OK;
} 

