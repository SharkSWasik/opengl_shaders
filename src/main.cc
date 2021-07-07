#include "GL/glew.h"
#include "GL/freeglut.h"
#include "GL/glu.h"

#include "program.hh"
#include "util.hh"
#include <GL/freeglut_std.h>
#include <cstdlib>
#include <math.h>

GLuint vboId;
GLuint program_id;

#define TEST_OPENGL_ERROR()                                                             \
  do {									\
    GLenum err = glGetError();					                        \
    if (err != GL_NO_ERROR) std::cerr << "OpenGL ERROR!" << __LINE__ << std::endl;      \
  } while(0)

void init_VAO()
{
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
}

// init vertex buffer;
void init_VBO()
{
    //6 vertices for the entire screen (2 triangles)
    GLfloat vertices[] = {
        -1, -1, 0, -1, 1, 0, 1, 1, 0,
        1, 1, 0, 1, -1, 0,-1, -1, 0
    };


    // reservation of buffer
    glGenBuffers(1, &vboId);TEST_OPENGL_ERROR();

    //activation of buffer, vbo type
    glBindBuffer(GL_ARRAY_BUFFER, vboId);TEST_OPENGL_ERROR();

    //allocation
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);TEST_OPENGL_ERROR();

    //how to read buffer
    glVertexAttribPointer(
            0,                   //location
            3,                  // size one vertex
            GL_FLOAT,           // type
            GL_FALSE,           // normalize
            0,                  // stride
            (void*)0            // array buffer offset
            );TEST_OPENGL_ERROR();

    glEnableVertexAttribArray(0);TEST_OPENGL_ERROR();
}

void init_GL()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthRange(0.0, 0.1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glClearColor(0, 0, 0, 0);
}


void idle()
{
    GLint time_location = glGetUniformLocation(program_id, "time");
    float time = glutGet(GLUT_ELAPSED_TIME) / 1000.;
    glUniform1f(time_location, time);
    glutPostRedisplay();
}

void display()
{

    // clear the color buffer before each drawing
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLint resoltion_location = glGetUniformLocation(program_id, "resolution");
    TEST_OPENGL_ERROR();

    glUniform2f(resoltion_location, 1, 1);TEST_OPENGL_ERROR();

    glBindBuffer(GL_ARRAY_BUFFER, vboId);TEST_OPENGL_ERROR();

    glDrawArrays(GL_TRIANGLES, 0, 6);TEST_OPENGL_ERROR();
    // swap the buffers and hence show the buffers
    // content to the screen
    glutSwapBuffers();
}

void init_glut(int &argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitContextVersion(4, 6);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(252, 252);
    glutInitWindowPosition(10, 10);
    glutCreateWindow("Test OpenGL - POGL");
    glutDisplayFunc(display);
    glutIdleFunc(idle);
}

//glew is opengl extension wrangler
bool init_glew() {
  if (glewInit()) {
    std::cerr << " Error while initializing glew";
    return false;
  }
  return true;
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cerr << "missing vertexShader and/or fragmentShader path" << std::endl;
        exit(EXIT_FAILURE);
    }

    init_glut(argc, argv);

    if (!init_glew())
    {
        std::cerr << "failing init glew" << std::endl;
        exit(EXIT_FAILURE);
    }

    init_GL();
    init_VAO();
    init_VBO();
    
    auto vertex = load_file(argv[1]);
    auto fragment = load_file(argv[2]);
    auto prog = mygl::Program::make_program(vertex, fragment);
    program_id = prog->my_program;

    if (!prog->is_ready())
    {
        std::cerr << "prog is not ready" << std::endl;
        exit(EXIT_FAILURE);
    }

    prog->use();
    glutMainLoop();
    delete prog;
    return 0;
}
