#include "level.h"
#include "level_sequence.h"
#include "level_serialization.h"

#define DEFAULT_PATHES_COUNT 3
char *default_pathes[] = {"resources/level/level1.level", "resources/level/level2.level", "resources/level/level3.level"};

struct PathSequence get_default_path_sequence()
{
    struct PathSequence path_seq;
    path_seq.pathes = default_pathes;
    path_seq.pathes_count = DEFAULT_PATHES_COUNT;
    return path_seq;
}

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
