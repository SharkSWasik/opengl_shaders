#include "GL/glew.h"
#include "GL/freeglut.h"
#include "GL/glu.h"

#include "program.hh"
#include "image.hh"
#include "image_io.hh"
#include "util.hh"
#include "matrix4.hh"


#include <GL/freeglut_std.h>
#include <cstdlib>
#include <math.h>
#include <vector>

//vbo for vertices and texture coords
GLuint program_id;
GLuint VertexArrayID;

#define TEST_OPENGL_ERROR()                                                             \
  do {									\
    GLenum err = glGetError();					                        \
    if (err != GL_NO_ERROR) std::cerr << "OpenGL ERROR!" << __LINE__ << std::endl;      \
  } while(0)

void init_VAO()
{
    glGenVertexArrays(1, &VertexArrayID);TEST_OPENGL_ERROR();
    glBindVertexArray(VertexArrayID);TEST_OPENGL_ERROR();
}

// init vertex buffer;
void init_VBO()
{
    GLuint vboId[2];
    GLint vertex_location = glGetAttribLocation(program_id, "position");TEST_OPENGL_ERROR();
    GLint uv_location = glGetAttribLocation(program_id, "uv");TEST_OPENGL_ERROR();

    // reservation of buffer
    init_VAO();
    glGenBuffers(2, vboId);TEST_OPENGL_ERROR();

    if (uv_location != -1)
    {

        std::vector<GLfloat> uv = {
            0, 1.5,
            0, 0,
            1, 0,
            0, 1.5,
            1, 1.5,
            1, 0,
        };

        //activation of buffer, vbo type
        glBindBuffer(GL_ARRAY_BUFFER, uv_location);TEST_OPENGL_ERROR();

        //allocation
        glBufferData(GL_ARRAY_BUFFER, uv.size() * sizeof(float), uv.data(), GL_STATIC_DRAW);TEST_OPENGL_ERROR();

        //how to read buffer
        glVertexAttribPointer(
                uv_location,        //location
                2,                  // size one vertex
                GL_FLOAT,           // type
                GL_FALSE,           // normalize
                0,                  // stride
                0            // array buffer offset
                );TEST_OPENGL_ERROR();

        glEnableVertexAttribArray(uv_location);TEST_OPENGL_ERROR();
    }

    //6 vertices for the entire screen (2 triangles)
    std::vector<GLfloat> vertices = {
    // positions
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};
    //activation of buffer, vbo type
    glBindBuffer(GL_ARRAY_BUFFER, vboId[1]);TEST_OPENGL_ERROR();

    //allocation
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);TEST_OPENGL_ERROR();

    //how to read buffer
    glVertexAttribPointer(
            vertex_location,    //location
            3,                  // size one vertex
            GL_FLOAT,           // type
            GL_FALSE,           // normalize
            0,                  // stride
            0            // array buffer offset
            );TEST_OPENGL_ERROR();

    glEnableVertexAttribArray(vertex_location);TEST_OPENGL_ERROR();

    glBindVertexArray(0);
}

void init_textures()
{
    std::vector<std::string> sky_faces = {"../textures/skybox_nz.tga",
                                        "../textures/skybox_nx.tga",
                                        "../textures/skybox_py.tga",
                                        "../textures/skybox_ny.tga",
                                        "../textures/skybox_px.tga",
                                        "../textures/skybox_pz.tga"
                                        };
    GLuint texture_id;
    GLint tex_location;

    glGenTextures(1, &texture_id);TEST_OPENGL_ERROR();
    glActiveTexture(GL_TEXTURE0);TEST_OPENGL_ERROR();

    //activation of texture
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

    for (int i = 0; i < 6; i++)
    {
        tifo::rgb24_image *sky = tifo::load_image(sky_faces[i].c_str());
        //generation of texture
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, sky->sx, sky->sy, 0, GL_RGB, GL_UNSIGNED_BYTE, sky->pixels);TEST_OPENGL_ERROR();
    }


    tex_location = glGetUniformLocation(program_id, "sky_sampler");TEST_OPENGL_ERROR();
    glUniform1i(tex_location, 0);TEST_OPENGL_ERROR();

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);TEST_OPENGL_ERROR();
}

void init_matrix()
{

    Mat camera;

    camera.look_at(camera, 4.0f, 3.0f, 3.0f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f);
    //camera.frucstum(camera, 1.57f, 0.52f, 4.0f, 3.0f, 0.1f, 100.0f);

    GLint mvp_location = glGetUniformLocation(program_id, "mvp");

    float view_ [16];

    for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
    {
        view_[i * 4 + j] = camera.data[i][j];
        std::cout << camera.data[i][j] << std::endl;
    }

    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, view_);
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

    glBindVertexArray(VertexArrayID);TEST_OPENGL_ERROR();

    glDrawArrays(GL_TRIANGLES, 0, 36);TEST_OPENGL_ERROR();

    glBindVertexArray(0);
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

   // init_VAO();
    init_VBO();
    init_textures();
   // init_matrix();

    glutMainLoop();
    delete prog;
    return 0;
}
