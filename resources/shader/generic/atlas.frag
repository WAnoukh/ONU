#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform vec3 color;
uniform sampler2D texture;
uniform float alpha;

void main()
{
    FragColor = texture2D(texture, TexCoord)* vec4(color, alpha);
}
