#define STB_IMAGE_IMPLEMENTATION
#include "glad/glad.h"
#include <stb_image.h>

#include "texture.h"

GLuint texture_slot;
GLuint texture_key;

struct TextureAtlas font_atlas;

unsigned int get_texture_slot()
{
    return texture_slot;
}

unsigned int get_texture_key()
{
    return texture_key;
}

struct TextureAtlas get_texture_font_atlas()
{
    return font_atlas;
}

GLuint texture_load(char* path)
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(1);
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 4);
    if(data != NULL)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        perror( "Failed to load texture");
        return 0;
    }
    stbi_image_free(data);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    return texture;
}

void load_default_images()
{
   texture_slot = texture_load("resources\\sprite\\slot.png");
   texture_key = texture_load("resources\\sprite\\key.png");
   font_atlas.texture_id = texture_load("resources\\sprite\\font.png");
   font_atlas.width = 10;
   font_atlas.height = 5;
}

