#version 330 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec4 color;

out vec2 TexCoords;
out vec4 VertColor;

uniform mat4 model;
uniform mat4 projection;

void main()
{
    TexCoords = uv;
    VertColor = color * 1.9921875;
    gl_Position = projection * model * vec4(position, 0.0, 1.0);
}