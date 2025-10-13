#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aTexCoord;

out vec2 TexCoord;

uniform vec2 atlas_size;
uniform vec2 atlas_pos;

uniform mat3 view;

void main()
{
    vec3 pos = view * vec3(aPos.xy, 1.0);
    gl_Position = vec4(pos.xy, 0.0, 1.0);
    vec2 corrected_pos = vec2(atlas_pos.x, atlas_size.y - atlas_pos.y - 1);
    TexCoord = (vec2(aTexCoord.x, aTexCoord.y) + corrected_pos) / atlas_size;
    //TexCoord = (vec2(aTexCoord.x, aTexCoord.y));
}
