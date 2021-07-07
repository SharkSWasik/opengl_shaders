#version 460

uniform float time;
uniform vec2 resolution;

out vec4 output_color;
in vec3 pos;


/*
 * Original shader from: https://www.shadertoy.com/view/fsSSzh
 */

#ifdef GL_ES
precision mediump float;
#endif

// shadertoy emulation
#define iTime time
#define iResolution resolution

// --------[ Original ShaderToy begins here ]---------- //
#define PI 3.14159265
#define rot(a) mat2(cos(a),sin(a),-sin(a),cos(a))

 float rand(vec2 st)
{
    return fract(sin(dot(st, vec2(12.9898, 78.233))) * 43758.5453);
}
              

vec2 pmod(vec2 p, float n)
{
float a = atan(p.x, p.y) + PI / n;
float th = 2.0 * PI / n;
a = floor(a / th) * th;
return rot(a)* p;
}

float Cube(vec3 p, float s)
{
p = abs(p);
return length(max(p - vec3(s, s, s), 0.0));
}

float sdCross(vec3 p, float scale)
{
p = abs(p);
float dxy = max(p.x, p.y);
float dyz = max(p.y, p.z);
float dzx = max(p.z, p.x);
return min(dxy, min(dyz, dzx)) - scale;
}


float menger(vec3 p)
{
    float k = 1.;
    p.z = mod(p.z, k) - 0.5 * k;

    //
    float s = .8;
    p = abs(p);
    float d = Cube(p, s);
    //
    float scale = 4.;
    //
    float h = 1.5;
    for (int i = 0; i < 4; i++)
    {

        p = mod(p, h) - h * 0.5;
        s *= scale;
        p = 1.0 - scale * abs(p);
        //
        d = max(d, sdCross(p, .75) / s);

    }

    return d;
}

float sdHexPrism(vec3 p, vec2 h)
{
const vec3 k = vec3(-0.8660254, 0.5, 0.57735);
p = abs(p);
p.xy -= 2.0 * min(dot(k.xy, p.xy), 0.0) * k.xy;
vec2 d = vec2(
    length(p.xy - vec2(clamp(p.x, -k.z * h.x, k.z * h.x), h.x)) * sign(p.y - h.x),
    p.z - h.y);
return min(max(d.x, d.y), 0.0) + length(max(d, 0.0));
}

#define rootT 1.73205080757


float hex(vec3 p,float height) {
    p.yz *=rot(PI / 2.0); 
    p.xy *=rot(PI / 2.0); 

    float r = .015;
    float l = .015;
    p.z -= height;
    p.xy = abs(p.xy);
    float offset = 0.025;

    vec3 pos1 = p;
    vec3 pos2 = p;
    vec2 off1 = vec2(2.0, 2.0 * rootT) * r + vec2(offset, offset);




    pos1.x = mod(pos1.x, off1.x) - off1.x * 0.5;


    pos2.x= mod(pos2.x + 0.5 * off1.x, off1.x) - off1.x * 0.5;
    pos2.y -= off1.y * 0.5;


    float d1 = sdHexPrism(pos1, vec2(r, l));
    float d2 = sdHexPrism(pos2, vec2(r, l));




    d1 = min(d1, d2);

    return d1;
}

float AirPlane(vec3 p){

    return 1.0;

}

float map(vec3 p,inout float flash)
{
     vec3 pos2=p;
     p.z -= iTime * 0.1;
    
    
    /////////////////////////
    vec3 pos = p;
    pos=abs(pos)-0.5;
    pos.z-=0.5;
    pos.z-=0.2;
    pos.z-=0.5;
    pos.z-=0.1;
    
    pos.xy = pmod(pos.xy, 10.0);
    float k=1.2;
    pos=mod(pos,k)-k*0.5;
    float d2 = menger(pos);
  
    if (d2 < 0.001) {
        flash =1.0- abs(sin(p.z+iTime*4.0));
        flash += .15;
        flash = clamp(flash,0.0, 1.0);
    }
    
    ////////////////////////
    //pos2
    float d3=AirPlane(pos2);
    
    
    ////////////////////////
    

    return d2;



}


