#version 460

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;

out vec3 pos;
out vec2 text_coord;


void main()
{
    gl_Position.xyz = position;
    gl_Position.w = 1.0;
    pos = (position + 1) / 2;
    text_coord = uv;
    vec2 test = uv + vec2(1);
}
