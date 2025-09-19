#ifndef LEVEL_SEQUENCE_H
#define LEVEL_SEQUENCE_H

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

struct PathSequence get_default_path_sequence();

int sequence_load_path_sequence(struct PathSequence path_seq, struct Sequence *out_sequence);
#endif // LEVEL_SEQUENCE_H