vec3 gn(vec3 p) {
    vec2 e = vec2(0.0001, 0.0);
    float flash = 0.0;
    return normalize(vec3(
        map(p + e.xyy, flash) - map(p - e.xyy, flash),
        map(p + e.yxy, flash) - map(p - e.yxy, flash),
        map(p + e.yyx, flash) - map(p - e.yyx, flash)
        ));
}

vec3 hsv2rgb2(vec3 c, float k) {
    return smoothstep(0. + k, 1. - k,
        .5 + .5 * cos((vec3(c.x, c.x, c.x) + vec3(3., 2., 1.) / 3.) * radians(360.)));
}


void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    
    vec2 uv = (fragCoord.xy*2.0-iResolution.xy)/min(iResolution.x,iResolution.y);
    float speed=.5;
    float time=mod(iTime,15.0);
    /*float x=rand(vec2(0.123,floor(iTime*speed)))*0.1*2.0-0.1;
    float y=rand(vec2(0.456,floor(iTime*speed)))*0.1*2.0-0.1;
    float z=rand(vec2(0.789,floor(iTime*speed)))*0.1*2.0-0.1;*/
    float addVal=0.2;
    float x=0.0;
    x+=step(5.0,time)*(rand(vec2(0.123,0.123))*2.0-1.0)*addVal;
    x+=step(10.0,time)*(rand(vec2(0.456,0.456))*2.0-1.0)*addVal;
    x+=step(15.0,time)*(rand(vec2(0.789,0.789))*2.0-1.0)*addVal;
    
    float y=0.0;
    y+=step(5.0,time)*(rand(vec2(0.741,0.741))*2.0-1.0)*addVal;
    y+=step(10.0,time)*(rand(vec2(0.852,0.852))*2.0-1.0)*addVal;
    y+=step(15.0,time)*(rand(vec2(0.963,0.963))*2.0-1.0)*addVal;
    
    float z=0.0;
    z+=step(5.0,time)*(rand(vec2(0.321,0.321))*2.0-1.0)*addVal*0.5;
    z+=step(10.0,time)*(rand(vec2(0.654,0.654))*2.0-1.0)*addVal*0.5;
    z+=step(15.0,time)*(rand(vec2(0.987,0.987))*2.0-1.0)*addVal*0.5;
    
    float tx=rand(vec2(0.963,floor(iTime*speed)))*0.1*2.0-0.1;
    float ty=rand(vec2(0.852,floor(iTime*speed)))*0.1*2.0-0.1;
    float tz=rand(vec2(0.741,floor(iTime*speed)))*0.1*2.0-0.1;
    
    //vec3 ta=vec3(tx,ty,tz);
    vec3 ta=vec3(0.0);
    vec3 ro=vec3(0.0,0.0,0.1);
    ro.y-=0.05;
    
    vec3 cDir=normalize(ta-ro);
    vec3 cSide=cross(cDir,vec3(0.,1.,0.));
    vec3 cUp=cross(cDir,cSide);
    float depth=1.;
    
    vec3 rd=normalize(uv.x*cSide+uv.y*cUp+depth*cDir);
    
    vec3 col = vec3(0., 0., 0.);

    float d, t = 0., acc, flash = 0.0;
    float adjD = .75;

    for (int i = 0; i < 80; i++)
    {
        d = map(ro + rd * t, flash);
        if (d < 0.001 || t>1000.0)break;
        t += d * adjD;
        acc += exp(-1.0 * d);


    }

    vec3 refRo = ro + rd * t;
    vec3 n = gn(refRo);
   // rd = refract(rd, n,0.5);
    rd = reflect(rd, n);
    ro = refRo;
    t = 0.1;
    float acc2, flash2 = 0.0;
    for (int i = 0; i < 20;i++){
        d = map(ro + rd * t, flash2);
        if (d < 0.0001 || t>1000.0)break;
        t += d * adjD;
        acc2 += exp(-1.0 * d);
    }

    float H = mod(iTime*0.1, 1.0);
    col = acc * hsv2rgb2(vec3(H,1.0,1.0),2.2) * 0.08 * flash;
    col += acc2 *vec3(1.,1.,1.) * 0.01 * flash2 ;
    
    fragColor = vec4(col,1.0);
}
// --------[ Original ShaderToy ends here ]---------- //

void main(void)
{
    mainImage(output_color, pos.xy);
}
