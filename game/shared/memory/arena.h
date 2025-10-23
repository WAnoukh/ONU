#ifndef ARENA_H
#define ARENA_H

#include <stdint.h>

struct Arena
{
    char* start;
    char* end;
    char* offset;
};

struct Arena arena_init(int size);

void arena_deinit(struct Arena *arena);

void *arena_allocate(struct Arena *arena, uint64_t size);

void *arena_allocate_align(struct Arena *arena, uint64_t size, uint64_t align);

void arena_reset(struct Arena *arena);

#endif // ARENA_H
