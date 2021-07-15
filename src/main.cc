#include "GL/glew.h"
#include "GL/freeglut.h"
#include "GL/glu.h"

#include <glm/ext/vector_float3.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp> // glm::pi

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
GLuint VertexArrayID[2];

#define TEST_OPENGL_ERROR()                                                             \
  do {									\
    GLenum err = glGetError();					                        \
    if (err != GL_NO_ERROR) std::cerr << "OpenGL ERROR!" << __LINE__ << std::endl;      \
  } while(0)


void init_VAO()
{
    glGenVertexArrays(2, VertexArrayID);TEST_OPENGL_ERROR();
    glBindVertexArray(VertexArrayID[0]);TEST_OPENGL_ERROR();
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

 //   std::vector<GLfloat> vertices = {
    std::vector<GLfloat> vertices = {
    // positions
    -10.0f,  10.0f, -10.0f,
    -10.0f, -10.0f, -10.0f,
     10.0f, -10.0f, -10.0f,
     10.0f, -10.0f, -10.0f,
     10.0f,  10.0f, -10.0f,
    -10.0f,  10.0f, -10.0f,

    -10.0f, -10.0f,  10.0f,
    -10.0f, -10.0f, -10.0f,
    -10.0f,  10.0f, -10.0f,
    -10.0f,  10.0f, -10.0f,
    -10.0f,  10.0f,  10.0f,
    -10.0f, -10.0f,  10.0f,

     10.0f, -10.0f, -10.0f,
     10.0f, -10.0f,  10.0f,
     10.0f,  10.0f,  10.0f,
     10.0f,  10.0f,  10.0f,
     10.0f,  10.0f, -10.0f,
     10.0f, -10.0f, -10.0f,

    -10.0f, -10.0f,  10.0f,
    -10.0f,  10.0f,  10.0f,
     10.0f,  10.0f,  10.0f,
     10.0f,  10.0f,  10.0f,
     10.0f, -10.0f,  10.0f,
    -10.0f, -10.0f,  10.0f,

    -10.0f,  10.0f, -10.0f,
     10.0f,  10.0f, -10.0f,
     10.0f,  10.0f,  10.0f,
     10.0f,  10.0f,  10.0f,
    -10.0f,  10.0f,  10.0f,
    -10.0f,  10.0f, -10.0f,

    -10.0f, -10.0f, -10.0f,
    -10.0f, -10.0f,  10.0f,
     10.0f, -10.0f, -10.0f,
     10.0f, -10.0f, -10.0f,
    -10.0f, -10.0f,  10.0f,
     10.0f, -10.0f,  10.0f
};

    //activation of buffer, vbo type
    glBindBuffer(GL_ARRAY_BUFFER, vboId[1]);TEST_OPENGL_ERROR();

    //allocation
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);TEST_OPENGL_ERROR();

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

    glBindVertexArray(VertexArrayID[0]);
    
    GLint resoltion_location = glGetUniformLocation(program_id, "resolution");
    TEST_OPENGL_ERROR();
    glUniform2f(resoltion_location, 1, 1);TEST_OPENGL_ERROR();
}

void init_textures()
{

    std::vector<std::string> sky_faces = {"../textures/skybox_px.tga",
                                        "../textures/skybox_nx.tga",
                                        "../textures/skybox_py.tga",
                                        "../textures/skybox_ny.tga",
                                        "../textures/skybox_pz.tga",
                                        "../textures/skybox_nz.tga"
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

    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f/3.0f, 0.5f, 100.0f);

    // Or, for an ortho camera :
    //glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates

    // Camera matrix
    float radius = 10.0f;
    float camX = sin(time / 10) * radius;
    float camZ = cos(time / 10) * radius;
    //float camX = 4.5 * cos(0.5 * time);
    //float camZ = 4.5 * sin(0.5 * time);
    glm::mat4 View;

    glm::vec3 camera_position(0.f, 1.f, -4.f);
    glm::vec3 camera_look_at(0.f, 0.f, 0.f);
    glm::vec3 up (0.0f, 1.0f, 0.0f);

    View = glm::lookAt(
                        camera_position,//camera
                        camera_look_at,
                        up
                        );

    // Model matrix : an identity matrix (model will be at the origin)
    glm::mat4 Model = glm::mat4(1.0f);
    // Our ModelViewProjection : multiplication of our 3 matrices
    glm::mat4 mvp = Projection * View * Model;

    GLint mvp_location = glGetUniformLocation(program_id, "MVP");
    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, &mvp[0][0]);

    GLint look_at_location = glGetUniformLocation(program_id, "camera_look_at");
    glUniform3fv(look_at_location, 1, &camera_look_at[0]);

    GLint camera_location = glGetUniformLocation(program_id, "camera_position");
    glUniform3fv(camera_location, 1, &camera_position[0]);

    glutPostRedisplay();
}

void display()
{

    // clear the color buffer before each drawing
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(VertexArrayID[0]);TEST_OPENGL_ERROR();

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

    glutMainLoop();
    delete prog;
    return 0;
}
