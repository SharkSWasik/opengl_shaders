#version 460


layout(local_size_x = 20, local_size_y = 20) in;

layout(rgba32f, binding=0) uniform image2D raymarch;
layout(rgba32f, binding=2) uniform image2D sun_texture;
layout(rgba32f, binding=1) uniform imageCube sky_sampler;

// Global Shader variables
uniform float time;
uniform vec3 camera_position;

// Linkage into shader from previous stage
vec4 position_v2;
vec3 position;

// Usefull defines
#define PI 3.1415926535897932384626433832795

// Global variables
vec3 sun_pos;
vec3 col;
vec3 blood_color = vec3(0.65,0.06,0.12);
float animation_time;

float random (in vec2 _st) {
    return fract(sin(dot(_st.xy,
                         vec2(12.9898,78.233)))*
        43758.5453123);
}

// Based on Morgan McGuire @morgan3d
// https://www.shadertoy.com/view/4dS3Wd
float noise (in vec2 _st) {
    vec2 i = floor(_st);
    vec2 f = fract(_st);

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    vec2 u = f * f * (3.0 - 2.0 * f);

    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}


float map(float value, float min1, float max1, float min2, float max2) {
  return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}
#define NUM_OCTAVES 8

float fbm ( in vec2 _st) {
    float v = 0.1;
    float a = 0.5;
    vec2 shift = vec2(100.0);
    // Rotate to reduce axial bias
    for (int i = 0; i < NUM_OCTAVES; ++i) {
        v += a * noise(_st);
        _st = _st * 2.0;
        a *= 0.5;
    }
    if (v <= 0.6)
        return 0.0;
    return map(v, 0.6, 1.0, 0.0, 1.0);
}

mat2 m2 = mat2(1.6,-1.2,1.2,1.6);


float get_layers(vec2 dir)
{
    dir.y -= 10;
    dir.x += time / 1000;
    vec2 fbm_1;
    fbm_1.x = fbm(dir.xy);
    fbm_1.y = fbm(dir.xy);

    vec2 fbm_2;

    fbm_2.x = fbm(dir.xy + fbm_1 * time);
    fbm_2.y = fbm(dir.xy + fbm_1 * time / 5);

    float f = fbm(dir.xy + fbm_2);
    return (f*f*f+0.6*f*f+0.5*f);
}

vec3 raymarcher(vec3 p, vec3 ray_direction)
{
    // The maximum distance we can see
    float max_distance = 200.;
    vec4 direction = vec4(p, 0);
    float closest_dist;
    vec4 skycolor = vec4(0,0,0,0);


    // Number of steps possible to find the closest object
    vec4 sum = vec4(0,0,0,0);
    col = imageLoad(sun_texture, ivec2(sun_pos.x, gl_GlobalInvocationID.y * 2)).xyz;
    for (int i = 0; i < 70; i++)
    {

        // Moving forward in the wanted direction
        direction += vec4(ray_direction, 1) * 60;
        float sundot = clamp(dot(direction.xyz,sun_pos),0.0,1.0);

        float x = gl_GlobalInvocationID.x;
        float y = gl_GlobalInvocationID.y;
        vec4 position_ = vec4(x, y, 900, 1) + vec4(0, time * 0.2, time * 0.55, 0);
        float density = get_layers(position_.xy / vec2(900,900));

        float closest_dist = density;

        if (density > 0.1)
        {
            skycolor.xyz = vec3(mix(vec3(1.0,0.95,0.8), vec3(0.5,0.3,0.35), density));
            skycolor.xyz *= col * 0.8;
            sum += vec4(skycolor.xyz * density, density);
        }

        if (sum.w > 0.9 || direction.w > max_distance)
        {
            sum.w = 0.9;
            break;
        }


    }

    //vec3(0.572,0.733,1.000);
    //}
    return clamp(col * (1 - sum.xyz) + sum.xyz, 0.0, 1.0);
}

// Our main function calling camera, raymarcher, lighting, ...
void main()
{
    vec2 res = vec2(900,900);
    col = vec3(0);
    sun_pos = vec3(450 + 440 * cos(time / 4), 200 + 100 * cos(time), 1);


    //Sample
    float x = gl_GlobalInvocationID.x;
    float y = gl_GlobalInvocationID.y;
    vec4 position_ = vec4(x, y, 900, 1);
    vec3 ray_direction = normalize(position_.xyz);

    vec3 color = vec3(0,0,0);
    if (position_.y < 450)
        color = raymarcher(camera_position.xyz, ray_direction.xyz);
    else
        color = vec3(1.0,0.3,0.3);
    //    float color = get_layers(vec2(position_.x / res.x, position_.y / res.y));*/

    col = vec3(0.572,0.733,0.8);

    imageStore(raymarch, ivec2(gl_GlobalInvocationID.xy), vec4(color,1));

}
