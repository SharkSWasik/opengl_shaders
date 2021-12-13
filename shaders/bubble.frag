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
in vec3 drops_;
in vec2 ripples_;

// Linkage out of the shader to next stage
out vec4 output_color;

// Usefull defines
#define PI 3.1415926535897932384626433832795

// Global variables
vec3 col;
vec3 blood_color = vec3(0.65,0.06,0.12);
float animation_time;

// Represent the output of the raymarcher and distance to closest objects
struct distance
{
    float ripple;
    float drop;
    float min;
    float drop_height;
};


// Random number generation
float randx(float seed){
    return fract(sin(dot(vec2(seed), vec2(12.9898, 78.233))) * 43758.5453);
}
float randy(float seed){
    return fract(sin(dot(vec2(seed), vec2(10.9898, 50.233))) * 43758.5453);
}
vec2 random2(vec2 st){
    st = vec2( dot(st,vec2(127.1,311.7)),
              dot(st,vec2(269.5,183.3)) );
    return -1.0 + 2.0 * fract(sin(st));
}

// Noise function used for the blood spread
// Modified from idea: https://www.shadertoy.com/view/XdXGW8
float noise(vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    vec2 u = f*f*(3.0-2.0*f);

    return mix(mix(dot(random2(i + vec2(0.0,0.0) ), f - vec2(0.0,0.0)),
                   dot(random2(i + vec2(1.0,0.0) ), f - vec2(1.0,0.0)), u.x),
               mix(dot(random2(i + vec2(0.0,1.0) ), f - vec2(0.0,1.0)),
                   dot(random2(i + vec2(1.0,1.0) ), f - vec2(1.0,1.0)), u.x),
               u.y);
}

// Smooth minimum : used to smoothly merge waves
// Source: https://www.iquilezles.org/www/articles/smin/smin.htm
float smin(float d1, float d2)
{
    const float e = -6.;
    return log(exp(d1*e)+exp(d2*e))/e;
}

// Function generating the scene by returning the distance to the closest object
distance dist(vec3 p)
{
    // Default values really far as we are searching for the closest
    float ripple = ripples_[0];
    float drop = drops_[0];

    // Parameters
    int nb_drops = drops_.length();

    // Drop nb_drops randomly in space (max_y/x) and time (max_time)
    for (int i = 1; i < nb_drops; i++)
    {
        // Check which drop is the closest
        drop = min(drop, drops_[i]);

        // Merge ripples using smooth minimum
        ripple = smin(ripple, ripples_[i]);
    }

    // Get closest object that will be drawn
    float closest = min(ripple, drop);
    float starting_time = randx(8) * 10.;
    distance res = {ripple, drop, min(ripple, drop), starting_time};
    return res;
}

// Diffuse function for lighting
float diffuse(vec3 n,vec3 l,float p) {
    return pow(dot(n,l) * 0.4 + 0.6, p);
}

// Lighting function used for ambient and refraction lights
vec3 lighting(vec3 direction)
{
    float light = dot(direction,sqrt(vec3(.3,.5,.2)));

    return vec3(light*.5);
}

// Function used to change colors of the scenes depending on the closest object
vec3 change_colors(distance dist, vec3 normal, vec3 p)
{
    // Default color
    vec3 color_res = vec3(0);

    // Ripple and blood spreading colors
    if (dist.min == dist.ripple)
    {
        if (dist.drop > noise(p.xy))
        {
            // Calculate water color depending on the reflect the diffusion
            vec3 I = normalize(position_.xyz + time * vec3(1,0,0));
            vec3 R = reflect(I, normalize(normal));
            vec3 reflect_color = texture(sky_sampler, R).xyz;
            vec3 refrated = vec3(0.1f, 0.19f, 0.22f) + diffuse(normal, vec3(0.3f,0.5f,0.2f), 80.) * vec3(0.8f, 0.9f,0.6f);
            vec3 water_color = mix(reflect_color, refrated, 0.4);

            // Spreading blood
            if (dist.drop_height + dist.drop < noise(p.xy))
            {
                color_res = mix(blood_color, water_color, clamp(clamp(dist.drop / 3, 0, 1) + 0.2 , 0, 1));
            }
            // Simple ripple
            else
            {
                color_res = water_color;
            }
        }
        // intersection between Drop and water
        else{
            color_res = blood_color;
        }
    }
    // Drop Color
    else
    {
         color_res = blood_color;
    }
    return color_res;
}

// Classical function to get normal in the scene using our dist function
vec3 get_normal(vec3 p)
{
    vec2 e = vec2(1,-1) * .01;

    return normalize(dist(p-e.yxx).min * e.yxx +
		     dist(p-e.xyx).min * e.xyx +
		     dist(p-e.xxy).min * e.xxy +
		     dist(p-e.y).min * e.y);
}

// Ray marcher searching for the closest object to draw depending on position and the ray direction
vec4 raymarcher(vec3 p, vec3 ray_direction)
{
    vec4 result = vec4(p, 0);
    distance dist_obj;

    // The maximum distance we can see
    float max_distance = 500.;

    // Number of steps possible to find the closest object
    for(int i = 0; i < 99; i++)
    {
        // Call our distance function to find the distance to closest object
        dist_obj = dist(result.xyz);
        float closest_dist = dist_obj.min;

        // Moving forward in the wanted direction
        result += vec4(ray_direction,1) * closest_dist;

        // check if we found an object close enought or if we reached the max distance
        if (closest_dist < .01 || result.w > max_distance)
            break;
    }
    // If there were no object in our ray, draw the texture of the sky box
    if (result.w > max_distance)
        col = texture(sky_sampler, position_.xyz + time * vec3(1,0,0)).xyz;
    // We found an object, lets find its color
    else
        col = change_colors(dist_obj, get_normal(result.xyz), result.xyz);
    return result;
}

// Our main function calling camera, raymarcher, lighting, ...
void main_image(out vec4 out_color, in vec2 fragCoord)
{
    // Time of our periodic animation
    animation_time = mod(time, 15.);

    // Resolution and camera position given by uniform
    vec2 res = resolution.xy;
    vec3 pos = camera_position;

    // Raymarcher
    vec3 ray_direction = normalize(position_.xyz);
    vec4 march = raymarcher(pos, ray_direction);

    // Global lighting
    vec3 normal = get_normal(march.xyz);
    vec3 refraction_direction = refract(ray_direction,normal,.75);
    vec3 refracted = lighting(refraction_direction);
    vec3 ambient = lighting(ray_direction)*.5;

    col += mix(refracted,ambient,0.6);

    out_color = vec4(col*col,1);
}

void main(void)
{
    main_image(output_color, pos_.xy);
    output_color = vec4(drops_[1],drops_[2],ripples_[1],ripples_[0]);
    //output_color = vec4(position_[1],position_[2], pos_[1],pos_[0]);
}
