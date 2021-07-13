#version 460

uniform float time;
uniform vec2 resolution;
uniform samplerCube cubemap;

out vec4 output_color;

in vec3 color;

void main( void )
{
	output_color = texture(cubemap, color);
}

