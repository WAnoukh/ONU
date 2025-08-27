#ifndef LEVEL_SERIALIZATION_H
#define LEVEL_SERIALIZATION_H
struct Level;

int serialize_level(struct Level level, const char* path);

int deserialize_level(struct Level *out_level, const char *path);

#endif // LEVEL_SERIALIZATION_H
