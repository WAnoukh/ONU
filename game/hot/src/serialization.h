#ifndef SERIALIZATION_H
#define SERIALIZATION_H

struct PathSequence;
struct Level;
struct Game;

struct Version
{
    int major;
    int minor;
    int patch;
};

static inline int compare_version_value(struct Version version, int major, int minor, int patch)
{
    int major_diff = version.major - major;
    if(major_diff)
    {
        return major_diff;
    }
    int minor_diff = version.minor - minor;
    if(minor_diff)
    {
        return minor_diff;
    }
    return version.patch - patch;
}

static inline int compare_version(struct Version a, struct Version b)
{
    return compare_version_value(a, b.major, b.minor, b.patch);
}

int serialize_level(const struct Level *level, const char* path);

int deserialize_level_into_game(struct Game *game, const char *path);

int deserialize_level(struct Level *out_level, const char *path);

int serialize_path_sequence(struct PathSequence path_seq, const char *path);

int deserialize_path_sequence(struct PathSequence *out_path_seq, const char *path);

int deserialize_sequence_into_game(struct Game *game, char *path);

#endif // LEVEL_SERIALIZATION_H
