#ifndef LEVEL_SEQUENCE_H
#define LEVEL_SEQUENCE_H

struct Arena;

struct PathSequence 
{
    char **pathes;
    int pathes_count;
};

struct Sequence
{
    struct Level *levels;
    int levels_count;
};

int sequence_load_path_sequence(struct Arena *arena, struct PathSequence path_seq, struct Sequence *out_sequence);

void path_sequence_deinit(struct PathSequence *path_seq);

#endif // LEVEL_SEQUENCE_H
