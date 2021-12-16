#version 460

// Global Shader variables
uniform sampler2D raymarch;
uniform samplerCube sky_sampler;

// Linkage into shader from previous stage
in vec3 pos_;
in vec4 position_;
in vec2 uv_;

// Linkage out of the shader to next stage
out vec4 output_color;

void main()
{
   // uvec4 pixel = uvec4(1.0,0.0,1.0,0.0);
    //
    output_color = texture(raymarch, uv_) * vec4(1,1,1, 1.0);  
    //output_color = texture(sky_sampler, pos_);
    // imageStore(raymarch, ivec2(pos_.xy), pixel);
   // output_color = imageLoad(raymarch, ivec2(uv_));
}
