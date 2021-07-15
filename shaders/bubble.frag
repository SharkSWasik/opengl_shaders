#version 460

uniform float time;
uniform vec2 resolution;
uniform samplerCube sky_sampler;
uniform mat4 MVP;

out vec4 output_color;
in vec3 pos_;

in vec4 position_;

uniform vec3 camera_look_at;
uniform vec3 camera_position;

vec3 col;
vec3 blood_color = vec3(0.65,0.06,0.12);

struct distance
{
    float ripple;
    float sphere;
    float min;
    float drop_height;
};

float animation_time;

vec2 random2(vec2 st){
    st = vec2( dot(st,vec2(127.1,311.7)),
              dot(st,vec2(269.5,183.3)) );
    return -1.0 + 2.0 * fract(sin(st));
}

float noise(vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    vec2 u = f*f*(3.0-2.0*f);

    return mix( mix( dot( random2(i + vec2(0.0,0.0) ), f - vec2(0.0,0.0) ),
                     dot( random2(i + vec2(1.0,0.0) ), f - vec2(1.0,0.0) ), u.x),
                mix( dot( random2(i + vec2(0.0,1.0) ), f - vec2(0.0,1.0) ),
                     dot( random2(i + vec2(1.0,1.0) ), f - vec2(1.0,1.0) ), u.x), u.y);
}

vec3 lighting(vec3 direction)
{
    float light = dot(direction,sqrt(vec3(.3,.5,.2)));

    return vec3(light*.5);
}

float smin(float d1, float d2)
{
    // source: https://www.iquilezles.org/www/articles/smin/smin.htm
    const float e = -6.;
    return log(exp(d1*e)+exp(d2*e))/e;
}

float sd_ripple(vec3 p, vec2 offset, float starting_time)
{
    float local_time = animation_time - starting_time;
    float amplitude_max = 0.3;
    float amplitude = amplitude_max * mod(-local_time / 4., 1.);
    if (mod(local_time / 4., 1.) < 0.2)
    amplitude = amplitude_max * mod(time, 1.);
    float frequence = 3.;
    float speed = 6;
    float height_offset = 1.2;
    float time_offset = 0.5;
    float l = dot(p.xz + offset, p.xz + offset);
    float attenuation = 60. / (20. * (l + 1.));

    // if (local_time < 0. || local_time > 4.)
      //  return p.y + height_offset;

    return p.y + height_offset + attenuation * amplitude * sin(frequence * l - local_time * speed - time_offset) / (1. + l);
}

float height_sphere(float starting_time)
{
    float local_time = animation_time - starting_time;

    float dropping_height = 3.;
    float height = -10. * local_time / 2 + dropping_height;
    float intersection_time = (dropping_height + 1) * 2 / 10.;
    if (local_time >= intersection_time)
    {
        height = -1. * (local_time - intersection_time) / 4 - 1;
    }
    return height;
}

float sd_sphere(vec3 p, vec2 offset, float ball_size, float h_ripple, float starting_time)
{
    float falling_time = 15;
    float local_time = animation_time - starting_time;

    if (local_time < 0. || local_time > falling_time)
        return 300.;

    float dropping_height = 3.;
    float height = -10. * local_time / 2 + dropping_height;
    float intersection_time = (dropping_height + 1) * 2 / 10.;
    if (local_time >= intersection_time)
    {
        height = -1. * (local_time - intersection_time) / 4 - 1;
    }
    float drop = length(p + vec3(offset.x, -height, offset.y)) - ball_size;

    return drop;
}

float randx(float seed){
    return fract(sin(dot(vec2(seed), vec2(12.9898, 78.233))) * 43758.5453);
}

float randy(float seed){
    return fract(sin(dot(vec2(seed), vec2(10.9898, 50.233))) * 43758.5453);
}

struct distance dist(vec3 p)
{
    float ripple = 20000;

    float drop = 200;

    float drop_height;
    for (int i = 0; i < 2; i++)
    {
        float y = (randy(i) - 0.5) * 2;
        float x = (randx(i) - 0.5) * 2;
        float starting_time = randx(i + 1) * 10;


        float save = drop;
        drop = min(drop, sd_sphere(p, vec2(x,y), .2, 1.0, starting_time));

        if (drop != save)
            drop_height = height_sphere(starting_time);

        ripple = smin(ripple, sd_ripple(p, vec2(x, y), starting_time + 1.2));
    }

    float closest = min(ripple, drop);

    struct distance res = {ripple, drop, min(ripple, drop), drop_height};
    return res;
}

float diffuse(vec3 n,vec3 l,float p) {
    return pow(dot(n,l) * 0.4 + 0.6, p);
}

vec3 change_colors(struct distance dist, vec3 normal, vec3 p)
{
    vec3 color_res = vec3(0);
    if (dist.min == dist.ripple)
    {
        if (dist.sphere > 0.2)
        {
            vec3 I = normalize(position_.xyz);
            vec3 R = reflect(I, normalize(normal));
            vec3 reflect_color = texture(sky_sampler, R).xyz;
            vec3 refrated = vec3(0.1f, 0.19f, 0.22f) + diffuse(normal, vec3(0.3f,0.5f,0.2f), 80.) * vec3(0.8f, 0.9f,0.6f);
            vec3 water_color = mix(reflect_color, refrated, 0.4);

            if (dist.drop_height + dist.sphere < noise(p.xy))
            {
                color_res = mix(blood_color, water_color, clamp(dist.sphere / 3, 0, 1));
            }
            else
            {
                color_res = water_color;
            }
        }
        else{
            color_res = blood_color;
        }
    }
    else
    {
         color_res = blood_color;
    }
    return color_res;
}

vec3 get_normal(vec3 p)
{
    vec2 e = vec2(1,-1) * .01;

    return normalize(dist(p-e.yxx).min * e.yxx +
		     dist(p-e.xyx).min * e.xyx +
		     dist(p-e.xxy).min * e.xxy +
		     dist(p-e.y).min * e.y);
}

vec4 raymarcher(vec3 p, vec3 ray_direction)
{
    vec4 result = vec4(p,0);
    float max_distance = 500.;
    struct distance dist_obj;
    for(int i = 0; i < 99; i++)
    {
        dist_obj = dist(result.xyz);
        float closest_dist = dist_obj.min;

        result += vec4(ray_direction,1)*closest_dist;

        if (closest_dist < .01 || result.w > max_distance)
            break;
    }
    if (result.w > max_distance)
        col = texture(sky_sampler, position_.xyz).xyz;
    else
        col = change_colors(dist_obj, get_normal(result.xyz), result.xyz);
    return result;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    animation_time = mod(time, 20.);
    vec2 res = resolution.xy;

    vec3 pos = camera_position;

    //raymarcher
    vec3 ray_direction = normalize(position_.xyz);
    vec4 march = raymarcher(pos, ray_direction);

    //lighting
    vec3 normal = get_normal(march.xyz);
    vec3 refraction_direction = refract(ray_direction,normal,.75);
    vec3 refracted = lighting(refraction_direction);
    vec3 ambient = lighting(ray_direction)*.5;

    col += mix(refracted,ambient,0.6);

    fragColor = vec4(col*col,1);
}

void main(void)
{
    mainImage(output_color, pos_.xy);
}
