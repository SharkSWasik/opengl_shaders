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

float hash( float n )
{
    return fract( sin( n ) * 45843.349 );

}

float noise( in vec3 x )
{
    
    vec3 p = floor( x );
    vec3 k = fract( x );
    
    k *= k * k * ( 3.0 - 2.0 * k );
    
    float n = p.x + p.y * 57.0 + p.z * 113.0; 
    
    float a = hash( n );
    float b = hash( n + 1.0 );
    float c = hash( n + 57.0 );
    float d = hash( n + 58.0 );
    
    float e = hash( n + 113.0 );
    float f = hash( n + 114.0 );
    float g = hash( n + 170.0 );
    float h = hash( n + 171.0 );
    
    float res = mix( mix( mix ( a, b, k.x ), mix( c, d, k.x ), k.y ),
                     mix( mix ( e, f, k.x ), mix( g, h, k.x ), k.y ),
                     k.z
    				 );
    
    return res;
    
}

#define NUM_OCTAVES 8

float fbm ( in vec3 _st) {
    float v = 0.1;
    float a = 0.5;
    float sum = 0;
    vec3 shift = vec3(100.0);
    // Rotate to reduce axial bias
    for (int i = 0; i < NUM_OCTAVES; ++i) {
        v += a * noise(_st);
        _st = _st * 2;
        _st += time / 100;
        sum += a;
        a *= 0.5;
    }
    return v;
}

float get_layers(vec3 dir)
{
    vec3 fbm_1;
    fbm_1.x = fbm(dir);
    fbm_1.y = fbm(dir);
    fbm_1.z = fbm(dir);

    vec3 fbm_2;

    fbm_2.x = fbm(dir + fbm_1 + vec3(30, 4, 7));
    fbm_2.y = fbm(dir + fbm_1 + vec3(5,30,2));
    fbm_2.z = fbm(dir + fbm_1 + vec3(15, 3, 3));

    float f = fbm(dir + fbm_2);
    return (f*f*f+0.6*f*f+0.5*f);
}

vec3 raymarcher_sky(vec3 p, vec3 ray_direction)
{
    // The maximum distance we can see
    float max_distance = 10.;
    vec4 direction = vec4(p, 0);
    vec4 skycolor = vec4(0,0,0,0);

    // Number of steps possible to find the closest object
    vec4 sum = vec4(0,0,0,0);

    col = imageLoad(sun_texture, ivec2(sun_pos.x, int(100 + ray_direction.y * 800) * 1.5)).xyz;

    for (int i = 0; i < 30; i++)
    {

        // Moving forward in the wanted direction
        direction += vec4(ray_direction, 1) * 4;


        float density =  get_layers(direction.xyz);
        density = 1 - (direction.y * density);
        if (density > 0.1)
        {
            skycolor.xyz = vec3(mix(vec3(1.0,0.95,0.8), vec3(0.45,0.5,0.45), density));
            sum += vec4(skycolor.xyz * density, density);
        }

        if (sum.w > 0.9 || direction.w > max_distance)
        {
            sum.w = 0.9;
            break;
        }

    }

    return clamp(col * (1 - sum.xyz) + sum.xyz, 0.0, 1.0);
}

// Our main function calling camera, raymarcher, lighting, ...
vec3 sky(vec3 position_)
{
    vec2 res = vec2(900,900);
    col = vec3(0);
    sun_pos = vec3(450 + 440 * cos(time / 4), 200 + 100 * cos(time), 1);


    float x = position_.x * 900;
    float y = position_.y * 900;

    if (position.y < 450)
        y = y + (2 * gl_GlobalInvocationID.y) - 450;
    else
        y = gl_GlobalInvocationID.y - 2 * y;


    vec4 position = vec4(x, y, 900, 1);
    vec3 ray_direction = normalize(position.xyz);

    return raymarcher_sky(camera_position.xyz, ray_direction.xyz);
}

// Global variables

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

// Ripple distance function to create waves depending on time/position
float sd_ripple(vec3 p, vec2 offset, float starting_time)
{
    // Time since the start of the ripple
    float local_time = animation_time - starting_time;

    // Get the amplitude of the ripple changing over time
    float amplitude_max = 0.3;
    // Decrease slowly to amplitude 0
    float amplitude = amplitude_max * (-local_time/4. + 1.125);
    // Increase quickly to amplitude_max to start the ripple
    if (local_time < 0.5)
        amplitude = amplitude_max * sin(PI * local_time);

    // Variables of the ripple
    float frequence = 3.;
    float speed = 3.;
    float height_offset = 0.7;
    float time_offset = 0.5;
    float l = dot(p.xz + offset, p.xz + offset);

    // Attenuation of the wave amplitude as it goes away from its center
    float attenuation = 60. / (20. * (l + 1.));

    // Check if animation should start or end depending on the time
    if (local_time < 0. || local_time > 4.5)
        return p.y + height_offset;

    // Merge all those variable into a ripple function using sin()
    return p.y + height_offset +
           attenuation * amplitude *
           sin(frequence * l - animation_time * speed - time_offset) / (1. + l);
}

// Round cone distance function to generate a drop shape
// Modified from idea: https://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
float sd_round_cone(vec3 p, float r1, float r2, float h, vec3 offset)
{
    vec2 q = vec2(length(p.xz + offset.xz), p.y + offset.y);

    float b = (r1 - r2) / h;
    float a = sqrt(1.0 - b * b);
    float k = dot(q, vec2(-b, a));

    if(k < 0.0)
        return length(q) - r1;
    if(k > a*h)
        return length(q - vec2(0.0,h)) - r2;

    return dot(q, vec2(a, b)) - r1;
}

