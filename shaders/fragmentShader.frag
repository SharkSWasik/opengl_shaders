#version 460

uniform float time;
uniform vec2 resolution;
uniform samplerCube sky_sampler;

out vec4 output_color;

in vec3 pos_;

void main( void )
{
	output_color = texture(sky_sampler, pos_);
}

