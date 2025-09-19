#include "level.h"
#include "level_sequence.h"
#include "serialization.h"

int sequence_load_path_sequence(struct PathSequence path_seq, struct Sequence *out_sequence)
{
    struct Sequence sequence;
    sequence.levels_count = path_seq.pathes_count;
    sequence.levels = malloc(sizeof(struct Level)*path_seq.pathes_count);
    int fail_index = -1; 
    for(int i = 0; i < path_seq.pathes_count; ++i)
    {
        char *path = *(path_seq.pathes+i);
        if(!deserialize_level(sequence.levels+i, path))
        {
            fail_index = i;
            printf("Error : the loading of %s failed during sequence load.\n", path);
            break;
        }
    }
    if(fail_index >= 0)
    {
        for(int j = 0; j <= fail_index; ++j)
        {
            level_deinit(sequence.levels+j);
        }
        return 0;
    }
    *out_sequence = sequence;
    return 1;
}

void path_sequence_deinit(struct PathSequence *path_seq)
{
    for(int i = 0; i < path_seq->pathes_count; ++i)
    {
        free(path_seq->pathes[i]);
    }
    free(path_seq->pathes);
}
