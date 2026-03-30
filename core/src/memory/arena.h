#ifndef ARENA_H
#define ARENA_H

#include <stdint.h>

#define ASSERT_ARENA(arena) assert(arena.end > arena.start && arena.offset >= arena.start && arena.offset <= arena.end)
#define MB(nb) nb * 1024 * 1024

typedef char* ArenaOffset;

struct Arena
{
    char* start;
    char* end;
    char* offset;
};

int arena_init(struct Arena *out_arena, int size);

void arena_deinit(struct Arena *arena);

void *arena_allocate(struct Arena *arena, uint64_t size);

void *arena_allocate_align(struct Arena *arena, uint64_t size, uint64_t align);

void arena_reset(struct Arena *arena);

int arena_create_patch(struct Arena *base, size_t size, struct Arena *out_arena);

ArenaOffset arena_get_offset(struct Arena *arena);

void arena_set_offset(struct Arena *arena, ArenaOffset offset);

#endif // ARENA_H
