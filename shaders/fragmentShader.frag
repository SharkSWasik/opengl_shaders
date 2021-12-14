#version 460

// Global Shader variables
uniform float time;
uniform vec2 resolution;
uniform samplerCube sky_sampler;
uniform mat4 MVP;
uniform vec3 camera_look_at;
uniform vec3 camera_position;

// Linkage into shader from previous stage
in vec3 pos_;
in vec4 position_;

// Linkage out of the shader to next stage
out vec4 output_color;

void main()
{
    output_color = texture(sky_sampler, pos_);
}
