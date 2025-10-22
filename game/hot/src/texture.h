#ifndef TEXTURE_H
#define TEXTURE_H

struct TextureAtlas
{
    unsigned int texture_id;
    int width;
    int height;
};

unsigned int get_texture_slot();

unsigned int get_texture_key();

struct TextureAtlas get_texture_font_atlas();

struct TextureAtlas get_atlas_tilemap();

void load_default_images();

static inline void atlas_index_to_coordinates(struct TextureAtlas atlas, int index, int *out_x, int *out_y)
{
    *out_y = index / atlas.width;
    *out_x = index - (*out_y) * atlas.width;
}

#endif // TEXTURE_H
