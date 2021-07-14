#version 460

uniform float time;
uniform vec2 resolution;
uniform sampler2D sky_sampler;

out vec4 output_color;
in vec3 pos_;
in vec2 text_coord;

uniform vec3 camera_;

vec3 col;

vec3 lighting(vec3 direction)
{
    float light = dot(direction,sqrt(vec3(.3,.5,.2)));

    return vec3(light*.5+.5);
}

float smin(float d1, float d2)
{
    // source: https://www.iquilezles.org/www/articles/smin/smin.htm
    const float e = -6.;
    return log(exp(d1*e)+exp(d2*e))/e;
}

float sd_ripple(vec3 p, vec2 offset)
{
    // example of basic ripple equation: sin(10(x^2+y^2))/10
    float amplitude = 0.4;
    float frequence = 1.;
    float speed = 3.14;
    float height_offset = 0.8;
    float time_offset = 0.;
    float l = dot(p.xz + offset, p.xz + offset);
    float attenuation = 60. / (20. * (l + 1.));

    return p.y + height_offset + attenuation * amplitude * sin(frequence * l - time * speed - time_offset) / (1. + l);
}

float sd_sphere(vec3 p, vec2 offset, float ball_size)
{
    float falling_time = 1.;
    float dropping_height = 2.;
    float height = -10. * pow(mod(time/2.,falling_time)+0.1, 2.) + dropping_height;
    float drop = length(p + vec3(offset.x, -height, offset.y)) - ball_size;
    return drop;
}

float dist(vec3 p, bool change_colors)
{
    float ripple = sd_ripple(p, vec2(1.0));

    float drop = sd_sphere(p, vec2(1.0), .4);

    float closest = min(ripple, drop);

    if (change_colors)
    {
	if (closest == ripple)
            col = vec3(0.,0.,0.1);
    	else if (closest == drop)
            col = vec3(0.1,0.1,0.1);
    }
    return min(ripple,drop);
}

vec3 get_normal(vec3 p)
{
    vec2 e = vec2(1,-1)*.01;

    return normalize(dist(p-e.yxx, false) * e.yxx +
		     dist(p-e.xyx, false) * e.xyx +
		     dist(p-e.xxy, false) * e.xxy +
		     dist(p-e.y  , false) * e.y);
}

vec4 raymarcher(vec3 p, vec3 ray_direction)
{
    vec4 result = vec4(p,0);
    float max_distance = 13.;
    for(int i = 0; i < 50; i++)
    {
        float closest_dist = dist(result.xyz, true);
        result += vec4(ray_direction,1)*closest_dist;

        if (closest_dist < .01 || result.w > max_distance)
            break;
    }
    if (result.w > max_distance)
        col = texture(sky_sampler, text_coord).xyz;
    return result;
}

mat3 setCamera(vec3 camera_position, vec3 camera_look_at)
{
    vec3 camera_direction = normalize(camera_look_at - camera_position);
    vec3 camera_right = normalize(cross(camera_direction, vec3(0., 1., 0.)));
    vec3 camera_up = cross(camera_right,camera_direction);
    return mat3(camera_right, camera_up, camera_direction);
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    //camera
    vec3 camera_look_at = vec3(0.0);
    vec3 camera_position = camera_look_at + vec3(4.5 * abs(cos(0.4 * time)), 1.3, 4.5 * abs(sin(0.3 * time)));
    float focal_length = 1.;

    mat3 camera = setCamera(camera_position, camera_look_at);
    vec2 position = (fragCoord - resolution/2.) / min(resolution.x, resolution.y);

    //raymarcher
    vec3 ray_direction = camera * normalize(vec3(position.xy, focal_length));
    vec4 march = raymarcher(camera_position, ray_direction);

    //lighting
    vec3 normal = get_normal(march.xyz);
    vec3 refraction_direction = refract(ray_direction,normal,.75);
    vec3 refracted = lighting(refraction_direction);
    vec3 ambient = lighting(ray_direction)*.5+.5;

    if (col != texture(sky_sampler, text_coord).xyz)
        col += mix(refracted,ambient,0.6);

    fragColor = vec4(col*col,1);
}

void main(void)
{
    mainImage(output_color, pos_.xy);
}

