#version 460


layout(local_size_x = 16, local_size_y = 16) in;

void main(void)
{
    uint i = gl_GlobalInvocationID.x;
    
}