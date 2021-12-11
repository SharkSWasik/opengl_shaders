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

    void Program::insert_fragment(std::string shader)
    {
        shaders_id.push_back(glCreateShader(GL_FRAGMENT_SHADER));
        shaders.push_back(shader);
    }

    void Program::insert_vertex(std::string shader)
    {
        shaders_id.push_back(glCreateShader(GL_VERTEX_SHADER));
        shaders.push_back(shader);
    }

    void Program::insert_compute(std::string shader)
    {
        shaders_id.push_back(glCreateShader(GL_COMPUTE_SHADER));
        shaders.push_back(shader);
    }

    Program *Program::make_program()
    {
        ready_m = true;

        //Send the vertex shader source code to GL
        for (size_t i = 0; i < shaders.size(); i++)
        {
            auto shader = shaders[i].c_str();
            glShaderSource(shaders_id[i], 1, &shader, NULL);
        }

        //Compile the vertex shade
        for (size_t i = 0; i < shaders.size(); i++)
            glCompileShader(shaders_id[i]);

        for (size_t i = 0; i < shaders.size(); i++)
        {
            if (!get_log_shader(shaders_id[i]))
            {
                //Don't leak the shader
                glDeleteShader(shaders_id[i]);
                ready_m = false;
            }
        }

        //attach shaders to our program
        for (size_t i = 0; i < shaders.size(); i++)
            glAttachShader(my_program, shaders_id[i]);

        glLinkProgram(my_program);

        if (!get_log_program(my_program))
        {
            glDeleteProgram(my_program);
            for (size_t i = 0; i < shaders.size(); i++)
                glDeleteProgram(shaders_id[i]);
            ready_m = false;
        }

        // Always detach shaders after a successful link.
        for (size_t i = 0; i < shaders.size(); i++)
            glDetachShader(my_program, shaders_id[i]);

        return this;
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
    GLint Program::get_log_program(GLuint program)
    {
        GLint isLinked = 0;
        glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);

        if (isLinked == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

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