// Give the height of the drop depending on time and their starting time
float get_drop_height(float starting_time)
{
    float local_time = animation_time - starting_time;
    float dropping_height = 10.;

    // Simplified version of motion equation
    float height = -10. * local_time / 2 + dropping_height;
    // Determine the time where the drop will pass under the water
    float intersection_time = (dropping_height + 1) * 2 / 10.;
    // When under water, reduce the speed of the drop
    if (local_time >= intersection_time)
        height = -1. * (local_time - intersection_time) / 4 - 1;

    return height;
}

// Drop distance function to generate drops depending on time and position
float sd_drop(vec3 p, vec2 offset, float ball_size, float h_ripple, float starting_time)
{
    float falling_time = 15;
    float local_time = animation_time - starting_time;

    // Check if it's the time to display the drop
    if (local_time < 0. || local_time > falling_time)
        return 300.;

    // Get the altitude of the drop
    float height = get_drop_height(starting_time);

    // use round_cone distance function to simulate a drop
    float drop = sd_round_cone(p, 0.2, .02, 0.4, vec3(offset.x, -height, offset.y));
    // You can also use a sphere instead of a drop shape
    // drop = length(p + vec3(offset.x, -height, offset.y)) - ball_size;

    return drop;
}

// Function generating the scene by returning the distance to the closest object
distance dist(vec3 p)
{
    // Default values really far as we are searching for the closest
    float ripple = 2000.;
    float drop = 2000.;
    float drop_height;

    // Parameters
    int nb_drops = 2;
    float max_y = 12.;
    float max_x = 8.;
    float max_time = 10.;

    // Drop nb_drops randomly in space (max_y/x) and time (max_time)
    for (int i = 0; i < nb_drops; i++)
    {
        float y = (randy(i) - 0.5) * max_y;
        float x = (randx(i) - 0.5) * max_x;
        float starting_time = randx(i + 1) * max_time;

        // Check which drop is the closest
        float save = drop;
        drop = min(drop, sd_drop(p, vec2(x,y), .2, 1.0, starting_time));
        if (drop != save)
            drop_height = get_drop_height(starting_time);

        // Merge ripples using smooth minimum
        ripple = smin(ripple, sd_ripple(p, vec2(x, y), starting_time + 2));
    }

    // Get closest object that will be drawn
    float closest = min(ripple, drop);
    distance res = {ripple, drop, min(ripple, drop), drop_height};
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
            vec3 I = normalize(position_v2.xyz);
            vec3 R = reflect(I, normalize(normal));
            vec3 reflect_color = sky(R).xyz;

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
    float max_distance = 400.;

    // Number of steps possible to find the closest object
    for(int i = 0; i < 99; i++)
    {
        // Call our distance function to find the distance to closest object
        dist_obj = dist(result.xyz);
        float closest_dist = dist_obj.min;

        // Moving forward in the wanted direction
        result += vec4(ray_direction,1)*closest_dist;

        // check if we found an object close enought or if we reached the max distance
        if (closest_dist < .01 || result.w > max_distance)
            break;
    }
    // If there were no object in our ray, draw the texture of the sky box
    if (result.w > max_distance)
    {
        col = sky(position_v2.xyz);//imageLoad(sky_sampler, ivec2(gl_GlobalInvocationID.xy)).xyz;
    }
    // We found an object, lets find its color
    else
        col = change_colors(dist_obj, get_normal(result.xyz), result.xyz);
    return result;
}

mat4 rotationX( in float angle ) {
	return mat4(	1.0,		0,			0,			0,
			 		0, 	cos(angle),	-sin(angle),		0,
					0, 	sin(angle),	 cos(angle),		0,
					0, 			0,			  0, 		1);
}

mat4 rotationY( in float angle ) {
	return mat4(	cos(angle),		0,		sin(angle),	0,
			 				0,		1.0,			 0,	0,
					-sin(angle),	0,		cos(angle),	0,
							0, 		0,				0,	1);
}


mat4 rotationZ( in float angle ) {
	return mat4(	cos(angle),		-sin(angle),	0,	0,
			 		sin(angle),		cos(angle),		0,	0,
							0,				0,		1,	0,
							0,				0,		0,	1);
}

// Our main function calling camera, raymarcher, lighting, ...
void main()
{
    vec2 res = vec2(900,900);
    col = vec3(0);

    //Sample
    float x = gl_GlobalInvocationID.x - res.x / 2;
    float y = res.y / 2 - gl_GlobalInvocationID.y;
    vec4 position_ = vec4(x, y, 900, 1);
    // Time of our periodic animation
    animation_time = mod(time, 15.);

    // Resolution and camera position given by uniform
    vec3 pos = camera_position;
    // Raymarcher
    position_v2 = normalize(position_);
    vec3 ray_direction = position_v2.xyz;

    vec4 march = raymarcher(pos, ray_direction);

    // Global lighting
    vec3 normal = get_normal(march.xyz);
    vec3 refraction_direction = refract(ray_direction, normal,.75);
    vec3 refracted = lighting(refraction_direction);
    vec3 ambient = lighting(ray_direction)*.5;
    col += mix(refracted, ambient,0.6);

    imageStore(raymarch, ivec2(gl_GlobalInvocationID.xy), vec4(col,1));
}
