#version 460

uniform float time;
uniform vec2 resolution;

out vec3 output_color;

float circle(vec2 pt, float radius)
{
	return step(length(pt), cos(pt.x*4.0*(1.-time)) / cos(pt.y*4.0*(1.-time)) + radius/2. + sin(pt.x * radius * 4.*time) *cos(pt.y * radius*4.*time));}

void main( void )
{
	/*vec2 uv = (.xy * 2. - resolution) / resolution.y * 5.; //* mix(0.067, 3.0, tw());
	vec3 color;
	float t = uv.x * 72. + time * 50. + tw() * 1000.;
	for (int i = 0; i < 3; i++) {
		float d = abs(uv.y - sin(radians(t)) * .3 * float(i) * sin(time));
		color[i] = .006 / (d * d);
		t += 120.;
	}
    color += circle(uv, 3.6);*/
	output_color = vec3(time, 0 , 0);
}
