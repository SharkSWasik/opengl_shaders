#version 460

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;

out vec3 pos_;
uniform mat4 MVP;

void main()
{
    pos_ = position;
    gl_Position = MVP * vec4(position,1);
}
