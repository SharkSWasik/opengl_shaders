#version 460

uniform float time;
uniform vec2 resolution;
uniform sampler2D sky_sampler;

out vec4 output_color;
in vec3 pos_;
//in vec2 text_coord;

vec3 col;

// shadertoy emulation
#define iTime time
#define iResolution resolution

#define AA 1

//Background gradient
vec3 background(vec3 d)
{
    float light = dot(d,sqrt(vec3(.3,.5,.2)));

    return vec3(max(light*.5+.5,.0));
}
//Smooth minimum (based off IQ's work)
float smin(float d1, float d2)
{
    const float e = -6.;
    return log(exp(d1*e)+exp(d2*e))/e;
}

float sd_ripple(vec3 p)
{
    float amplitude = 0.2;
    float y_translation = 0.8;
    float frequence = 3;
    float speed = 2;
    float time_offset = 1.5;

    float l = dot(p.xz,p.xz);
    return p.y + y_translation + amplitude * sin(l * frequence - iTime * speed - time_offset) / (1. + l);
}

float sd_sphere(vec3 p, float s)
{
    float h1 = iTime * 2;
    float h2 = iTime * 2;
    //float h2 = cos(iTime+.1);
    float drop = length(p - vec3(0,1,0) + vec3(0,0.3,0) * h1) - s;
    int i = 3;
    while (i < 12)
    {
        drop = min(length(p - vec3(0,i,0) + vec3(0,0.3,0) * h2) - s, drop);
        //drop = smin(drop,length(p+vec3(.1,.8,0)*h2)-.2);
        i += 2;
    }
    return drop;
}

//Ripple and drop distance function
float dist(vec3 p)
{
    float ripple = sd_ripple(p);

    float drop = sd_sphere(p, .2);

    if (ripple > drop)
        col = vec3(0,0,0);
    else if (ripple < 0.2)
        col = vec3(1,0,0);
    //else
      //  col = texture(sky_sampler, text_coord).xyz;
    return min(ripple,drop);
    //return min(drop, drop);
}
//Typical SDF normal function
vec3 normal(vec3 p)
{
    vec2 e = vec2(1,-1)*.01;

    return normalize(dist(p-e.yxx)*e.yxx+dist(p-e.xyx)*e.xyx+
    dist(p-e.xxy)*e.xxy+dist(p-e.y)*e.y);
}
//Basic raymarcher
vec4 march(vec3 p, vec3 d)
{
    vec4 m = vec4(p,0);
    for(int i = 0; i<99; i++)
    {
        float s = dist(m.xyz);
        m += vec4(d,1)*s;

        if (s<.01 || m.w>20.) break;
    }
    if (m.w > 20)
        col = vec3(1,1,1);
    return m;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 res = iResolution.xy;
    
    //vec3 pos = vec3(.05*cos(iTime),.1*sin(iTime),-4);
    vec3 pos = vec3(0,0,-4);

    //Sample
    for(float x = 0.;x<AA;x++)
    for(float y = 0.;y<AA;y++)
    {
        vec3 ray = normalize(vec3(fragCoord-res/2.+vec2(x,y)/AA,res.y));
        vec4 mar = march(pos,ray);
        vec3 nor = normal(mar.xyz);
        vec3 ref = refract(ray,nor,.75);
        float r = smoothstep(.8,1.,dot(reflect(ray,nor),sqrt(vec3(.3,.5,.2))));
        vec3 wat = background(ref)+.1*r*sqrt(abs(dot(ray,nor)));
        vec3 bac = background(ray)*.5+.5;

        float fade = pow(min(mar.w/20.,1.),.3);
        col += mix(wat,bac,fade);
    }
    col /= AA*AA;
    fragColor = vec4(col*col,1);

}

void main(void)
{
    mainImage(output_color, pos_.xy);
}

