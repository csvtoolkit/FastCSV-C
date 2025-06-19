#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include "../arena.h"

#define TEST_ARENA_SIZE 1024

void test_arena_create() {
    printf("Testing arena_create...\n");
    
    Arena arena;
    ArenaResult result = arena_create(&arena, TEST_ARENA_SIZE);
    
    assert(result == ARENA_OK);
    assert(arena.memory != NULL);
    assert(arena.current == arena.memory);
    assert(arena.end == arena.memory + TEST_ARENA_SIZE);
    assert(arena.total_size == TEST_ARENA_SIZE);
    assert(arena.used_size == 0);
    assert(arena.owns_memory == true);
    
    arena_destroy(&arena);
    printf("✓ arena_create passed\n");
}

void test_arena_create_null_pointer() {
    printf("Testing arena_create with null pointer...\n");
    
    ArenaResult result = arena_create(NULL, TEST_ARENA_SIZE);
    assert(result == ARENA_ERROR_NULL_POINTER);
    
    printf("✓ arena_create null pointer test passed\n");
}

void test_arena_create_zero_size() {
    printf("Testing arena_create with zero size...\n");
    
    Arena arena;
    ArenaResult result = arena_create(&arena, 0);
    assert(result == ARENA_ERROR_INVALID_SIZE);
    
    printf("✓ arena_create zero size test passed\n");
}

void test_arena_create_with_buffer() {
    printf("Testing arena_create_with_buffer...\n");
    
    char buffer[TEST_ARENA_SIZE];
    Arena arena;
    ArenaResult result = arena_create_with_buffer(&arena, buffer, TEST_ARENA_SIZE);
    
    assert(result == ARENA_OK);
    assert(arena.memory == buffer);
    assert(arena.current == buffer);
    assert(arena.end == buffer + TEST_ARENA_SIZE);
    assert(arena.total_size == TEST_ARENA_SIZE);
    assert(arena.used_size == 0);
    assert(arena.owns_memory == false);
    
    printf("✓ arena_create_with_buffer passed\n");
}

void test_arena_alloc() {
    printf("Testing arena_alloc...\n");
    
    Arena arena;
    arena_create(&arena, TEST_ARENA_SIZE);
    
    void *ptr1, *ptr2;
    ArenaResult result1 = arena_alloc(&arena, 64, &ptr1);
    ArenaResult result2 = arena_alloc(&arena, 32, &ptr2);
    
    assert(result1 == ARENA_OK);
    assert(result2 == ARENA_OK);
    assert(ptr1 != NULL);
    assert(ptr2 != NULL);
    assert(ptr1 != ptr2);
    assert(arena.used_size >= 64 + 32);
    
    arena_destroy(&arena);
    printf("✓ arena_alloc passed\n");
}

void test_arena_alloc_alignment() {
    printf("Testing arena_alloc alignment...\n");
    
    Arena arena;
    arena_create(&arena, TEST_ARENA_SIZE);
    
    void *ptr;
    arena_alloc(&arena, 1, &ptr);
    
    assert(((uintptr_t)ptr % 8) == 0);
    
    arena_destroy(&arena);
    printf("✓ arena_alloc alignment passed\n");
}

void test_arena_alloc_out_of_memory() {
    printf("Testing arena_alloc out of memory...\n");
    
    Arena arena;
    arena_create(&arena, 64);
    
    void *ptr;
    ArenaResult result = arena_alloc(&arena, TEST_ARENA_SIZE, &ptr);
    
    assert(result == ARENA_ERROR_OUT_OF_MEMORY);
    assert(ptr == NULL);
    
    arena_destroy(&arena);
    printf("✓ arena_alloc out of memory test passed\n");
}

void test_arena_strdup() {
    printf("Testing arena_strdup...\n");
    
    Arena arena;
    arena_create(&arena, TEST_ARENA_SIZE);
    
    const char *original = "Hello, World!";
    char *copy = arena_strdup(&arena, original);
    
    assert(copy != NULL);
    assert(strcmp(copy, original) == 0);
    assert(copy != original);
    
    arena_destroy(&arena);
    printf("✓ arena_strdup passed\n");
}

void test_arena_reset() {
    printf("Testing arena_reset...\n");
    
    Arena arena;
    arena_create(&arena, TEST_ARENA_SIZE);
    
    void *ptr;
    arena_alloc(&arena, 64, &ptr);
    size_t used_before = arena.used_size;
    
    arena_reset(&arena);
    
    assert(arena.current == arena.memory);
    assert(arena.used_size == 0);
    assert(used_before > 0);
    
    arena_destroy(&arena);
    printf("✓ arena_reset passed\n");
}

void test_arena_regions() {
    printf("Testing arena regions...\n");
    
    Arena arena;
    arena_create(&arena, TEST_ARENA_SIZE);
    
    void *ptr1;
    arena_alloc(&arena, 64, &ptr1);
    
    ArenaRegion region = arena_begin_region(&arena);
    
    void *ptr2;
    arena_alloc(&arena, 32, &ptr2);
    
    size_t used_before_restore = arena.used_size;
    arena_end_region(&region);
    size_t used_after_restore = arena.used_size;
    
    assert(used_before_restore > used_after_restore);
    
    arena_destroy(&arena);
    printf("✓ arena regions passed\n");
}

void test_arena_can_allocate() {
    printf("Testing arena_can_allocate...\n");
    
    Arena arena;
    arena_create(&arena, 128);
    
    assert(arena_can_allocate(&arena, 64) == true);
    assert(arena_can_allocate(&arena, 256) == false);
    
    void *ptr;
    arena_alloc(&arena, 80, &ptr);
    
    assert(arena_can_allocate(&arena, 64) == false);
    assert(arena_can_allocate(&arena, 32) == true);
    
    arena_destroy(&arena);
    printf("✓ arena_can_allocate passed\n");
}

void test_arena_get_sizes() {
    printf("Testing arena size functions...\n");
    
    Arena arena;
    arena_create(&arena, TEST_ARENA_SIZE);
    
    assert(arena_get_used_size(&arena) == 0);
    assert(arena_get_free_size(&arena) == TEST_ARENA_SIZE);
    
    void *ptr;
    arena_alloc(&arena, 64, &ptr);
    
    assert(arena_get_used_size(&arena) >= 64);
    assert(arena_get_free_size(&arena) < TEST_ARENA_SIZE);
    
    arena_destroy(&arena);
    printf("✓ arena size functions passed\n");
}

int main() {
    printf("Running Arena Tests...\n\n");
    
    test_arena_create();
    test_arena_create_null_pointer();
    test_arena_create_zero_size();
    test_arena_create_with_buffer();
    test_arena_alloc();
    test_arena_alloc_alignment();
    test_arena_alloc_out_of_memory();
    test_arena_strdup();
    test_arena_reset();
    test_arena_regions();
    test_arena_can_allocate();
    test_arena_get_sizes();
    
    printf("\n✅ All Arena tests passed!\n");
    return 0;
} 