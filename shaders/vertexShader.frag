#version 460

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
in vec3 camera;
out vec3 pos_;
out vec2 uv_;
uniform mat4 MVP;

void main()
{
    uv_ = uv;
    gl_Position = vec4(position,1);//MVP * vec4(position,1);
}
