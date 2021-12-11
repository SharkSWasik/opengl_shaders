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

        Program *make_program();
        void insert_fragment(std::string shader);
        void insert_vertex(std::string shader);
        void insert_compute(std::string shader);

        GLint get_log_shader(GLuint shader);
        GLint get_log_program(GLuint program);
        bool is_ready();
        void use();
        GLuint my_program;

    private:
        std::vector<std::string> shaders;
        std::vector<GLuint> shaders_id;
        bool ready_m;
    };
}
