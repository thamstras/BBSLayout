#version 330 core
in vec2 TexCoords;
in vec4 VertColor;
out vec4 color;

uniform sampler2D tex;

void main()
{
    color = VertColor * texture(tex, TexCoords);
}