#version 460

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;

in vec3 camera;

out vec2 text_coord;
out vec3 pos_;
uniform mat4 MVP;

void main()
{
    pos_ = (position + 1) / 2;
    text_coord = uv;
    //gl_Position = vec4(position, 1);
    gl_Position = vec4(position,1);
}
