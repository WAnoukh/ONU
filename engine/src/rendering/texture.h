#ifndef TEXTURE_H
#define TEXTURE_H

typedef unsigned int GLuint;

struct TextureAtlas
{
    unsigned int texture_id;
    int width;
    int height;
};

struct TextureInfo
{
    GLuint texture_slot;
    GLuint texture_key;

    struct TextureAtlas atlas_font;
    struct TextureAtlas atlas_tilemap;
};

void texture_set_info(struct TextureInfo *info);

struct TextureInfo *texture_get_info();

struct TextureAtlas get_texture_font_atlas();

struct TextureAtlas get_atlas_tilemap();

void load_default_images();

GLuint texture_load(char* path);

GLuint texture_load_hdr(const char* path);

unsigned int get_texture_slot();

unsigned int get_texture_key();

static inline void atlas_index_to_coordinates(struct TextureAtlas atlas, int index, int *out_x, int *out_y)
{
    *out_y = index / atlas.width;
    *out_x = index - (*out_y) * atlas.width;
}

#endif // TEXTURE_H
