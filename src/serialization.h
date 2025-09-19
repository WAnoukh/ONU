#ifndef SERIALIZATION_H
#define SERIALIZATION_H

struct PathSequence;
struct Level;
struct Game;

int serialize_level(const struct Level *level, const char* path);

int deserialize_level_into_game(struct Game *game, const char *path);

int deserialize_level(struct Level *out_level, const char *path);

int serialize_path_sequence(struct PathSequence path_seq, const char *path);

int deserialize_path_sequence(struct PathSequence *out_path_seq, const char *path);

int deserialize_sequence_into_game(struct Game *game, char *path);

#endif // LEVEL_SERIALIZATION_H
