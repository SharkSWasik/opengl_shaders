#version 460


layout(local_size_x = 16, local_size_y = 16) in;

layout(std430, binding = 2) buffer Ripples{
    float ripples[];
};

layout(std430, binding = 3) buffer Drops{
    float drops[];
};

// Usefull defines
#define PI 3.1415926535897932384626433832795

// Global variables
float animation_time;


struct MyVec3 {
    float x;
    float y;
    float z;
};

uniform float time;
uniform vec3 camera_position;

// Random number generation
float randx(float seed){
    return fract(sin(dot(vec2(seed), vec2(12.9898, 78.233))) * 43758.5453);
}
float randy(float seed){
    return fract(sin(dot(vec2(seed), vec2(10.9898, 50.233))) * 43758.5453);
}
// Smooth minimum : used to smoothly merge waves
// Source: https://www.iquilezles.org/www/articles/smin/smin.htm
float smin(float d1, float d2)
{
    const float e = -6.;
    return log(exp(d1*e)+exp(d2*e))/e;
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

void main(void)
{
    uint i = gl_GlobalInvocationID.x;
    vec3 p = camera_position;

    // Parameters
    int nb_drops = 8;
    float max_y = 12.;
    float max_x = 8.;
    float max_time = 10.;

    float y = (randy(i) - 0.5) * max_y;
    float x = (randx(i) - 0.5) * max_x;
    float starting_time = randx(i + 1) * max_time;

    // Check which drop is the closest
    float save = drops[i];
  //  drops[i] = 2000;
    //ripples[i] = 2000;
    //drops[i] = min(drops[i], sd_drop(p, vec2(x,y), .2, 1.0, starting_time));

    // Merge ripples using smooth minimum
   // ripples[i] = smin(ripples[i], sd_ripple(p, vec2(x, y), starting_time + 2));
}
