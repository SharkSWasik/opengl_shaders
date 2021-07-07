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
// Load data in VBO and return the vbo's id
GLuint loadDataInBuffers()
{
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    GLfloat vertices[] = {// vertex coordinates
                          -1.0, -1.0, 0,
                          1.0, -1.0, 0,
                          0, 1.0, 0};


    // allocate buffer space and pass data to it
    glGenBuffers(1, &vboId);
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // unbind the active buffer
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    return vboId;
}

bool init_gl()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthRange(0.0, 0.1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // clear the framebuffer each frame with black color
    glClearColor(0, 0, 0, 0);
    loadDataInBuffers();
    return true;
}

// Function that does the drawing
// glut calls this function whenever it needs to redraw
void display()
{

    // clear the color buffer before each drawing
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw triangles starting from index 0 and
    // using 3 indices
    //

    GLint time_location = glGetUniformLocation(program_id, "time");
    float time = sin(glutGet(GLUT_ELAPSED_TIME) / 1000) / 2 + 0.5f;
    glUniform1f(time_location, time);


    GLint resoltion_location = glGetUniformLocation(program_id, "resolution");
    float height = GLUT_SCREEN_HEIGHT;
    float width = GLUT_SCREEN_WIDTH;
    glUniform2f(resoltion_location, height, width);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glVertexAttribPointer(
   0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
   3,                  // size
   GL_FLOAT,           // type
   GL_FALSE,           // normalized?
   0,                  // stride
   (void*)0            // array buffer offset
);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(0);


    // swap the buffers and hence show the buffers
    // content to the screen
    glutSwapBuffers();
    glutPostRedisplay();
}

bool init_glut(int &argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitContextVersion(4, 6);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(252, 252);
    glutInitWindowPosition(10, 10);
    glutCreateWindow("Test OpenGL - POGL");
    glutDisplayFunc(display);
    return true;
}

bool init_glew()
{
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
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

    bool ok = init_glut(argc, argv) && init_glew() && init_gl();
    if (!ok)
    {
        std::cerr << "init error" << std::endl;
        exit(EXIT_FAILURE);
    }

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
