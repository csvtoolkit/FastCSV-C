#ifndef ARENA_H
#define ARENA_H

#include <stddef.h>
#include <stdbool.h>

#define ARENA_DEFAULT_SIZE (1024 * 1024)

typedef struct {
    char *memory;
    char *current;
    char *end;
    size_t total_size;
    size_t used_size;
    bool owns_memory;
} Arena;

typedef enum {
    ARENA_OK = 0,
    ARENA_ERROR_NULL_POINTER,
    ARENA_ERROR_MEMORY_ALLOCATION,
    ARENA_ERROR_OUT_OF_MEMORY,
    ARENA_ERROR_INVALID_SIZE
} ArenaResult;

ArenaResult arena_create(Arena *arena, size_t size);
ArenaResult arena_create_with_buffer(Arena *arena, void *buffer, size_t size);
void arena_reset(Arena *arena);
void arena_destroy(Arena *arena);

ArenaResult arena_alloc(Arena *arena, size_t size, void **ptr);
char* arena_strdup(Arena *arena, const char *str);
void* arena_realloc(Arena *arena, void *ptr, size_t old_size, size_t new_size);

size_t arena_get_used_size(const Arena *arena);
size_t arena_get_free_size(const Arena *arena);
bool arena_can_allocate(const Arena *arena, size_t size);


typedef struct {
    Arena *arena;
    char *checkpoint;
    size_t used_at_checkpoint;
} ArenaRegion;

ArenaRegion arena_begin_region(Arena *arena);
void arena_end_region(ArenaRegion *region);
ArenaResult arena_restore_region(ArenaRegion *region);

const char* arena_error_string(ArenaResult result);

#endif

