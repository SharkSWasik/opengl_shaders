#version 460

layout(location = 0) in vec3 position;
out vec3 pos;

void main()
{
    gl_Position.xyz = position;
    gl_Position.w = 1.0;
    pos = (position + 1) / 2;
}
