#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
uniform mat3 view;

void main()
{
    vec3 pos = view * vec3(aPos.xy, 1.0);
    gl_Position = vec4(pos.xy, 0.0, 1.0);
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}
