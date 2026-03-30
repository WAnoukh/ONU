#include "arena.h"
#include <assert.h>
#include <stdalign.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int arena_init(struct Arena *out_arena, int size)
{
    out_arena->start = malloc(size);
    if(!out_arena->start)
    {
        return 0;
    }
    out_arena->end = out_arena->start + size;
    out_arena->offset = out_arena->start;

    return 1;
}

uint64_t next_power_of_two_64(uint64_t n) {
    if (n == 0) return 1;
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n |= n >> 32;
    n++;
    return n;
}

void arena_deinit(struct Arena *arena)
{
    free(arena->start);
    arena->start = NULL;
    arena->end = NULL;
    arena->offset = NULL;
}

void *arena_allocate(struct Arena *arena, uint64_t size)
{
    return arena_allocate_align(arena, size, next_power_of_two_64(size));
}

void *arena_allocate_align(struct Arena *arena, uint64_t size, uint64_t align)
{
    assert((align & (align - 1)) == 0); // align power of 2

    uint64_t cur_ptr = (uint64_t)arena->offset;
    uint64_t aligned = (cur_ptr + (align - 1)) & ~(uint64_t)(align - 1);
    uint64_t bumped_ptr = aligned + size;
    if(bumped_ptr > (uint64_t)arena->end)
    {
        return NULL;
    }
    arena->offset = (char*)bumped_ptr;
    return (char*)aligned;
}

void arena_reset(struct Arena *arena)
{
    arena->offset= arena->start;
}

int arena_create_patch(struct Arena *base, size_t size, struct Arena *out_arena)
{
    char *start = arena_allocate(base, size);
    if(!start) return 0;

    out_arena->start  = start;
    out_arena->end    = start + size;
    out_arena->offset = start;
    return 1;
}

char *arena_get_offset(struct Arena *arena)
{
    return arena->offset;
}

void arena_set_offset(struct Arena *arena, char *offset)
{
    arena->offset = offset;
}

