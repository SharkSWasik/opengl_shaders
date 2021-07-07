#version 460

uniform float time;
uniform vec2 resolution;

out vec4 output_color;
in vec3 pos;


float tw()
{
	return sin(time) * 0.5 + 0.5;
}

float circle(vec2 pt, float radius)
{
	return step(length(pt), cos(pt.x*4.0*(1.-tw())) / cos(pt.y*4.0*(1.-tw())) + radius/2. + sin(pt.x * radius * 4.*tw()) *cos(pt.y * radius*4.*tw()));}


void main( void )
{
	vec2 uv = (pos.xy * 2. - resolution) / resolution.y * 5.; //* mix(0.067, 3.0, tw());
	vec3 color;
	float t = uv.x * 72. + time * 50. + tw() * 1000.;
	for (int i = 0; i < 3; i++) {
		float d = abs(uv.y - sin(radians(t)) * .3 * float(i) * sin(time));
		color[i] = .006 / (d * d);
		t += 120.;
	}
	color += circle(uv, 3.6);
	output_color = vec4(color, 1);
}

