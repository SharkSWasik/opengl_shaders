#include <string>
#include <iostream>
#include <vector>

#include "GL/glew.h"

namespace mygl
{
    class Program
    {
    public:
        Program();
        ~Program();
        static Program *make_program(std::string &vertex_shader_src,
                                     std::string &fragment_shaders);
        GLint get_log_shader(GLuint shader);
        GLint get_log_program();
        bool is_ready();
        void use();

    private:
        GLuint my_program;
        bool ready_m;
    };
}