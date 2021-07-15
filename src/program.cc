#include "GL/glew.h"

#include "program.hh"
#include <cstdlib>

//create an opengl program with shaders
namespace mygl
{
    Program::Program()
    {
        my_program = glCreateProgram();
    }

    Program::~Program()
    {
    }

    Program *Program::make_program(std::string &vertex_shader_src,
                                   std::string &fragment_shaders)
    {
        Program *program = new Program();
        program->ready_m = true;

        //Create empty shaders
        auto my_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        auto my_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

        auto char_vertex_src = vertex_shader_src.c_str();
        auto char_fragment_src = fragment_shaders.c_str();

        //Send the vertex shader source code to GL
        glShaderSource(my_vertex_shader, 1, &char_vertex_src, NULL);
        glShaderSource(my_fragment_shader, 1, &char_fragment_src, NULL);

        //Compile the vertex shader
        glCompileShader(my_vertex_shader);
        glCompileShader(my_fragment_shader);

        if (!program->get_log_shader(my_vertex_shader))
        {
            //Don't leak the shader
            glDeleteShader(my_vertex_shader);
            program->ready_m = false;
        }

        if (!program->get_log_shader(my_fragment_shader))
        {
            glDeleteShader(my_fragment_shader);
            program->ready_m = false;
        }

        //attach shaders to our program
        glAttachShader(program->my_program, my_vertex_shader);
        glAttachShader(program->my_program, my_fragment_shader);

        glLinkProgram(program->my_program);

        if (!program->get_log_program())
        {
            glDeleteProgram(program->my_program);
            glDeleteProgram(my_vertex_shader);
            glDeleteProgram(my_fragment_shader);
            program->ready_m = false;
        }

        // Always detach shaders after a successful link.
        glDetachShader(program->my_program, my_vertex_shader);
        glDetachShader(program->my_program, my_fragment_shader);
        return program;
    }

    //log report for shader
    GLint Program::get_log_shader(GLuint shader)
    {
        GLint success = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (success == GL_FALSE)
        {
            GLint logSize = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);

            std::vector<char> log(logSize);
            glGetShaderInfoLog(shader, logSize, NULL, &log[0]);
            std::cout << "error compiling vertex shader " << log.data() << std::endl;
        }
        return success;
    }

    //log report for program
    GLint Program::get_log_program()
    {
        GLint isLinked = 0;
        glGetProgramiv(my_program, GL_LINK_STATUS, (int *)&isLinked);

        if (isLinked == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetProgramiv(my_program, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(my_program, maxLength, &maxLength, &infoLog[0]);

            std::cout << "error while linking program" << std::endl;
        }
        return isLinked;
    }

    bool Program::is_ready()
    {
        return ready_m;
    }

    void Program::use()
    {
        glUseProgram(my_program);
    }
}
