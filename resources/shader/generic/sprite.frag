#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform vec3 color;
uniform sampler2D texture;

void main()
{
    FragColor = texture2D(texture, TexCoord)* vec4(color, 1.f);
    //FragColor = vec4(TexCoord, 0.f, 1.0f);
}
