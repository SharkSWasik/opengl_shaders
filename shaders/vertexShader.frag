#version 460

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
in vec2 ripples;
in vec3 drops;

in vec3 camera;

out vec3 pos_;
out vec4 position_;
out vec3 drops_;
out vec2 ripples_;

uniform mat4 MVP;

void main()
{
    pos_ = position;
    gl_Position = MVP * vec4(position,1);
    position_ = gl_Position;
    ripples_ = ripples;
    drops_ = drops;
    if (uv == 121)
        gl_Position = vec4(0,0,0,0);
}
